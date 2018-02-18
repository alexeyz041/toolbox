
#include <unistd.h>
#include "scaler.h"
#include "crop.h"


int Crop::init_filters(const char *filters_descr)
{
    char args[512];
    int ret = 0;
    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();

    AVRational time_base = { 1, 10 };
    AVRational sample_aspect_ratio = { 1,1 };
    enum AVPixelFormat pix_fmt = AV_PIX_FMT_ARGB;
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_ARGB, AV_PIX_FMT_NONE };

    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            width, height, pix_fmt,
            time_base.num, time_base.den,
            sample_aspect_ratio.num, sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in", args, NULL, filter_graph);
    if (ret < 0) {
    	fprintf(stderr, "Cannot create buffer source\n");
        goto end;
    }
    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
    	fprintf(stderr, "Cannot create buffer sink\n");
        goto end;
    }
    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
    	fprintf(stderr, "Cannot set output pixel format\n");
        goto end;
    }
    /*
     * Set the endpoints for the filter graph. The filter_graph will
     * be linked to the graph described by filters_descr.
     */
    /*
     * The buffer source output must be connected to the input pad of
     * the first filter described by filters_descr; since the first
     * filter input label is not specified, it is set to "in" by
     * default.
     */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;
    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;
    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                    &inputs, &outputs, NULL)) < 0)
        goto end;
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;
end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    return ret;
}


void Crop::crop(uint8_t *ibuf,int ilen,uint8_t *obuf,int olen)
{
	int len = av_image_get_buffer_size(AV_PIX_FMT_ARGB,frame->width,frame->height,32);
	if(len != ilen) {
		fprintf(stderr,"wrong input length = %d vs %d\n",len,ilen);
		exit(1);
	}
	int ret = av_image_fill_arrays(frame->data, frame->linesize,
	                             ibuf,
								 AV_PIX_FMT_ARGB, frame->width, frame->height, 32);
	if(ret < 0) {
	 	fprintf(stderr, "av_image_fill_arrays failed\n");
	 	exit(1);
	}

	frame->pts = frame_cnt++;
	/* push the decoded frame into the filtergraph */
	if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
		fprintf(stderr,"Error while feeding the filtergraph\n");
		exit(1);
	}
	/* pull filtered frames from the filtergraph */
	ret = av_buffersink_get_frame(buffersink_ctx, frame2);
	if (ret < 0) {
	  	fprintf(stderr,"Error while pulling frames from the filtergraph\n");
	  	exit(1);
	}

	sws_context = sws_getCachedContext(sws_context,
	            frame2->width, frame2->height, AV_PIX_FMT_BGRA,
	            frame3->width, frame3->height, AV_PIX_FMT_YUV420P,
	            0, NULL, NULL, NULL);

	sws_scale(sws_context,frame2->data,frame2->linesize,0,frame3->height,frame3->data,frame3->linesize);

	len = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,frame3->width,frame3->height,32);
	if(len != olen) {
		fprintf(stderr,"wrong output length = %d vs %d\n",len,olen);
		exit(1);
	}

	ret = av_image_copy_to_buffer(obuf,
							olen,
		                    frame3->data,
							frame3->linesize,
							AV_PIX_FMT_YUV420P,
							frame3->width,
							frame3->height,
							32);

	if (ret < 0) {
	  	fprintf(stderr,"av_image_copy_to_buffer failed\n");
	  	exit(1);
	}
	av_frame_unref(frame2);
}




Crop::Crop(int width,int height,int offx,int offy,int width2,int height2) : width(width), height(height), width2(width2), height2(height2),
		frame(nullptr),frame2(nullptr),buffersink_ctx(nullptr),buffersrc_ctx(nullptr),filter_graph(nullptr),frame_cnt(0),sws_context(nullptr)
{
	char filter_descr[64] = {0};
	snprintf(filter_descr,sizeof(filter_descr),"crop=%d:%d:%d:%d",width2,height2,offx,offy);

	avfilter_register_all();
	ffmpeg_encoder_init_frame(&frame, width, height, AV_PIX_FMT_ARGB);
	//ffmpeg_encoder_init_frame(&frame2, width2, height2, AV_PIX_FMT_ARGB);
	frame2 = av_frame_alloc();
	ffmpeg_encoder_init_frame(&frame3, width2, height2, AV_PIX_FMT_YUV420P);
	if(init_filters(filter_descr) < 0) {
		fprintf(stderr,"init_filters failed");
		exit(1);
	}
}



Crop::~Crop()
{
	av_frame_free(&frame);
	av_frame_free(&frame2);
}



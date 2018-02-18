
#include "scaler.h"



void ffmpeg_encoder_init_frame(AVFrame **framep, int width, int height,int pix_fmt)
{
    int ret = 0;
    AVFrame *frame = nullptr;
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = pix_fmt;
    frame->width  = width;
    frame->height = height;
    ret = av_image_alloc(frame->data, frame->linesize, frame->width, frame->height, (AVPixelFormat)frame->format, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }
    *framep = frame;
}


void Scaler::ffmpeg_encoder_set_frame_yuv_from_rgb(uint8_t *rgb)
{
    const int in_linesize[1] = { 4*frame->width };
    const uint8_t * const in_rgb[1] = { rgb };

    sws_context = sws_getCachedContext(sws_context,
            frame->width, frame->height, AV_PIX_FMT_BGRA,
            frame->width, frame->height, AV_PIX_FMT_YUV420P,
            0, NULL, NULL, NULL);

    sws_scale(sws_context,
    		in_rgb,
			in_linesize,
			0,
            frame->height,
			frame->data,
			frame->linesize);
}


void Scaler::ffmpeg_encoder_scale()
{
    sws_context = sws_getCachedContext(sws_context,
            frame->width, frame->height, AV_PIX_FMT_YUV420P,
            frame2->width, frame2->height, AV_PIX_FMT_YUV420P,
			SWS_FAST_BILINEAR, NULL, NULL, NULL);

    sws_scale(sws_context, (const uint8_t * const *)frame->data, frame->linesize, 0,
            frame->height, frame2->data, frame2->linesize);
}

//=============================================================================

Scaler::Scaler(int width,int height,int width2,int height2) : sws_context(nullptr)
{
	avcodec_register_all();
	ffmpeg_encoder_init_frame(&frame, width, height, AV_PIX_FMT_YUV420P);
    ffmpeg_encoder_init_frame(&frame2, width2, height2,AV_PIX_FMT_YUV420P);
}


void Scaler::scale(uint8_t *ibuf,int ilen,uint8_t *obuf,int olen)
{
	int len = av_image_get_buffer_size(AV_PIX_FMT_ARGB,frame->width,frame->height,32);
	if(len != ilen) {
		fprintf(stderr,"scale: wrong input length %d vs %d\n",ilen,len);
		exit(1);
	}

	ffmpeg_encoder_set_frame_yuv_from_rgb(ibuf);	// ibuf -> frame
	ffmpeg_encoder_scale();							// frame -> frame2

	len = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,frame2->width,frame2->height,32);
	if(len != olen) {
		fprintf(stderr,"scale: wrong output length %d vs %d\n",olen,len);
		exit(1);
	}

	int ret = av_image_copy_to_buffer(obuf,
							olen,
	                        frame2->data,
							frame2->linesize,
							AV_PIX_FMT_YUV420P,
							frame2->width,
							frame2->height,
							32);
	if(ret < 0) {
		fprintf(stderr,"scale: av_image_copy_to_buffer failed\n");
		exit(1);
	}
}


Scaler::~Scaler()
{
    av_frame_free(&frame);
    av_frame_free(&frame2);
}



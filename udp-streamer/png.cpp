
#include "png.h"

static void ffmpeg_encoder_init_frame(AVFrame **framep, int width, int height,int pix_fmt)
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



void Png::ffmpeg_encoder_set_frame_rgb24_from_bgra(uint8_t *rgb)
{
    const int in_linesize[1] = { 4*frame->width };
    const uint8_t * const in_rgb[1] = { rgb };

    sws_context = sws_getCachedContext(sws_context,
            frame->width, frame->height, AV_PIX_FMT_BGRA,
            frame->width, frame->height, AV_PIX_FMT_RGB24, //AV_PIX_FMT_YUV420P,
            0, NULL, NULL, NULL);

    sws_scale(sws_context,
    		in_rgb,
			in_linesize,
			0,
            frame->height,
			frame->data,
			frame->linesize);
}


int Png::compress(uint8_t *buf,uint8_t **obuf,int *olen)
{
	ffmpeg_encoder_set_frame_rgb24_from_bgra(buf);

	if(packet.data) {
		av_free_packet(&packet);
		packet.data = nullptr;
		packet.size = 0;
	}

    av_init_packet(&packet);
    int gotFrame = 0;
    if (avcodec_encode_video2(pngContext, &packet, frame, &gotFrame) < 0) {
        return -1;
    }

    *obuf = packet.data;
    *olen = packet.size;
    return 0;
}

//=============================================================================

Png::Png(int width,int height) : width(width), height(width), sws_context(nullptr)
{
	packet.data = nullptr;
	packet.size = 0;

	avcodec_register_all();
	ffmpeg_encoder_init_frame(&frame, width, height, AV_PIX_FMT_RGB24 /*AV_PIX_FMT_YUV420P*/);

    pngCodec = avcodec_find_encoder(AV_CODEC_ID_PNG);
    if (!pngCodec) {
        return;
    }
    pngContext = avcodec_alloc_context3(pngCodec);
    if (!pngContext) {
        return;
    }

    pngContext->pix_fmt = AV_PIX_FMT_RGB24; //AV_PIX_FMT_YUV420P;
    pngContext->height = height;
    pngContext->width = width;

    if (avcodec_open2(pngContext, pngCodec, NULL) < 0) {
        return;
    }
}



Png::~Png()
{
	if(packet.data) {
		av_free_packet(&packet);
		packet.data = nullptr;
	}
    avcodec_close(pngContext);
	av_freep(&frame->data[0]);
    av_frame_free(&frame);
}




#include "decoder.h"

static void ffmpeg_decoder_init_frame(AVFrame **framep, int width, int height,int pix_fmt)
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


Decoder::Decoder(int width,int height) : codec(nullptr), c(nullptr), frame(nullptr), frame2(nullptr),
										width(width), height(height), sws_context(nullptr), buffer(nullptr)
{
	av_register_all();
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
    	fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    c->width = width;
    c->height = height;
    c->extradata = NULL;
    c->pix_fmt = AV_PIX_FMT_YUV420P;
    c->flags2 |= AV_CODEC_FLAG2_CHUNKS;

    //if(codec->capabilities&CODEC_CAP_TRUNCATED)
    //    c->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    ffmpeg_decoder_init_frame(&frame,width,height,AV_PIX_FMT_YUV420P);
    ffmpeg_decoder_init_frame(&frame2,width,height,AV_PIX_FMT_BGRA);
    av_init_packet(&avpkt);

    buffer = (uint8_t *)malloc(width*height*4);
    if(!buffer) {
    	fprintf(stderr,"out of memory");
    	exit(1);
    }
}



Decoder::~Decoder()
{
    avcodec_close(c);
    av_free(c);
    av_frame_free(&frame);
    if(buffer) {
    	free(buffer);
    	buffer = nullptr;
    }
}


void Decoder::write_frame()
{
	sws_context = sws_getCachedContext(sws_context,
            frame->width, frame->height, AV_PIX_FMT_YUV420P,
            frame2->width, frame2->height, AV_PIX_FMT_BGRA,
            0, NULL, NULL, NULL);

	sws_scale(sws_context,
    		frame->data,
			frame->linesize,
			0,
            frame->height,
			frame2->data,
			frame2->linesize);

	int olen = width*height*4;
	int len = av_image_get_buffer_size(AV_PIX_FMT_BGRA,frame2->width,frame2->height,32);
	if(len != olen) {
		printf("len = %d olen = %d\n",len,olen);
	}

	av_image_copy_to_buffer(buffer,
							olen,
	                        frame2->data,
							frame2->linesize,
							AV_PIX_FMT_BGRA,
							frame2->width,
							frame2->height,
							32);
	write(buffer,olen);
}


int Decoder::decode(uint8_t* inbuf,int inlen)
{
	avpkt.size = inlen;
	avpkt.data = inbuf;
    while (avpkt.size > 0) {
        int len = 0;
        int got_frame = 0;
        len = avcodec_decode_video2(c, frame, &got_frame, &avpkt);
        if (len < 0) {
             fprintf(stderr, "Error while decoding frame\n");
             return len;
        }
        if (got_frame) {
        	write_frame();
        }
        if (avpkt.data) {
            avpkt.size -= len;
            avpkt.data += len;
        }
    }
    return 0;
}




#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include "scaler.h"
#include "encoder2.h"


int Encoder2::init()
{
    int ret = 0;
    AVCodec *codec = nullptr;

    avcodec_register_all();
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        return -1;
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        return -1;
    }

    c->bit_rate = 1000000;
    c->width = width;
    c->height = height;
    c->time_base.num = 1;
    c->time_base.den = fps;
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

//    if (codec_id == AV_CODEC_ID_H264)
    av_opt_set(c->priv_data, "preset", "ultrafast", 0);
    av_opt_set(c->priv_data, "tune", "zerolatency", 0);
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return -1;
    }

    ffmpeg_encoder_init_frame(&frame, width, height, AV_PIX_FMT_YUV420P);
    return 0;
}


void Encoder2::release()
{
    int got_output = 0, ret = 0;
    do {
        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;

        ret = avcodec_encode_video2(c, &pkt, NULL, &got_output);
        if (ret < 0) {
            fprintf(stderr, "Error encoding frame\n");
            exit(1);
        }
        if (got_output) {
            //fwrite(pkt.data, 1, pkt.size, file);
            av_packet_unref(&pkt);
        }
    } while (got_output);

    avcodec_close(c);
    av_free(c);
    c = nullptr;

    av_frame_free(&frame);
    frame = nullptr;
}


int Encoder2::encode(uint8_t *buf,int len,uint64_t ts)
{
    int ret=0, got_output=0;

    int ilen = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,frame->width,frame->height,32);
    if(len != ilen) {
    	fprintf(stderr, "Wrong input data length %d vs %d\n",len,ilen);
    	return -1;
    }

    ret = av_image_fill_arrays(frame->data, frame->linesize,
                             buf,
							 AV_PIX_FMT_YUV420P, frame->width, frame->height, 32);
    if(ret < 0) {
    	fprintf(stderr, "av_image_fill_arrays failed\n");
    	return -1;
    }

    frame->pts = ts/frame_time;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    ret = avcodec_encode_video2(c, &pkt, frame, &got_output);
    if (ret < 0) {
        fprintf(stderr, "Error encoding frame\n");
        return -1;
    }
    if (got_output) {
        write(pkt.data, pkt.size, pkt.pts * frame_time);
        av_packet_unref(&pkt);
    }
    return 0;
}


//=============================================================================

Encoder2::Encoder2(int w,int h,int fps) : width(w), height(h), fps(fps), frame_time(0), c(nullptr), frame(nullptr)
{
	frame_time = 1000000 / fps;
    if(init() != 0) {
    	fprintf(stderr,"init failed\n");
    	exit(1);
    }
}

Encoder2::~Encoder2()
{
	release();
}


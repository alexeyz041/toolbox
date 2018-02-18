
#ifndef SCALER_H
#define SCALER_H

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
    #include <libswscale/swscale.h>
}


class Scaler {
public:
	Scaler(int width,int height,int width2,int height2);
	~Scaler();
	void scale(uint8_t *ibuf,int ilen,uint8_t *obuf,int olen);

	static int getBufferSize(int width,int height)
	{
		return av_image_get_buffer_size(AV_PIX_FMT_YUV420P,width,height,32);
	}

private:
	void ffmpeg_encoder_set_frame_yuv_from_rgb(uint8_t *rgb);
	void ffmpeg_encoder_scale(void);

private:
	AVFrame *frame;
	AVFrame *frame2;
	struct SwsContext *sws_context;
};

void ffmpeg_encoder_init_frame(AVFrame **framep, int width, int height,int pix_fmt);

#endif //SCALER_H



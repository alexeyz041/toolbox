
#ifndef PNG_H
#define PNG_H

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
    #include <libswscale/swscale.h>
}


class Png {
public:
	Png(int width,int height);
	~Png();
	int compress(uint8_t *buf,uint8_t **obuf,int *olen);

private:
	void ffmpeg_encoder_set_frame_rgb24_from_bgra(uint8_t *rgb);

private:
	int width;
	int height;
	AVFrame *frame;
	struct SwsContext *sws_context;
	AVCodec *pngCodec;
	AVCodecContext *pngContext;
	AVPacket packet;
};

#endif //PNG_H


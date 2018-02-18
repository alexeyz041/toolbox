#ifndef CROP_H
#define CROP_H

extern "C" {
	#include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
	#include <libavfilter/buffersink.h>
	#include <libavfilter/buffersrc.h>
	#include <libavutil/opt.h>
	#include <libswscale/swscale.h>
}


class Crop {
public:
	Crop(int width,int height,int offx,int offy,int width2,int height2);
    ~Crop();
    void crop(uint8_t *ibuf,int ilen,uint8_t *obuf,int olen);

private:
    int init_filters(const char *filters_descr);

private:
    int width;
    int height;
    int width2;
    int height2;

    AVFrame *frame;
    AVFrame *frame2;
    AVFrame *frame3;
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
    int frame_cnt;
    struct SwsContext *sws_context;
};


#endif //CROP_H

#ifndef GST_H_INCLUDED
#define GST_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int	width;
    int height;
    int bpp;
    char ip[32];
    int port;
} GST_CONFIG;

void gst_main(void *ctx,GST_CONFIG *cfg);
void get_image(void *ctx,void *buf,int size);

#ifdef __cplusplus
}
#endif

#endif //GST_H_INCLUDED

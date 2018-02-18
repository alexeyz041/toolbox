
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include "encoder.h"


int Encoder::init()
{
char fpss[32] = {0};
	snprintf(fpss,sizeof(fpss),"%d",fps);

    /* Get default params for preset/tuning */
    if( x264_param_default_preset( &param, "ultrafast", "zerolatency") < 0 )
        return -1;

    /* Configure non-default params */
    param.i_bitdepth = 8;
    param.i_csp = X264_CSP_I420;
    param.i_width  = width;
    param.i_height = height;
    param.b_vfr_input = 0;
    param.b_repeat_headers = 1;
    param.b_annexb = 1;

    /* Apply profile restrictions. */
    if( x264_param_apply_profile( &param, "high" ) < 0 )
        return -1;

    if( x264_param_parse(&param, "fps" , fpss) != 0)
    	return -1;
    if( x264_param_parse(&param, "keyint" , fpss) != 0)
    	return -1;

    if( x264_picture_alloc( &pic, param.i_csp, param.i_width, param.i_height ) < 0 )
        return -1;

    h = x264_encoder_open( &param );
    if(!h) {
    	x264_picture_clean( &pic );
    	return -1;
    }
    return 0;
}


int Encoder::encode(uint8_t *buf,int size,uint64_t ts)
{
    int i_frame_size = 0;
    int luma_size = width * height;
    int chroma_size = luma_size / 4;

 //   printf("encode...");

    if(size != luma_size + 2*chroma_size)
    	return -1;

    /* Encode frames */
    {
        /* Read input frame */
        memcpy(pic.img.plane[0], buf, luma_size);
        memcpy(pic.img.plane[1], buf+luma_size, chroma_size);
        memcpy(pic.img.plane[2], buf+luma_size+chroma_size, chroma_size);

        pic.i_pts = ts;
        i_frame_size = x264_encoder_encode( h, &nal, &i_nal, &pic, &pic_out );
        if( i_frame_size < 0 )
            return -1;
        else if( i_frame_size )
        {
            write( nal->p_payload, i_frame_size, pic.i_pts );
        }
    }

//    printf("ok\n");
    return 0;
}


void Encoder::release()
{
    /* Flush delayed frames */
    while( x264_encoder_delayed_frames( h ) )
    {
//        int i_frame_size = 
	x264_encoder_encode( h, &nal, &i_nal, NULL, &pic_out );
//        if( i_frame_size < 0 )
//            goto fail;
//        else if( i_frame_size )
//        {
//           if( !fwrite( nal->p_payload, i_frame_size, 1, f ) )
//               goto fail;
//        }
    }

    x264_encoder_close( h );
    x264_picture_clean( &pic );
}

//=============================================================================

Encoder::Encoder(int w,int h,int fps) : width(w), height(h), fps(fps), i_frame(0), i_nal(0)
{
    if(init() != 0) {
    	fprintf(stderr,"init failed\n");
    	exit(1);
    }
}

Encoder::~Encoder()
{
	release();
}


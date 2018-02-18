
#include "screenshot.h"




void initimage( struct shmimage * image )
{
    image->ximage = NULL ;
    image->shminfo.shmaddr = (char *) -1 ;
}


void destroyimage( Display * dsp, struct shmimage * image )
{
    if( image->ximage )
    {
        XShmDetach( dsp, &image->shminfo ) ;
        XDestroyImage( image->ximage ) ;
        image->ximage = NULL ;
    }

    if( image->shminfo.shmaddr != ( char * ) -1 )
    {
        shmdt( image->shminfo.shmaddr ) ;
        image->shminfo.shmaddr = ( char * ) -1 ;
    }
}


int createimage( Display * dsp, struct shmimage * image, int width, int height )
{
    // Create a shared memory area 
    image->shminfo.shmid = shmget( IPC_PRIVATE, width * height * BPP, IPC_CREAT | 0600 ) ;
    if( image->shminfo.shmid == -1 )
    {
        perror( "screenshot" ) ;
        return false ;
    }

    // Map the shared memory segment into the address space of this process
    image->shminfo.shmaddr = (char *) shmat( image->shminfo.shmid, 0, 0 ) ;
    if( image->shminfo.shmaddr == (char *) -1 )
    {
        perror( "screenshot 2" ) ;
        return false ;
    }

    image->data = (unsigned int*) image->shminfo.shmaddr ;
    image->shminfo.readOnly = false ;

    // Mark the shared memory segment for removal
    // It will be removed even if this program crashes
    shmctl( image->shminfo.shmid, IPC_RMID, 0 ) ;

    // Allocate the memory needed for the XImage structure
    image->ximage = XShmCreateImage( dsp, XDefaultVisual( dsp, XDefaultScreen( dsp ) ),
                        DefaultDepth( dsp, XDefaultScreen( dsp ) ), ZPixmap, 0,
                        &image->shminfo, 0, 0 ) ;
    if( !image->ximage )
    {
        destroyimage( dsp, image ) ;
        printf("could not allocate the XImage structure\n" ) ;
        return false ;
    }

    image->ximage->data = (char *)image->data ;
    image->ximage->width = width ;
    image->ximage->height = height ;

    // Ask the X server to attach the shared memory segment and sync
    XShmAttach( dsp, &image->shminfo ) ;
    XSync( dsp, false ) ;
    return true ;
}


void getrootwindow( Display * dsp, struct shmimage * image )
{
    XShmGetImage( dsp, XDefaultRootWindow( dsp ), image->ximage, 0, 0, AllPlanes ) ;
    // This is how you access the image's BGRA packed pixels
    // Lets set the alpha channel of each pixel to 0xff
    int x, y ;
    unsigned int * p = image->data ;
    for( y = 0 ; y < image->ximage->height; ++y )
    {
        for( x = 0 ; x < image->ximage->width; ++x )
        {
            *p++ |= 0xff000000 ;
        }
    }
}


/*
void initpngimage( png_image * pi, struct shmimage * image )
{
    bzero( pi, sizeof( png_image ) ) ;
    pi->version = PNG_IMAGE_VERSION ;
    pi->width = image->ximage->width ;
    pi->height = image->ximage->height ;
    pi->format = PNG_FORMAT_BGRA ;
}


int savepng( struct shmimage * image, char * path )
{
    FILE * f = fopen( path, "w" ) ;
    if( !f )
    {
        perror( "screenshot" ) ;
        return false ;
    }
    png_image pi ;
    initpngimage( &pi, image ) ;
    unsigned int scanline = pi.width * BPP ;
    if( !png_image_write_to_stdio( &pi, f, 0, image->data, scanline, NULL) )
    {
        fclose( f ) ;
        printf( "could not save the png image\n" ) ;
        return false ;
    }
    fclose( f ) ;
    return true ;
}

*/


int ScreenShot::init()
{
    dsp = XOpenDisplay( NULL ) ;
    if( !dsp )
    {
        printf("could not open a connection to the X server\n" ) ;
        return -1;
    }

    if( !XShmQueryExtension( dsp ) )
    {
        XCloseDisplay( dsp ) ;
        printf("the X server does not support the XSHM extension\n" ) ;
        return -1 ;
    }

    int screen = XDefaultScreen( dsp ) ;
    initimage( &image ) ;
    if( !createimage( dsp, &image, XDisplayWidth( dsp, screen ), XDisplayHeight( dsp, screen ) ) )
    {
        XCloseDisplay( dsp ) ;
        return -1;
    }
    return 0;
}



void ScreenShot::get(int *width,int *height,uint32_t **buf)
{
    getrootwindow(dsp, &image );

    *width = image.ximage->width;
    *height = image.ximage->height;
    *buf = image.data;

//    printf("%d x %d\n", image.ximage->width,image.ximage->height);
}


void ScreenShot::release()
{
    destroyimage( dsp, &image ) ;
    XCloseDisplay( dsp ) ;
}

//=============================================================================

ScreenShot::ScreenShot()
{
	if(init() != 0) {
		printf("screenshot init failed\n");
	}
}

ScreenShot::~ScreenShot()
{
	release();
}



#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <png.h>

#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>

#define NAME  "screenshot"
#define BPP   4


struct shmimage {
    XShmSegmentInfo shminfo ;
    XImage * ximage ;
    unsigned int * data ; // will point to the image's BGRA packed pixels
};


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
        perror( NAME ) ;
        return false ;
    }

    // Map the shared memory segment into the address space of this process
    image->shminfo.shmaddr = (char *) shmat( image->shminfo.shmid, 0, 0 ) ;
    if( image->shminfo.shmaddr == (char *) -1 )
    {
        perror( NAME ) ;
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
        printf( NAME ": could not allocate the XImage structure\n" ) ;
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
        perror( NAME ) ;
        return false ;
    }
    png_image pi ;
    initpngimage( &pi, image ) ;
    unsigned int scanline = pi.width * BPP ;
    if( !png_image_write_to_stdio( &pi, f, 0, image->data, scanline, NULL) )
    {
        fclose( f ) ;
        printf( NAME ": could not save the png image\n" ) ;
        return false ;
    }
    fclose( f ) ;
    return true ;
}



int main( int argc, char * argv[] )
{
    if( argc != 2 )
    {
        printf( "Usage:\n" ) ;
        printf( "      " NAME " file\n\n" ) ;
        return 0 ;
    }

    Display * dsp = XOpenDisplay( NULL ) ;
    if( !dsp )
    {
        printf( NAME ": could not open a connection to the X server\n" ) ;
        return 1 ;
    }

    if( !XShmQueryExtension( dsp ) )
    {
        XCloseDisplay( dsp ) ;
        printf( NAME ": the X server does not support the XSHM extension\n" ) ;
        return 1 ;
    }

    int screen = XDefaultScreen( dsp ) ;
    struct shmimage image ;
    initimage( &image ) ;
    if( !createimage( dsp, &image, XDisplayWidth( dsp, screen ), XDisplayHeight( dsp, screen ) ) )
    {
        XCloseDisplay( dsp ) ;
        return 1 ;
    }

    getrootwindow( dsp, &image ) ;

    printf("%d x %d\n", image.ximage->width,image.ximage->height);

    if( !savepng( &image, argv[1] ) )
    {
        destroyimage( dsp, &image ) ;
        XCloseDisplay( dsp ) ;
        return 1 ;
    }

    destroyimage( dsp, &image ) ;
    XCloseDisplay( dsp ) ;
    return 0 ;
}


#include "screen.h"
#include <unistd.h>


int Screen::init()
{
	//Initialize SDL
	if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
		fprintf(stderr,"SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	//Create window
	gWindow = SDL_CreateWindow("Screen",
								SDL_WINDOWPOS_UNDEFINED,
								SDL_WINDOWPOS_UNDEFINED,
								width,
								height,
								SDL_WINDOW_SHOWN );
	if( gWindow == NULL ) {
		fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	//Get window surface
	gScreenSurface = SDL_GetWindowSurface( gWindow );

	// clear screen
	SDL_Surface *windowFillSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	SDL_FillRect( windowFillSurface, NULL, 0 );
	SDL_BlitSurface(windowFillSurface, NULL, gScreenSurface, NULL);
	SDL_UpdateWindowSurface( gWindow );
    SDL_FreeSurface(windowFillSurface);
	return 0;
}


void Screen::close()
{
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	SDL_Quit();
}



//=============================================================================

void *ui_thread(void *p)
{
	Screen *s = (Screen *)p;

	if(s->init() != 0) {
		fprintf(stderr,"screen failed to initialize!\n");
	}

	SDL_Event event;
	for(; !s->quit; ) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				s->quit = true;
			}
		}
		usleep(1000);
	}

	s->close();
	return NULL;
}


Screen::Screen(int width,int height) : width(width), height(height),
									gWindow(nullptr), gScreenSurface(nullptr), quit(false)
{
	if(pthread_create(&thread_id, NULL, ui_thread, this) < 0) {
		fprintf(stderr,"pthread_create failed\n");
	}
}


Screen::~Screen()
{
	quit = true;
	pthread_join(thread_id,NULL);
}


void Screen::show(uint8_t *buf,int size)
{
	uint32_t amask = 0xff000000;
	uint32_t rmask = 0x00ff0000;
	uint32_t gmask = 0x0000ff00;
	uint32_t bmask = 0x000000ff;
	SDL_Surface *temp = SDL_CreateRGBSurfaceFrom(buf, width, height, 32, width*4, rmask, gmask, bmask, amask);

	SDL_BlitSurface( /*image*/temp, NULL, gScreenSurface, NULL );

	//Update the surface
	SDL_UpdateWindowSurface( gWindow );
    //Free the temporary surface
    SDL_FreeSurface(temp);
}





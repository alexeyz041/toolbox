
#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <pthread.h>

class Screen {
public:
	Screen(int width,int height);
	~Screen();
	void show(uint8_t *buf,int size);

	int init(void);
	void close(void);

	int width;
	int height;
	SDL_Window *gWindow;
	SDL_Surface *gScreenSurface;
	bool quit;
	pthread_t thread_id;
};


#endif //SCREEN_H



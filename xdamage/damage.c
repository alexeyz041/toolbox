
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xdamage.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xfixes.h>


Window *winlist(Display *disp, unsigned long *len)
{
    Atom prop = XInternAtom(disp,"_NET_CLIENT_LIST",False);
    Atom type;
    int form;
    unsigned long remain;
    unsigned char *list;

    if (XGetWindowProperty(disp,XDefaultRootWindow(disp),prop,0,1024,False,XA_WINDOW,&type,&form,len,&remain,&list) != Success) {
        perror("winlist - GetWinProp failed");
        return NULL;
    }
    return (Window*)list;
}


char *winname(Display *disp, Window win)
{
    Atom prop = XInternAtom(disp,"WM_NAME",False);
    Atom type;
    int form;
    unsigned long remain;
    unsigned long len;
    unsigned char *list;

    if (XGetWindowProperty(disp,win,prop,0,1024,False,XA_STRING,&type,&form,&len,&remain,&list) != Success) {
        perror("winname - GetWinProp failed");
        return NULL;
    }
    return (char*)list;
}


int main(int argc,char **argv)
{
    int found = 0;
    Display* display = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(display);

    if(argc > 1) {
        unsigned long len = 0;
	Window *list = (Window *)winlist(display,&len);
        for(int i=0; i < len; i++) {
	    char *name = winname(display,list[i]);
    	    printf("window [%s]\n",name);
	    if(strstr(name,argv[1])) {
		printf("selecting %s\n",name);
		root = list[i];
		found = 1;
	    }
	    free(name);
	}
	XFree(list);
	if(!found) return 1;
    }
    
    XWindowAttributes attributes = {0};
    XGetWindowAttributes(display, root, &attributes);

    int width = attributes.width;
    int height = attributes.height;
    printf("size %dx%d\n",width,height);

    int damage_event = 0, damage_error = 0;
    int test = XDamageQueryExtension(display, &damage_event, &damage_error);
    Damage damage = XDamageCreate(display, root, XDamageReportRawRectangles /*XDamageReportNonEmpty XDamageReportBoundingBox XDamageReportDeltaRectangles*/);

//    XserverRegion region = XFixesCreateRegion(display, NULL, 0);
    for(;;) {
        XEvent event;
	while(XPending(display)) {
    	    XNextEvent(display,&event);
	    if(event.type == damage_event+XDamageNotify) {
		XDamageNotifyEvent *dev = (XDamageNotifyEvent*)&event; 			/* if type matches, cast to XDamageNotify  event */
		//printf("Damage notification in window %d\n", dev->drawable);

		printf("Rectangle x=%d, y=%d, w=%d, h=%d\n",
                				dev->area.x, dev->area.y, 
                				dev->area.width, dev->area.height);
	    }
	}

	XDamageSubtract(display,damage,None,None /*region*/);
/*
	int count = 0;
	XRectangle *area = XFixesFetchRegion(display, region, &count);
	if(area) {
	    for(int i=0; i < count; i++) {
		XRectangle rect = area[i];
    		printf("Rect x=%d, y=%d, w=%d, h=%d\n",	rect.x, rect.y,rect.width, rect.height);
	    }
	    XFree(area);
	}
	XFlush(display);
*/
	usleep(1000);
    }
//    XFixesDestroyRegion(display, region);
    XCloseDisplay(display);
    return 0;
}



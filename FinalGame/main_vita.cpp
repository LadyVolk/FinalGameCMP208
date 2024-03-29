#include <platform/vita/system/platform_vita.h>
#include "scene_app.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <menu_app.h>

unsigned int sceLibcHeapSize = 128*1024*1024;	// Sets up the heap area size as 128MiB.

int main(void)
{
	srand(time(NULL));
	// initialisation
	gef::PlatformVita platform;

	//SceneApp myApp(platform);
	//myApp.Run();

	MenuApp myApp(platform);
	myApp.Run();

	return 0;
}
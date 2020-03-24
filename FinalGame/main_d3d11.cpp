#include <platform/d3d11/system/platform_d3d11.h>
#include "scene_app.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "menu_app.h"
#include <system/debug_log.h>

unsigned int sceLibcHeapSize = 128*1024*1024;	// Sets up the heap area size as 128MiB.

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	srand(time(NULL));
	// initialisation
	gef::PlatformD3D11 platform(hInstance, 960, 544, false, true);

	MenuApp myApp(platform);
	myApp.Run();

	SceneApp gameApp(platform, SceneApp::easy );
	gameApp.Run();

	return 0;
}
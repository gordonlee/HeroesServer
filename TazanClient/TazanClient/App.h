#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#include <objbase.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus")
using namespace Gdiplus ;

#include <Mmsystem.h>
#pragma comment( lib, "winmm.lib" ) 

#define App_Name TEXT("Tazan Client Miner Ver")
#define App_ClassName TEXT("Tazan Client Miner Ver Class")

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

const int g_tileSize = 20;
const int g_appWidth = g_tileSize * 30;
const int g_appHeight = g_tileSize * 30;
const int g_framePerSecond = 30;
const int g_fpsElapsed = 1000 / g_framePerSecond;
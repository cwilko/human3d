
// Init.cpp
//
// Contains all the global variable declarations, include files, macros and defines
//
// 

// INCLUDES /////////////////////////

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <ddraw.h>
#include <d3d.h>
#include <stdlib.h>
#include <stdio.h>

#include <fstream.h>
#include <time.h>


// DEFINES //////////////////////////

#define WINDOW_CLASS_NAME "WINDOW_CLASS"
#define SCREEN_HEIGHT 768						// Screen resolution height
#define SCREEN_WIDTH 1024						// Screen resolution width
#define SCREEN_BPP 16							// Colour depth

#define VIEW_DISTANCE 10.0						// Observer distance from viewplane

// MACROS ///////////////////////////

#define PI              ((double)3.14159265)
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1:0)


// STRUCTURES ///////////////////////


// PROTOTYPES ///////////////////////

int DD_Init(HWND hwnd);								// Initialise DirectX
int DD_Shutdown(HWND hwnd);							// Terminate DirectX
VOID ShowStats();									
								


// Main windows callback function

LRESULT CALLBACK WindowFunc(HWND, UINT, WPARAM, LPARAM) ;


// GLOBAL VARIABLES //////////////////////////////



// DirectX global variables

static LPDIRECTDRAW pdd							= NULL;
static LPDIRECTDRAW4 lpdd						= NULL;
static LPDIRECTDRAWSURFACE4 lpddsprimary		= NULL;
static LPDIRECTDRAWSURFACE4 lpddsback			= NULL;
static LPDIRECTDRAWSURFACE4 lpddsRenderTarget	= NULL;
static LPDIRECTDRAWSURFACE4 lpddsZBuffer		= NULL;
static LPDIRECT3D3          lpD3D				= NULL;
static LPDIRECT3DDEVICE3    lpd3dDevice			= NULL;
static LPDIRECT3DVIEWPORT3  lpViewport			= NULL;
static D3DVIEWPORT2 vdData;
static D3DDEVICEDESC lpddDeviceDesc;
static DDSURFACEDESC2 ddsd;
static DDSCAPS2 ddscaps;
static HRESULT ddrval;
static D3DRECT ddrect;

// Global Variables

UCHAR *		video_buffer;

// Windows variables

static BOOL g_bActive = FALSE;

//-----------------------------------------------------------------------------
// External function-prototypes
//-----------------------------------------------------------------------------
HRESULT InitialiseEnvironment( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 ,HWND);
HRESULT MoveFrame( LPDIRECT3DDEVICE3, FLOAT );
HRESULT RenderView( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3, D3DRECT* );




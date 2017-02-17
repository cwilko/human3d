
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

#include <fstream>
#include <time.h>
#include <iostream>
using namespace std;


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






// DIRECTX FUNCTIONS
//-----------------------------------------------------------------------------
// Name: EnumZBufferCallback()
// Desc: Enumeration function to report valid pixel formats for z-buffers.
//-----------------------------------------------------------------------------
static HRESULT WINAPI EnumZBufferCallback( DDPIXELFORMAT* pddpf,
                                           VOID* pddpfDesired )
{
	// For this tutorial, we are only interested in z-buffers, so ignore any
	// other formats (e.g. DDPF_STENCILBUFFER) that get enumerated. An app
	// could also check the depth of the z-buffer (16-bit, etc,) and make a
	// choice based on that, as well. For this tutorial, we'll take the first
	// one we get.
    if( pddpf->dwFlags == DDPF_ZBUFFER )
    {
        memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );

        // Return with D3DENUMRET_CANCEL to end the search.
		return D3DENUMRET_CANCEL;
    }

    // Return with D3DENUMRET_OK to continue the search.
    return D3DENUMRET_OK;
}

int DD_Init(HWND hwnd)
{



	if(DirectDrawCreate(NULL,&pdd,NULL)!=DD_OK)
	{
		DD_Shutdown( hwnd );
		return 0;
	}

	if(pdd->QueryInterface(IID_IDirectDraw4, (LPVOID *) & lpdd ) != DD_OK)
	{
		DD_Shutdown( hwnd );
		return 0;
	}

	if(lpdd->SetCooperativeLevel(hwnd, DDSCL_ALLOWREBOOT|DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_FPUSETUP) != DD_OK )
	{
		DD_Shutdown( hwnd );
		return 0;
	}

    if (lpdd->QueryInterface( IID_IDirect3D3, (VOID**)&lpD3D ) != DD_OK)
    {
		DD_Shutdown( hwnd );
		return 0;
	}
    
	GUID pDeviceGUID = IID_IDirect3DRGBDevice;

	D3DFINDDEVICERESULT  devResult;
    D3DFINDDEVICESEARCH  devSearch;
    ZeroMemory( &devResult, sizeof(D3DFINDDEVICERESULT) );
    ZeroMemory( &devSearch, sizeof(D3DFINDDEVICESEARCH) );
    devResult.dwSize  = sizeof(D3DFINDDEVICERESULT);
    devSearch.dwSize  = sizeof(D3DFINDDEVICESEARCH);
    devSearch.dwFlags = D3DFDS_GUID;
    CopyMemory( &devSearch.guid, &pDeviceGUID, sizeof(GUID) );

    if( lpD3D->FindDevice( &devSearch, &devResult ) != DD_OK )
    {
        DD_Shutdown( hwnd );
		return 0;
    }

	cout << "Device Found" << endl << flush;

    if( devResult.ddHwDesc.dwFlags == 0L)
    {
        memcpy( &lpddDeviceDesc, &devResult.ddSwDesc, sizeof(D3DDEVICEDESC) );
	cout << "NO HW SUPPORT" << endl << flush;
	
    }
    else
    {
        memcpy( &lpddDeviceDesc, &devResult.ddHwDesc, sizeof(D3DDEVICEDESC) );
		
    }



	if(lpdd->SetDisplayMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,0,0) != DD_OK)
	{
		DD_Shutdown( hwnd );
		return 0;
	}

	// Set up a DirectX rectangle to contain screen parameters

	ddrect.x1 = 0;
	ddrect.y1 = 0;
	ddrect.x2 = SCREEN_WIDTH;
	ddrect.y2 = SCREEN_HEIGHT;
	

	ZeroMemory(&ddsd, sizeof(ddsd));
 	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_SYSTEMMEMORY ;
    ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.dwBackBufferCount = 1;

	if (lpdd->CreateSurface(&ddsd,&lpddsprimary,NULL) != DD_OK)
	{
		DD_Shutdown( hwnd );
		return 0;
	}
	cout << "Primary Surface Created" << endl << flush;

	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	if (lpddsprimary->GetAttachedSurface(&ddscaps,&lpddsback) != DD_OK)
	{
		DD_Shutdown( hwnd );
		return 0;
	}
	cout << "Retrieved Back Buffer Surface" << endl << flush;
	lpddsRenderTarget = lpddsback;
	lpddsRenderTarget->AddRef();


	DDPIXELFORMAT ddpfZBuffer;
	lpD3D->EnumZBufferFormats( pDeviceGUID, 
		                        EnumZBufferCallback, (VOID*)&ddpfZBuffer );

	
    if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )
    {
		DD_Shutdown( hwnd );
		return 0;
	}
	cout << "Found ZBuffer Format" << endl << flush;
	// Get z-buffer dimensions from the render target
    // Setup the surface desc for the z-buffer.
	ZeroMemory(&ddsd, sizeof(ddsd));
 	ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth        = SCREEN_WIDTH;
	ddsd.dwHeight       = SCREEN_HEIGHT;
    memcpy( &ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT) );


	if( lpdd->CreateSurface( &ddsd, &lpddsZBuffer, NULL ) != D3D_OK)
	{
		DD_Shutdown( hwnd );
		return 0;
	}

	if( lpddsback->AddAttachedSurface( lpddsZBuffer ) != D3D_OK )
	{
		DD_Shutdown( hwnd );
		return 0;
	}
	cout << "ZBuffer Created" << endl << flush;
	
	DDSURFACEDESC2 ddsd;
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	lpdd->GetDisplayMode( &ddsd );
	if( ddsd.ddpfPixelFormat.dwRGBBitCount <= 8 ) return 0;
		
	
    	if ( lpD3D->CreateDevice( pDeviceGUID, lpddsRenderTarget,
                               &lpd3dDevice, NULL ) != D3D_OK )
	{
		DD_Shutdown( hwnd );
		return 0;
	}
	cout << "Device Created" << endl << flush;



	ZeroMemory( &vdData, sizeof(D3DVIEWPORT2) );
    vdData.dwSize       = sizeof(D3DVIEWPORT2);
	vdData.dwWidth      = SCREEN_WIDTH;
	vdData.dwHeight     = SCREEN_HEIGHT;
    vdData.dvClipX      = -1.0f;
    vdData.dvClipWidth  = 2.0f;
    vdData.dvClipY      = 1.0f;
    vdData.dvClipHeight = 2.0f;
    vdData.dvMaxZ       = 1.0f;

	if ( lpD3D->CreateViewport( &lpViewport, NULL ) != D3D_OK)
	{
		DD_Shutdown( hwnd );
		return 0;
	}

    // Associate the viewport with the D3DDEVICE object
    lpd3dDevice->AddViewport( lpViewport );

    // Set the parameters to the new viewport
    lpViewport->SetViewport2( &vdData );

    // Set the viewport as current for the device
    lpd3dDevice->SetCurrentViewport( lpViewport );
	cout << "Viewport Created" << endl << flush;

	return 1;

} // end DD_Init

int DD_Shutdown ( HWND hwnd ) 
{
	
	if ( lpViewport )			lpViewport->Release();			lpViewport = NULL;
	if ( lpd3dDevice )			lpd3dDevice->Release();			lpd3dDevice = NULL;
	if ( lpddsback )			lpddsback->Release();			lpddsback = NULL;
	if ( lpddsRenderTarget )	lpddsRenderTarget->Release();	lpddsRenderTarget = NULL;
	if ( lpddsprimary )			lpddsprimary->Release();		lpddsprimary = NULL;
	if ( lpD3D )				lpD3D->Release();				lpD3D = NULL;
	
	if(lpdd) {
	
		lpdd->SetCooperativeLevel( hwnd, DDSCL_NORMAL );
		lpdd->Release();
		lpdd = NULL;
	
	}

	if (pdd)					pdd->Release();					pdd = NULL;
	
	
	return 1;

} // end DD_Shutdown







int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{

	HWND hwnd;
	MSG msg;
		
	

	// Initialise Application Window

	WNDCLASS wcl;

	wcl.hInstance		= hThisInst;
	wcl.lpszClassName	= WINDOW_CLASS_NAME;
	wcl.lpfnWndProc		= WindowFunc;
	wcl.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

	wcl.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor			= LoadCursor(NULL, IDC_ARROW);

	wcl.lpszMenuName	= WINDOW_CLASS_NAME;
	wcl.cbClsExtra		= 0;
	wcl.cbWndExtra		= 0;	

	wcl.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);
	

	if(!RegisterClass(&wcl)) return NULL;

	if(!(hwnd = CreateWindowEx (
		0,
		WINDOW_CLASS_NAME,
		"Game Prototype v1.1",
		WS_VISIBLE | WS_OVERLAPPED,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		hThisInst,
		NULL
		))) return 0;

	ShowWindow(hwnd, nWinMode);
	UpdateWindow(hwnd);
	SetFocus(hwnd);
	ShowCursor(0);
	



	if (! DD_Init(hwnd))
	{
		DestroyWindow(hwnd);
		MessageBox(hwnd,"Doesn`t Work","",0); return 0;
		return 0;
	} 

	cout << "Initialising Env" << endl << flush;
	if (InitialiseEnvironment(lpd3dDevice, lpViewport ,hwnd) != S_OK) return 0;
	while (1) 
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT) {  break; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} 
		else 
		{	
			
			MoveFrame(lpd3dDevice, ((FLOAT)clock())/CLOCKS_PER_SEC );

			RenderView(lpd3dDevice, lpViewport, &ddrect);
		
			ShowStats();

			while (TRUE)
               {
                   ddrval = lpddsprimary->Flip(NULL, 0);
                   if (ddrval == DD_OK) break;
                   
				   if (ddrval == DDERR_SURFACELOST)
                   {
                       ddrval = lpddsprimary->Restore();
                       if (ddrval != DD_OK)
                           break;
                   }
                   
				   if (ddrval != DDERR_WASSTILLDRAWING) break; 
			} 
			
            
			if (KEY_DOWN(VK_ESCAPE)) {
				
				//DD_Shutdown( hwnd );
			
				PostMessage(hwnd,WM_CLOSE,0,0);
			}
		} 

	}

	//DD_Shutdown( hwnd );

	return(msg.wParam);

}


// Message Function

LRESULT CALLBACK WindowFunc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch(message) {
        case WM_CLOSE:
            DestroyWindow( hwnd );
            return 0;
        
        case WM_DESTROY:
            //Cleanup3DEnvironment();
            PostQuitMessage(0);
            return 0L;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
	
}

//-----------------------------------------------------------------------------
// Name: AppShowStats()
// Desc: Shows frame rate and dimensions of the rendering device. Note: a 
//       "real" app wouldn't query the surface dimensions each frame.
//-----------------------------------------------------------------------------
VOID ShowStats()
{
    static FLOAT fFPS      = 0.0f;
    static FLOAT fLastTime = 0.0f;
    static DWORD dwFrames  = 0L;

    // Keep track of the time lapse and frame count
    FLOAT fTime = timeGetTime() * 0.001f; // Get current time in seconds
    ++dwFrames;

    // Update the frame rate once per second
    if( fTime - fLastTime > 1.0f )
    {
        fFPS      = dwFrames / (fTime - fLastTime);
        fLastTime = fTime;
        dwFrames  = 0L;
    }

    

    // Setup the text buffer to write out
    CHAR buffer[80];
    sprintf( buffer, "%7.02f fps (%dx%dx%d)", fFPS, SCREEN_WIDTH,
             SCREEN_HEIGHT, SCREEN_BPP );

	
	LPDIRECTDRAWSURFACE4 pddsRenderSurface;
    if( FAILED( lpd3dDevice->GetRenderTarget( &pddsRenderSurface ) ) )
        return;

    // Get a DC for the surface. Then, write out the buffer
    HDC hDC;
    if( SUCCEEDED( pddsRenderSurface->GetDC(&hDC) ) )
    {
        SetTextColor( hDC, RGB(255,255,0) );
        SetBkMode( hDC, TRANSPARENT );
        ExtTextOut( hDC, 0, 0, 0, NULL, buffer, strlen(buffer), NULL );
    
        pddsRenderSurface->ReleaseDC(hDC);
    }
    pddsRenderSurface->Release();
}



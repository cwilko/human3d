

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

    
	GUID pDeviceGUID = IID_IDirect3DHALDevice;

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

    if( devResult.ddHwDesc.dwFlags == 0L)
    {
        memcpy( &lpddDeviceDesc, &devResult.ddSwDesc, sizeof(D3DDEVICEDESC) );
		return 0;
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
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE | DDSCAPS_FLIP | DDSCAPS_COMPLEX ;
    ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.dwBackBufferCount = 1;

	if (lpdd->CreateSurface(&ddsd,&lpddsprimary,NULL) != DD_OK)
	{
		DD_Shutdown( hwnd );
		return 0;
	}


	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	if (lpddsprimary->GetAttachedSurface(&ddscaps,&lpddsback) != DD_OK)
	{
		DD_Shutdown( hwnd );
		return 0;
	}

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

	// Get z-buffer dimensions from the render target
    // Setup the surface desc for the z-buffer.
    ddsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
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







#include "init.cpp"
#include "directx.cpp"

// DEFINES //////////////////////////




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
		MessageBox(hwnd,"Doesn`t Work","poo",0); return 0;
		return 0;
	} 

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



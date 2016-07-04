// TextureList.cpp: implementation of the TextureList class.
//
//////////////////////////////////////////////////////////////////////

#include "TextureList.h"
#include <stdio.h>
#include <tchar.h>

struct TEXTURESEARCHINFO
	{
		DWORD dwBPP;          // Input for texture format search
		BOOL  bUseAlpha;
		BOOL  bOneBitAlpha;
		BOOL  bUsePalette;
		BOOL  bFoundGoodFormat;
	
		DDPIXELFORMAT* pddpf; // Result of texture format search
	};

#define D3DTEXTR_TRANSPARENTWHITE 0x00000001
#define D3DTEXTR_TRANSPARENTBLACK 0x00000002

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


TextureList::TextureList()
{

	currentTexture = head = NULL;
	TotalTextures = 0;
}

TextureList::~TextureList()
{

}

bool TextureList::Contains(char * name)
{

	if (strlen(name) >= 256) return false;

	for (int n = 0; n < TotalTextures; n++ ) {

		if (!strcmp(currentTexture->GetTextureName(), name)) return true;
		
		currentTexture = currentTexture->next;

	}

	return false;

}



bool TextureList::AddTexture(Texture *newTexture)
{

	if ( TotalTextures ) {
		newTexture->next = currentTexture->next;
		currentTexture->next = newTexture;
		currentTexture = newTexture;
	} else {
		newTexture->next = newTexture;
		head = currentTexture = newTexture;
		
	}

	TotalTextures++;

	return true;

}



Texture * TextureList::Contains(Texture *newTexture)
{
	if (TotalTextures == 0) return NULL;

	if (strlen(newTexture->GetTextureName()) >= 256) return NULL;

	for (int n = 0; n < TotalTextures; n++ ) {

		if (!strcmp(currentTexture->GetTextureName(), newTexture->GetTextureName())) 
			return currentTexture;
		
		currentTexture = currentTexture->next;

	}

	return NULL;
}


bool TextureList::LoadAllTextures(LPDIRECT3DDEVICE3 lpd3dDevice)
{
	if (! TotalTextures ) return false;

	char filename[256];

	for (int n = 0; n < TotalTextures; n++ ) {

		sprintf(filename,"%s.bmp",currentTexture->GetTextureName());

		if ( ! LoadTexture(lpd3dDevice, filename, currentTexture) )
			return false;
		
		currentTexture = currentTexture->next;

	}
	

	return true;

}







int TextureList::LoadTexture(LPDIRECT3DDEVICE3 lpd3dDevice, TCHAR *poo, Texture *pTexture)
{
	

	TCHAR* strExtension;

    if( NULL == ( strExtension = _tcsrchr( poo, TEXT('.') ) ) )
        return 0;


	// If the bitmap wasn't a resource, try it as a file.
	if( NULL == (
        pTexture->hbmBitmap = (HBITMAP)LoadImage( NULL, poo,
                                        IMAGE_BITMAP, 0, 0, 
                                        LR_LOADFROMFILE|LR_CREATEDIBSECTION ))) return 0;

	

	LPDIRECTDRAW4        pDD = NULL;
    LPDIRECTDRAWSURFACE4 pddsRender;

	// Get the current render target
	if( SUCCEEDED( lpd3dDevice->GetRenderTarget( &pddsRender ) ) )
	{
            // Get the DDraw4 interface from the render target
            pddsRender->GetDDInterface( (VOID**)&pDD );
            pddsRender->Release();
	}
    pDD->Release();

	// Get the device caps
    D3DDEVICEDESC ddHwDesc, ddSwDesc;
    DWORD         dwDeviceCaps;
    ddHwDesc.dwSize = sizeof(D3DDEVICEDESC);
    ddSwDesc.dwSize = sizeof(D3DDEVICEDESC);
    
	if( FAILED( lpd3dDevice->GetCaps( &ddHwDesc, &ddSwDesc ) ) )
        return 0;

    if( ddHwDesc.dwFlags ) dwDeviceCaps = ddHwDesc.dpcTriCaps.dwTextureCaps;
    else  return 0;

	// Get the bitmap structure (to extract width, height, and bpp)
    BITMAP bm;
    HBITMAP hbmBitmap = pTexture->hbmBitmap;
    GetObject( hbmBitmap, sizeof(BITMAP), &bm );
    DWORD dwWidth  = (DWORD)bm.bmWidth;
    DWORD dwHeight = (DWORD)bm.bmHeight;


	// Setup the new surface desc
    DDSURFACEDESC2 ddsd;
    
	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
    ddsd.dwSize                 = sizeof(DDSURFACEDESC2);
    ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|
                           DDSD_PIXELFORMAT|DDSD_TEXTURESTAGE;
    ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
    ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    ddsd.dwTextureStage  = 0L;
    ddsd.dwWidth         = dwWidth;
    ddsd.dwHeight        = dwHeight;


	// Adjust width and height, if the driver requires it
    if( dwDeviceCaps & D3DPTEXTURECAPS_POW2 )
    {
        for( ddsd.dwWidth=1;  dwWidth>ddsd.dwWidth;   ddsd.dwWidth<<=1 );
        for( ddsd.dwHeight=1; dwHeight>ddsd.dwHeight; ddsd.dwHeight<<=1 );
    }
    if( dwDeviceCaps & D3DPTEXTURECAPS_SQUAREONLY )
    {
        if( ddsd.dwWidth > ddsd.dwHeight ) ddsd.dwHeight = ddsd.dwWidth;
        else                               ddsd.dwWidth  = ddsd.dwHeight;
    }

    // Setup the structure to be used for texture enumration.
    TEXTURESEARCHINFO tsi;
    tsi.pddpf            = &ddsd.ddpfPixelFormat;
    tsi.bUseAlpha        = FALSE;
    tsi.bOneBitAlpha     = FALSE;
    tsi.bUsePalette      = TRUE;
    tsi.bFoundGoodFormat = FALSE;
    tsi.dwBPP = 16;

    // Enumerate the texture formats, and find the closest device-supported
    // texture pixel format
    lpd3dDevice->EnumTextureFormats( TextureSearchCallback, &tsi );

	if( FALSE == tsi.bFoundGoodFormat )
        return 0;



    HRESULT hr;
    if( FAILED( hr = pDD->CreateSurface( &ddsd, &pTexture->pddsSurface, NULL ) ) )
        return 0;

    // Create the texture
    if( FAILED( pTexture->pddsSurface->QueryInterface( IID_IDirect3DTexture2,
                                         (VOID**)&pTexture->ptexTexture ) ) )
        return 0;

	if( FAILED( CopyBitmapToSurface( pTexture->pddsSurface, hbmBitmap,
							                            0L ) ) )
		return 0;




	return 1;

}



//-----------------------------------------------------------------------------
// Name: CopyBitmapToSurface()
// Desc: Copies the image of a bitmap into a surface
//-----------------------------------------------------------------------------


HRESULT TextureList::CopyBitmapToSurface(LPDIRECTDRAWSURFACE4 pddsTarget, HBITMAP hbmBitmap, DWORD dwFlags)
{
  // Get a DDraw object to create a temporary surface
    LPDIRECTDRAW4 pDD;
    pddsTarget->GetDDInterface( (VOID**)&pDD );
    pDD->Release();

    // Get the bitmap structure (to extract width, height, and bpp)
    BITMAP bm;
    GetObject( hbmBitmap, sizeof(BITMAP), &bm );

    // Setup the new surface desc
    DDSURFACEDESC2 ddsd;
    
	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
    ddsd.dwSize                 = sizeof(DDSURFACEDESC2);
    ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    pddsTarget->GetSurfaceDesc( &ddsd );
    ddsd.dwFlags          = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT|
                            DDSD_TEXTURESTAGE;
    ddsd.ddsCaps.dwCaps   = DDSCAPS_TEXTURE|DDSCAPS_SYSTEMMEMORY;
    ddsd.ddsCaps.dwCaps2  = 0L;
    ddsd.dwWidth          = bm.bmWidth;
    ddsd.dwHeight         = bm.bmHeight;

    // Create a new surface for the texture
    LPDIRECTDRAWSURFACE4 pddsTempSurface;
    HRESULT hr;
    if( FAILED( hr = pDD->CreateSurface( &ddsd, &pddsTempSurface, NULL ) ) )
        return NULL;

    // Get a DC for the bitmap
    HDC hdcBitmap = CreateCompatibleDC( NULL );
    if( NULL == hdcBitmap )
    {
        pddsTempSurface->Release();
        return NULL;
    }
    SelectObject( hdcBitmap, hbmBitmap );

    // Handle palettized textures. Need to attach a palette
    if( ddsd.ddpfPixelFormat.dwRGBBitCount == 8 )
    {
        // Create the color table and parse in the palette
        DWORD pe[256];
        WORD  wNumColors = GetDIBColorTable( hdcBitmap, 0, 256,
                                             (RGBQUAD*)pe );
        for( WORD i=0; i<wNumColors; i++ )
        {
            pe[i] = 0xff000000 + RGB( GetBValue(pe[i]), GetGValue(pe[i]),
                                      GetRValue(pe[i]) );

            // Set alpha for transparent pixels
            if( dwFlags & D3DTEXTR_TRANSPARENTBLACK )
            {
                if( (pe[i]&0x00ffffff) == 0x00000000 )
                    pe[i] &= 0x00ffffff;
            }
            else if( dwFlags & D3DTEXTR_TRANSPARENTWHITE )
            {
                if( (pe[i]&0x00ffffff) == 0x00ffffff )
                    pe[i] &= 0x00ffffff;
            }
        }
        // Create & attach a palette with the bitmap's colors
        LPDIRECTDRAWPALETTE  pPalette;
        if( dwFlags & (D3DTEXTR_TRANSPARENTWHITE|D3DTEXTR_TRANSPARENTBLACK) )
            pDD->CreatePalette( DDPCAPS_8BIT|DDPCAPS_ALPHA, (PALETTEENTRY*)pe, &pPalette, NULL );
        else
            pDD->CreatePalette( DDPCAPS_8BIT, (PALETTEENTRY*)pe, &pPalette, NULL );
        pddsTempSurface->SetPalette( pPalette );
        pddsTarget->SetPalette( pPalette );
        if( pPalette ) {
			pPalette->Release();
			pPalette = NULL;
		}
    }

    // Copy the bitmap image to the surface.
    HDC hdcSurface;
    if( SUCCEEDED( pddsTempSurface->GetDC( &hdcSurface ) ) )
    {
        BitBlt( hdcSurface, 0, 0, bm.bmWidth, bm.bmHeight, hdcBitmap, 0, 0,
                SRCCOPY );
        pddsTempSurface->ReleaseDC( hdcSurface );
    }
    DeleteDC( hdcBitmap );

    // Copy the temp surface to the real texture surface
    pddsTarget->Blt( NULL, pddsTempSurface, NULL, DDBLT_WAIT, NULL );

    if( ddsd.ddpfPixelFormat.dwRGBBitCount == 8 )
    {
        LPDIRECTDRAWPALETTE pPalette;
        DWORD pe[256];
        pddsTempSurface->GetPalette( &pPalette );
        pPalette->GetEntries( 0, 0, 256, (PALETTEENTRY*)&pe );
        pPalette->Release();

        pddsTarget->GetPalette( &pPalette );
        pPalette->SetEntries( 0, 0, 256, (PALETTEENTRY*)&pe );
        pPalette->Release();
    }
    pddsTempSurface->Release();

    // For textures with real alpha (not palettized), set transparent bits
    if( ddsd.ddpfPixelFormat.dwRGBAlphaBitMask )
    {
        if( dwFlags & (D3DTEXTR_TRANSPARENTWHITE|D3DTEXTR_TRANSPARENTBLACK) )
        {
            // Lock the texture surface
            DDSURFACEDESC2 ddsd;
            
			ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
			ddsd.dwSize                 = sizeof(DDSURFACEDESC2);
			ddsd.dwFlags                = 0;
			ddsd.ddsCaps.dwCaps         = 0;
			ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

            while( pddsTarget->Lock( NULL, &ddsd, 0, NULL ) ==
                   DDERR_WASSTILLDRAWING );
             
            DWORD dwAlphaMask = ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
            DWORD dwRGBMask   = ( ddsd.ddpfPixelFormat.dwRBitMask |
                                  ddsd.ddpfPixelFormat.dwGBitMask |
                                  ddsd.ddpfPixelFormat.dwBBitMask );
            DWORD dwColorkey  = 0x00000000; // Colorkey on black
            if( dwFlags & D3DTEXTR_TRANSPARENTWHITE ) 
                dwColorkey = dwRGBMask;     // Colorkey on white

            // Add an opaque alpha value to each non-colorkeyed pixel
            for( DWORD y=0; y<ddsd.dwHeight; y++ )
            {
                WORD*  p16 =  (WORD*)((BYTE*)ddsd.lpSurface + y*ddsd.lPitch);
                DWORD* p32 = (DWORD*)((BYTE*)ddsd.lpSurface + y*ddsd.lPitch);
        
                for( DWORD x=0; x<ddsd.dwWidth; x++ )
                {
                    if( ddsd.ddpfPixelFormat.dwRGBBitCount == 16 )
                    {
                        if( ( *p16 &= dwRGBMask ) != dwColorkey )
                            *p16 |= dwAlphaMask;
                        p16++;
                    }
                    if( ddsd.ddpfPixelFormat.dwRGBBitCount == 32 )
                    {
                        if( ( *p32 &= dwRGBMask ) != dwColorkey )
                            *p32 |= dwAlphaMask;
                        p32++;
                    }
                }
            }
            pddsTarget->Unlock( NULL );
        }
    }

    return S_OK;;
}


//-----------------------------------------------------------------------------
// Name: TextureSearchCallback()
// Desc: Enumeration callback routine to find a best-matching texture format. 
//       The param data is the DDPIXELFORMAT of the best-so-far matching
//       texture. Note: the desired BPP is passed in the dwSize field, and the
//       default BPP is passed in the dwFlags field.
//-----------------------------------------------------------------------------


HRESULT CALLBACK TextureList::TextureSearchCallback(DDPIXELFORMAT *pddpf, VOID *param)
{
   if( NULL==pddpf || NULL==param )
        return DDENUMRET_OK;

    // Get the requested attributes for the pixel format
    TEXTURESEARCHINFO* ptsi = (TEXTURESEARCHINFO*)param;

    // Count the alpha bits for the pixel format
    DWORD dwAlphaBitCount = 0;
    if( pddpf->dwFlags & DDPF_ALPHAPIXELS )
        for( DWORD dwAMask = pddpf->dwRGBAlphaBitMask; dwAMask; dwAMask>>=1 )
            dwAlphaBitCount += ( dwAMask & 0x1 );

    // Skip any bumpmapping modes
    if( pddpf->dwFlags & (DDPF_BUMPLUMINANCE|DDPF_BUMPDUDV) )
        return DDENUMRET_OK;
        
    // Skip any luminance-only or alpha-only modes
    if( pddpf->dwFlags & (DDPF_LUMINANCE|DDPF_ALPHA) )
        return DDENUMRET_OK;

    // Skip any YUV or FourCC modes
    if( pddpf->dwFlags & (DDPF_YUV|DDPF_FOURCC) )
        return DDENUMRET_OK;

    // Check for 8-bit palettized formats
    if( pddpf->dwFlags & DDPF_PALETTEINDEXED8 )
    {
        // Skip this format if a palettized format was not requested
        if( ptsi->bUsePalette )
            return DDENUMRET_OK;

        // Skip palettized formats that have an added alpha channel
        if( 0 != dwAlphaBitCount )
            return DDENUMRET_OK;
        
        // Accept this format. Copy the pixelformat and mark it as good.
        memcpy( ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT) );
        ptsi->bFoundGoodFormat = TRUE;
        return DDENUMRET_CANCEL;
    }

    // Else, skip any non-RGB formats
    if( 0 == ( pddpf->dwFlags & DDPF_RGB ) )
        return DDENUMRET_OK;

    // Make sure current alpha format agrees with requested format type
    if( (TRUE==ptsi->bUseAlpha) && (0==dwAlphaBitCount) )
        return DDENUMRET_OK;
    if( (FALSE==ptsi->bUseAlpha) && (0!=dwAlphaBitCount) )
        return DDENUMRET_OK;

    // Make sure the bits-per-pixel matches the request
    if( pddpf->dwRGBBitCount == ptsi->dwBPP )
    {
        // Accept this format. Copy the pixelformat and mark it as good.
        memcpy( ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT) );
        ptsi->bFoundGoodFormat = TRUE;

        // Distinguish formats and requests for full- versus 1-bit of alpha.
        // If the match is not perfect, keep enumerating more formats.
        if( ptsi->bOneBitAlpha && ( 1 != dwAlphaBitCount ) )
            return DDENUMRET_OK;
        if( !(ptsi->bOneBitAlpha) && ( 1 == dwAlphaBitCount ) )
            return DDENUMRET_OK;

        // Return with the current texture format
        return DDENUMRET_CANCEL;
    }

    // Else, keep enumerating
    return DDENUMRET_OK;
}




//-----------------------------------------------------------------------------
// File: game.cpp
//
// Desc: Simple tutorial code to show how to enable z-buffering. The z-buffer
//       itself is created in the winmain.cpp file. This file controls
//       rendering and the setting of renderstates (some of which affect
//       z-buffering).
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


#define D3D_OVERLOADS
#include <math.h>
#include <d3d.h>
#include <tchar.h>
#include "Object.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------



#define MAX_VERTICES 5000
#define VIEW_DISTANCE 50

static LPDIRECT3DMATERIAL3 g_pmtrlObjectMtrl = NULL;
static LPDIRECT3DMATERIAL3 g_pmtrlObjectMtrl2 = NULL;
static LPDIRECT3DLIGHT     g_pLight              = NULL;
static D3DVERTEX		tester[6];

static Object	* Bird			= new Object(12);
static TextureList	* Textures	= new TextureList();



//-----------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------

int LoadFile( char *, Frame *, int);
int LoadTextures(LPDIRECT3DDEVICE3 lpd3dDevice, TCHAR * , Texture *);


//-----------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// InitialiseEnvironment
//	
// Sets up all Geometry
//-----------------------------------------------------------------------------


HRESULT InitialiseEnvironment( LPDIRECT3DDEVICE3 lpd3dDevice,
                               LPDIRECT3DVIEWPORT3 pvViewport, HWND hwnd )
{

	if ( !  LoadFile("bird1.asc", Bird->currentFrame, 100 ) )
		return E_FAIL;
	
	if (!Bird->AdvanceFrame()) return E_FAIL;

	if ( !  LoadFile("bird15.asc", Bird->currentFrame, 100)  )
		return E_FAIL;
	
		
Bird->AdvanceFrame();
	if ( !  LoadFile("bird2.asc", Bird->currentFrame, 100)  )
		return E_FAIL;	
if (!Bird->AdvanceFrame()) return E_FAIL;
	if ( !  LoadFile("bird25.asc", Bird->currentFrame, 100)  )
		return E_FAIL;
Bird->AdvanceFrame();	
	if ( !  LoadFile("bird3.asc", Bird->currentFrame, 100)  )
		return E_FAIL;
Bird->AdvanceFrame();
	if ( !  LoadFile("bird35.asc", Bird->currentFrame, 100)  )
		return E_FAIL;
Bird->AdvanceFrame();	
	if ( ! LoadFile("bird4.asc", Bird->currentFrame, 100)  )
		return E_FAIL;
Bird->AdvanceFrame();	
	if ( ! LoadFile("bird45.asc", Bird->currentFrame, 100)  )
		return E_FAIL;
Bird->AdvanceFrame();
	if ( ! LoadFile("bird5.asc", Bird->currentFrame, 100)  )
		return E_FAIL;
Bird->AdvanceFrame();	
	if ( ! LoadFile("bird55.asc", Bird->currentFrame, 100)  )
		return E_FAIL;
Bird->AdvanceFrame();
	if ( ! LoadFile("bird6.asc", Bird->currentFrame, 100)  )
		return E_FAIL;
Bird->AdvanceFrame();	
	if ( ! LoadFile("bird65.asc", Bird->currentFrame, 100)  )
		return E_FAIL;
Bird->AdvanceFrame();

	if (! Bird->LoadTextures(Textures, lpd3dDevice, hwnd) ) return E_FAIL;



	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
    
	LPDIRECT3D3 pD3D;
    if( FAILED( lpd3dDevice->GetDirect3D( &pD3D ) ) ) return E_FAIL;
    pD3D->Release();

    // Create the object material. This material will be used to draw the
	// triangle. Note: that when we use textures, the object material is
	// usually omitted or left as white.

    if( FAILED( pD3D->CreateMaterial( &g_pmtrlObjectMtrl, NULL ) ) )
        return E_FAIL;

	if( FAILED( pD3D->CreateMaterial( &g_pmtrlObjectMtrl2, NULL ) ) )
        return E_FAIL;

	// Set the object material as yellow. We're setting the ambient color here
	// since this tutorial only uses ambient lighting. For apps that use real
	// lights, the diffuse and specular values should be set. (In addition, the
	// polygons' vertices need normals for true lighting.)
    D3DMATERIAL       mtrl;
	D3DMATERIAL		mtrl2;
    D3DMATERIALHANDLE hmtrl;

	ZeroMemory( &mtrl, sizeof(D3DMATERIAL) );
    mtrl.dwSize       = sizeof(D3DMATERIAL);
    mtrl.dcvDiffuse.r = mtrl.dcvAmbient.r = 1.0f;
    mtrl.dcvDiffuse.g = mtrl.dcvAmbient.g = 1.0f;
    mtrl.dcvDiffuse.b = mtrl.dcvAmbient.b = 1.0f;
    mtrl.dcvDiffuse.a = mtrl.dcvAmbient.a = 1.0f;
    mtrl.dwRampSize   = 16L; // A default ramp size
	mtrl.power = 40.0f;
    g_pmtrlObjectMtrl->SetMaterial( &mtrl );

	ZeroMemory( &mtrl2, sizeof(D3DMATERIAL) );
    mtrl2.dwSize       = sizeof(D3DMATERIAL);
	mtrl2.dcvAmbient.r = 0.7f;
    mtrl2.dcvAmbient.g = 0.6f;
    mtrl2.dcvAmbient.b = 0.2f;
    g_pmtrlObjectMtrl2->SetMaterial( &mtrl2 );
	

    lpd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, TRUE );

	// Put the object material into effect. Direct3D is a state machine, and
	// calls like this set the current state. After this call, any polygons
	// rendered will be drawn using this material.
    g_pmtrlObjectMtrl->GetHandle( lpd3dDevice, &hmtrl );
    lpd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, hmtrl );

	// The ambient lighting value is another state to set. Here, we are turning
	// ambient lighting on to full white.
    lpd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0xffffffff );

//	lpd3dDevice->SetRenderState( D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME );
//	lpd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_CCW );

    // Set the transform matrices. Direct3D uses three independant matrices:
	// the world matrix, the view matrix, and the projection matrix. For
	// convienence, we are first setting up an identity matrix.
    D3DMATRIX mat;
	mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
	mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
	mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
	mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;
	
	// The world matrix controls the position and orientation of the polygons
	// in world space. We'll use it later to spin the triangle.
	D3DMATRIX matWorld = mat;
	matWorld._22 = 0.75f;
	matWorld._11 = 0.75f;
    lpd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );

	// The view matrix defines the position and orientation of the camera.
	// Here, we are just moving it back along the z-axis by 10 units.
	D3DMATRIX matView = mat;
	matView._43 = VIEW_DISTANCE;
//	matView._42 = -100.0f;
    lpd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );

	// The projection matrix defines how the 3D scene is "projected" onto the
	// 2D render target (the backbuffer surface). Refer to the docs for more
	// info about projection matrices.
	D3DMATRIX matProj = mat;
	matProj._11 =  1.0f;
	matProj._22 =  1.0f;
	matProj._34 =  1.0f;
	matProj._43 =  -1.0f;
	matProj._44 =  1.0f;
    lpd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

	if( FAILED( pD3D->CreateLight( &g_pLight, NULL ) ) )
        return E_FAIL;
    
    D3DLIGHT light;

	ZeroMemory( &light, sizeof(D3DLIGHT) );
    light.dwSize       = sizeof(D3DLIGHT);
    light.dltType      = D3DLIGHT_POINT;
    light.dvPosition.x = light.dvDirection.x = 0.0;
    light.dvPosition.y = light.dvDirection.y = 0.0;
    light.dvPosition.z = light.dvDirection.z = -10.0;

    light.dcvColor.r = 1.0;
    light.dcvColor.g = 1.0;
    light.dcvColor.b = 1.0;
    light.dvAttenuation0 = 1.0f;
    g_pLight->SetLight( &light );
    pvViewport->AddLight( g_pLight );

	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SetViewMatrix()
// Desc: Given an eye point, a lookat point, and an up vector, this
//       function builds a 4x4 view matrix.
//-----------------------------------------------------------------------------



HRESULT SetViewMatrix( D3DMATRIX& mat, D3DVECTOR& vFrom, D3DVECTOR& vAt,
					   D3DVECTOR& vWorldUp )
{
    // Get the z basis vector, which points straight ahead. This is the
    // difference from the eyepoint to the lookat point.
    D3DVECTOR vView = vAt - vFrom;

    FLOAT fLength = Magnitude( vView );
    if( fLength < 1e-6f )
        return E_INVALIDARG;

    // Normalize the z basis vector
    vView /= fLength;

    // Get the dot product, and calculate the projection of the z basis
    // vector onto the up vector. The projection is the y basis vector.
    FLOAT fDotProduct = DotProduct( vWorldUp, vView );

    D3DVECTOR vUp = vWorldUp - fDotProduct * vView;

    // If this vector has near-zero length because the input specified a
    // bogus up vector, let's try a default up vector
    if( 1e-6f > ( fLength = Magnitude( vUp ) ) )
    {
        vUp = D3DVECTOR( 0.0f, 1.0f, 0.0f ) - vView.y * vView;

        // If we still have near-zero length, resort to a different axis.
        if( 1e-6f > ( fLength = Magnitude( vUp ) ) )
        {
            vUp = D3DVECTOR( 0.0f, 0.0f, 1.0f ) - vView.z * vView;

            if( 1e-6f > ( fLength = Magnitude( vUp ) ) )
                return E_INVALIDARG;
        }
    }

    // Normalize the y basis vector
    vUp /= fLength;

    // The x basis vector is found simply with the cross product of the y
    // and z basis vectors
    D3DVECTOR vRight = CrossProduct( vUp, vView );
    
    // Start building the matrix. The first three rows contains the basis
    // vectors used to rotate the view to point at the lookat point
    mat._11 = vRight.x;  mat._12 = vUp.x;  mat._13 = vView.x;  mat._14 = 0.0f;
    mat._21 = vRight.y;  mat._22 = vUp.y;  mat._23 = vView.y;  mat._24 = 0.0f;
    mat._31 = vRight.z;  mat._32 = vUp.z;  mat._33 = vView.z;  mat._34 = 0.0f;

    // Do the translation values (rotations are still about the eyepoint)
    mat._41 = - DotProduct( vFrom, vRight );
    mat._42 = - DotProduct( vFrom, vUp );
    mat._43 = - DotProduct( vFrom, vView );
    mat._44 = 1.0f;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: MoveFrame()
// Desc: Called once per frame, the call is used for animating the scene. The
//       device is used for changing various render states, and the timekey is
//       used for timing of the dynamics of the scene.
//-----------------------------------------------------------------------------




HRESULT MoveFrame( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    // For this simple tutorial, we are rotating the triangle about the y-axis.
	// To do this, just set up a 4x4 matrix defining the rotation, and set it
	// as the new world transform.
 	D3DVECTOR vEyePt    = D3DVECTOR( VIEW_DISTANCE * (float) cos(fTimeKey), VIEW_DISTANCE * (float) sin(fTimeKey), 35);
	D3DVECTOR vLookatPt = D3DVECTOR( 0,0,35); // 4*sin( fTimeKey+.1), 2.5, 4*cos( fTimeKey+.1 ) );
	D3DVECTOR vUpVec    = D3DVECTOR( 0, 0, 1 );

	// Use the above parameters to build a new view matrix and put it into
	// effect.
	D3DMATRIX matView;
	SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );


	return S_OK;
}

int FrameCount = 0, time = 0;

//-----------------------------------------------------------------------------
// Name: RenderView()
// Desc: Renders the scene. This tutorial draws a bunch of intersecting
//       triangles that are rotating about the y-axis. Without z-buffering,
//       the polygons could not be drawn correctly (unless the app performed
//       complex polygon-division routines and sorted the polygons in back-to-
//       front order.)
//-----------------------------------------------------------------------------
HRESULT RenderView( LPDIRECT3DDEVICE3 pd3dDevice, 
                    LPDIRECT3DVIEWPORT3 pvViewport, D3DRECT* prcViewportRect )
{
    // Clear the viewport to a blue color. Also "clear" the z-buffer to the
	// value 1.0 (which represents the far clipping plane).
    pvViewport->Clear2( 1UL, prcViewportRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		                0x00000022, 1.0f, 0L );

    // Begin the scene
    if( FAILED( pd3dDevice->BeginScene() ) )
		return E_FAIL;


	// Draw all the objects
	
	Element ** TempElements;
	int TotalElements;



	// FOR ALL OBJECTS

	TotalElements = Bird->currentFrame->TotalElements;

	TempElements = Bird->currentFrame->ElementList;



		for (int n = 0; n < TotalElements; n++ ) {


			pd3dDevice->SetTexture( 0, TempElements[n]->ElementTexture->ptexTexture );


			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                           TempElements[n]->FaceList, 
						   TempElements[n]->GetFaceCount() * 3, NULL );
		
	
		


		
		}
		if ( time++ >= 5) {
			
			FrameCount++;
			time = 0;
			Bird->AdvanceFrame();
		}

		if ( FrameCount >= Bird->TotalFrames ) FrameCount = 0;




    
//		g_pmtrlObjectMtrl2->GetHandle( pd3dDevice, &hmtrl );
//		pd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, hmtrl );
	


    // End the scene.
    pd3dDevice->EndScene();

    return S_OK;
}



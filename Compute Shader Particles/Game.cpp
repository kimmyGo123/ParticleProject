#include "Game.h"
#include "Vertex.h"

// Include the texture loader from the DXTK
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include "Mesh.h"
#include <iostream>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game - Press Tab to swap models",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	camera = 0;
	prevMousePos = { 0,0 };

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{

	
	// Particle stuff
	delete particleEmitter;
	delete particleEmitCS;
	delete particleUpdateCS;
	delete particleDeadListInitCS;
	delete particleCopyDrawCountCS;
	delete particleVS;
	delete particleVSnoGS;
	delete particleGS;
	delete particlePS;

	// Remove sprite stuff
	delete spriteBatch;
	delete font;

	// Other objects
	delete camera;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMatrices();
	CreateEntities();

	// Sprite stuff for UI
	spriteBatch = new SpriteBatch(context);
	font = new SpriteFont(device, L"Fonts/Arial.spritefont");
	particleObj = new Mesh("C:\\Users\\rando\\Downloads\\Compute Shader Particles\\Compute Shader Particles\\cone.obj", device);
	std::cout << particleObj->GetIndexCount() << "AAAAAA\n";
	// Set up particles
	particleEmitter = new Emitter(
		1000000,
		1000000.0f, // Particles per second
		100.0f, // Particle lifetime
		device,
		context,
		particleDeadListInitCS,
		particleEmitCS,
		particleUpdateCS,
		particleCopyDrawCountCS,
		particleVS,
		particleGS,
		particlePS,
		particleVSnoGS,
		particleObj);



	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	particleEmitCS = new SimpleComputeShader(device, context);
	particleEmitCS->LoadShaderFile(L"ParticleEmitCS.cso");

	particleUpdateCS = new SimpleComputeShader(device, context);
	particleUpdateCS->LoadShaderFile(L"ParticleUpdateCS.cso");

	particleDeadListInitCS = new SimpleComputeShader(device, context);
	particleDeadListInitCS->LoadShaderFile(L"ParticleDeadListInitCS.cso");

	particleCopyDrawCountCS = new SimpleComputeShader(device, context);
	particleCopyDrawCountCS->LoadShaderFile(L"ParticleCopyDrawCountCS.cso");

	particleVS = new SimpleVertexShader(device, context);
	particleVS->LoadShaderFile(L"ParticleVS.cso");

	particleVSnoGS = new SimpleVertexShader(device, context);
	particleVSnoGS->LoadShaderFile(L"ParticleVSnoGS.cso");

	particleGS = new SimpleGeometryShader(device, context);
	particleGS->LoadShaderFile(L"ParticleGS.cso");

	particlePS = new SimplePixelShader(device, context);
	particlePS->LoadShaderFile(L"ParticlePS.cso");
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	camera = new Camera(0, 0, -5);
	camera->UpdateProjectionMatrix((float)width / height);
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateEntities()
{
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update the projection matrix assuming the camera exists
	if (camera)
	{
		camera->UpdateProjectionMatrix((float)width / height);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	// Handle particle update
	static bool updateParticles = false;
	bool currentP = (GetAsyncKeyState('P') & 0x8000) != 0;
	if (currentP && !prevP) { updateParticles = !updateParticles; }
	prevP = currentP;

	if (GetAsyncKeyState('G') & 0x8000) particleEmitter->UseGeometryShader(true);
	if (GetAsyncKeyState('V') & 0x8000) particleEmitter->UseGeometryShader(false);

	if (updateParticles)
	{
		particleEmitter->Update(deltaTime, totalTime);
	}
	else
	{
		// Single step
		float speed = 0.1f;
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) speed *= 10.0f;
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000) speed *= 0.1f;
		if (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) particleEmitter->Update(deltaTime * speed, totalTime);
		if (GetAsyncKeyState(VK_OEM_MINUS) & 0x8000) particleEmitter->Update(-deltaTime * speed, totalTime);
	}

	// Update the camera
	camera->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Black in this case) for clearing
	const float color[4] = { 0,0,0,0 };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	//draw mesh
	// Draw particles
	particleEmitter->Draw(camera, true);

	// UI
	/*spriteBatch->Begin();

	font->DrawString(
		spriteBatch,
		particleEmitter->GetUseGeometryShader() ? L"Drawing with geometry shader: YES" : L"Drawing with geometry shader: NO",
		XMFLOAT2(10, 10));

	font->DrawString(
		spriteBatch,
		L"Controls:\nWASD & Mouse Look\np: Pause/Unpause\n+: Step forward (while paused)\n-: Step backwards (while pauses)\ng: Use geometry shader\nv: Use vertex/pixel shader only\n\nPress 'p' to begin!",
		XMFLOAT2(10, 45));

	spriteBatch->End();

	*/
	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain effect,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Check left mouse button
	if (buttonState & 0x0001)
	{
		float xDiff = (x - prevMousePos.x) * 0.005f;
		float yDiff = (y - prevMousePos.y) * 0.005f;
		camera->Rotate(yDiff, xDiff);
	}

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion
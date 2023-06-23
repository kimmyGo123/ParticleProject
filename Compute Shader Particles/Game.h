#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>

#include "SpriteBatch.h"
#include "SpriteFont.h"

#include "Camera.h"
#include "Emitter.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();
	Mesh* particleObj;
	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	Camera* camera;

	// Particle stuff
	SimpleVertexShader* particleVS;
	SimpleVertexShader* particleVSnoGS;
	SimpleGeometryShader* particleGS;
	SimplePixelShader* particlePS;
	SimpleComputeShader* particleEmitCS;
	SimpleComputeShader* particleUpdateCS;
	SimpleComputeShader* particleDeadListInitCS;
	SimpleComputeShader* particleCopyDrawCountCS;
	Emitter* particleEmitter;

	// UI & input
	bool prevP;
	DirectX::SpriteBatch* spriteBatch;
	DirectX::SpriteFont* font;


	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateEntities();


	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};


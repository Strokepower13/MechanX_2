#pragma once
#include <DX3D/Prerequisites.h>

class GraphicsEngine
{
public:
	//Initialize the GraphicsEngine and DirectX 11 Device
	GraphicsEngine(Game* game);
	//Release all the resources loaded
	~GraphicsEngine();

	void update();

	RenderSystem* getRenderSystem();

private:
	std::unique_ptr<RenderSystem> p_renderSystem = nullptr;
	Game* p_game = nullptr;
};


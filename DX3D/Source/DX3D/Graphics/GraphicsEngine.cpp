#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/SwapChain.h>

#include <DX3D/Game/Game.h>
#include <DX3D/Game/Display.h>

GraphicsEngine::GraphicsEngine(Game* game) : p_game(game)
{
    p_renderSystem = std::make_unique<RenderSystem>();
}

GraphicsEngine::~GraphicsEngine()
{

}

void GraphicsEngine::update()
{
    auto& swapChain = p_game->p_display->p_swapChain;
    //p_renderSystem->clearState();

    //context->clearRenderTargetColor(swapChain, 0, 1, 1, 1);
    //auto winSize = p_game->p_display->getClientSize();
    int width = p_game->p_display->getClientWidth();
    int height = p_game->p_display->getClientHeight();

    //context->setViewportSize(width, height);

    swapChain->present(true);
}

RenderSystem* GraphicsEngine::getRenderSystem()
{
    return p_renderSystem.get();
}

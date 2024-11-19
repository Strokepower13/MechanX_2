#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>
#include <vector>

class SwapChain
{
public:
	//initialize SwapChain for a window
	SwapChain(HWND hwnd, UINT width, UINT height, RenderSystem* system, bool enable4xMsaa);

	void setFullscreen(bool fullscreen, unsigned int width, unsigned int height);
	void resize(unsigned int width, unsigned int height);
	void present(bool vsync);
private:
	void reloadBuffers(unsigned int width, unsigned int height);

	ID3D12Resource* currentBackBuffer() const;

	Microsoft::WRL::ComPtr<IDXGISwapChain> p_swapChain=nullptr;

	int p_swapChainBufferCount = 0;
	int p_currBackBuffer = 0;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> p_swapChainBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> p_depthStencilBuffer;
	
	D3D12_VIEWPORT p_screenViewport;
	D3D12_RECT p_scissorRect;

	RenderSystem* p_system = nullptr;

	bool p_enable4xMsaa;
	UINT p_4xMsaaQuality;

	friend class CommandManager;
	friend class DescriptorHeap;
};


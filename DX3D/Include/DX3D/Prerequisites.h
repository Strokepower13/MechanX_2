#pragma once
#include <memory>
#include <sstream>
#include <stdexcept>

class SwapChain;
class CommandManager;
class DescriptorHeap;
class RootSignature;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class InputLayout;
class PipelineState;
class VertexShader;
class GeometryShader;
class PixelShader;
class Texture;
class Texture2DArray;
class Effect;
class RenderSystem;
class GraphicsEngine;
class MSAAResources;

class GameTimer;
class Window;
class Game;
class Display;

class InputSystem;

class RasterizerState;
class BlendState;
class DepthStencilState;

typedef std::shared_ptr <SwapChain> SwapChainPtr;
typedef std::shared_ptr <CommandManager> CommandManagerPtr;
typedef std::shared_ptr <DescriptorHeap> DescriptorHeapPtr;
typedef std::shared_ptr <VertexBuffer> VertexBufferPtr;
typedef std::shared_ptr <IndexBuffer> IndexBufferPtr;
typedef std::shared_ptr <ConstantBuffer> ConstantBufferPtr;
typedef std::shared_ptr <RootSignature> RootSignaturePtr;
typedef std::shared_ptr <InputLayout> InputLayoutPtr;
typedef std::shared_ptr <PipelineState> PipelineStatePtr;
typedef std::shared_ptr <VertexShader> VertexShaderPtr;
typedef std::shared_ptr <GeometryShader> GeometryShaderPtr;
typedef std::shared_ptr <PixelShader> PixelShaderPtr;
typedef std::shared_ptr <Texture> TexturePtr;
typedef std::shared_ptr <Texture2DArray> Texture2DArrayPtr;

typedef std::shared_ptr <RasterizerState> RasterizerStatePtr;
typedef std::shared_ptr <BlendState> BlendStatePtr;
typedef std::shared_ptr <DepthStencilState> DepthStencilStatePtr;

typedef std::shared_ptr <MSAAResources> MSAAResourcesPtr;

enum class InputLayoutType
{
	PosColor = 0
};

enum class CullMode
{
	None = 0,
	Front,
	Back
};

enum class FillMode
{
	Wireframe = 0,
	Solid
};

enum class BlendMode
{
	Transparent = 0,
	AlphaToCoverage,
	NoRenderTargetWrites,
	Additive
};

enum class DepthStencilMode
{
	MarkMirror = 0,
	DrawReflection,
	NoDoubleBlend,
	AdditiveBlend,
	CountDepthComplexity,
	DrawDepthComplexity
};

#define DX3DError(message)\
{\
std::stringstream out;\
out << "DX3D Error: " << message << std::endl;\
OutputDebugStringA(out.str().c_str());\
throw std::runtime_error("");\
}

#define DX3DWarning(message)\
{\
std::stringstream out;\
out << "DX3D Warning: " << message << std::endl;\
OutputDebugStringA(out.str().c_str());\
}

#define DX3DInfo(message)\
{\
std::stringstream out;\
out << "DX3D Info: " << message << std::endl;\
OutputDebugStringA(out.str().c_str());\
}

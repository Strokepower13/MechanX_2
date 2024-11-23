#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>
#include <vector>

class PixelShader
{
public:
	PixelShader(const wchar_t* fullPath);
	~PixelShader();

private:
	std::vector<uint8_t> p_blob;

	friend class PipelineState;
};


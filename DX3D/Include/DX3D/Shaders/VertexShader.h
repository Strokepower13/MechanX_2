#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>
#include <vector>

class VertexShader
{
public:
	VertexShader(const wchar_t* fullPath);
	~VertexShader();

private:
	std::vector<uint8_t> p_blob;

	friend class PipelineState;
};


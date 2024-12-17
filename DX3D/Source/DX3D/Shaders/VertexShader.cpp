#include <DX3D/Shaders/VertexShader.h>
#include <DX3D/Utilities/ReadData.h>

VertexShader::VertexShader(const wchar_t* fullPath)
{
	p_blob = DX::ReadData(fullPath);
}

VertexShader::~VertexShader()
{
}

#include <DX3D/Graphics/PixelShader.h>
#include <DX3D/Utilities/ReadData.h>

PixelShader::PixelShader(const wchar_t* fullPath)
{
	p_blob = DX::ReadData(fullPath);
}

PixelShader::~PixelShader()
{
}

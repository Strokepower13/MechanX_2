#pragma once
#include <DX3D/AllHeaders.h>
#include <DirectXMath.h>

class InitD3D :public Game
{
public:
	InitD3D(HINSTANCE hInstance);
	~InitD3D();

protected:
	virtual void onCreate();
	virtual void onUpdate(float deltaTime);

private:
};


#pragma once
#include <DX3D/Prerequisites.h>
#include <Windows.h>
#include <Mouse.h>
#include <DirectXMath.h>

class InputSystem
{
public:
	InputSystem(HWND hWnd);
	~InputSystem();

	void update();

	bool isLButton();
	bool isRButton();
	DirectX::XMFLOAT2 getDeltaMousePosition() const;

private:
	std::unique_ptr<DirectX::Mouse> p_mouse;

	DirectX::XMFLOAT2 p_deltaMousePos{ 0.0f,0.0f };
};
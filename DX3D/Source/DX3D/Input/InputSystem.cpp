#include <DX3D/Input/InputSystem.h>

InputSystem::InputSystem(HWND hWnd)
{
	p_mouse = std::make_unique<DirectX::Mouse>();
	p_mouse->SetWindow(hWnd);
}

InputSystem::~InputSystem()
{
}

void InputSystem::update()
{
	auto state = p_mouse->GetState();

	if (state.positionMode == DirectX::Mouse::MODE_RELATIVE)
	{
		p_deltaMousePos = { float(state.x), float(state.y) };
	}
	p_mouse->SetMode(state.leftButton || state.rightButton ? DirectX::Mouse::MODE_RELATIVE : DirectX::Mouse::MODE_ABSOLUTE);
}

bool InputSystem::isLButton()
{
	auto state = p_mouse->GetState();
	return state.leftButton ? true : false;
}

bool InputSystem::isRButton()
{
	auto state = p_mouse->GetState();
	return state.rightButton ? true : false;
}

DirectX::XMFLOAT2 InputSystem::getDeltaMousePosition() const
{
	return p_deltaMousePos;
}

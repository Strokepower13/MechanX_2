#pragma once
#include <DX3D/Prerequisites.h>
#include <Windows.h>

class Window
{
public:
	Window(HINSTANCE hInstance);
	virtual ~Window();

	HWND getHwnd() const;
	int getClientWidth() const;
	int getClientHeight() const;
	void setClientWidth(int width);
	void setClientHeight(int height);

	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
	virtual void onFocus();
	virtual void onKillFocus();
	virtual void onSize();

protected:
	HINSTANCE p_hInstance;
	HWND p_hWnd;
	int p_clientWidth = 1024;
	int p_clientHeight = 768;
};
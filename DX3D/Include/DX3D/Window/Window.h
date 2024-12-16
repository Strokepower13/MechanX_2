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
	float getAspectRatio() const;

	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
	virtual void onFocus();
	virtual void onKillFocus();
	virtual void onSize();

protected:
	void setClientWidth(int width);
	void setClientHeight(int height);

	HINSTANCE p_hInstance;
	HWND p_hWnd;
	int p_clientWidth = 1024;
	int p_clientHeight = 768;

	bool p_minimized = false;
	bool p_maximized = false;
	bool p_resizing = false;

	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
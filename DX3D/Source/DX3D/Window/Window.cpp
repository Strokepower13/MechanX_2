#include <DX3D/Window/Window.h>
#include <Mouse.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		break;
	}

	case WM_SIZE:
	{
		Window* window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (window)
		{
			window->setClientWidth(LOWORD(lParam));
			window->setClientHeight(HIWORD(lParam));
			window->onSize();
		}
		break;
	}

	case WM_GETMINMAXINFO:
	{
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		break;
	}

	case WM_ACTIVATE:
	case WM_INPUT:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_SETFOCUS:
	{
		Window* window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (window) window->onFocus();
		break;
	}

	case WM_KILLFOCUS:
	{
		Window* window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		window->onKillFocus();
		break;
	}

	case WM_DESTROY:
	{
		Window* window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		window->onDestroy();
		break;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		break;
	}

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return NULL;
}

Window::Window(HINSTANCE hInstance)
{
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = &WndProc;
	wc.hInstance = p_hInstance;
	wc.hIcon = (HICON)LoadImage(hInstance, L"Assets/Logo/MechanXLogo.ico", IMAGE_ICON, 64, 64, LR_LOADFROMFILE | LR_SHARED);
	wc.hIconSm = (HICON)LoadImage(hInstance, L"Assets/Logo/MechanXLogoSm.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE | LR_SHARED);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszClassName = L"MechanXWindowClass";

	if (!RegisterClassEx(&wc))
		DX3DError("Window not created successfully.");

	RECT rc = { 0,0,p_clientWidth,p_clientHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	p_hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MechanXWindowClass", L"MechanX Application", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, p_hInstance, NULL);

	if (!p_hWnd)
		DX3DError("Window not created successfully.");

	SetWindowLongPtr(p_hWnd, GWLP_USERDATA, (LONG_PTR)this);
	ShowWindow(p_hWnd, SW_SHOW);
	UpdateWindow(p_hWnd);
}

Window::~Window()
{
	DestroyWindow(p_hWnd);
}

HWND Window::getHwnd() const
{
	return p_hWnd;
}

int Window::getClientWidth() const
{
	return p_clientWidth;
}

int Window::getClientHeight() const
{
	return p_clientHeight;
}

void Window::setClientWidth(int width)
{
	p_clientWidth = width;
}

void Window::setClientHeight(int height)
{
	p_clientHeight = height;
}

void Window::onCreate()
{
}

void Window::onUpdate()
{
}

void Window::onDestroy()
{
}

void Window::onFocus()
{
}

void Window::onKillFocus()
{
}

void Window::onSize()
{
}
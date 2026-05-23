#include "Window.h"

Window* Window::Instance = nullptr;

//	ウィンドウプロシージャー
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uiMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0L;
	}
	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

//Window::Window()
//	: WindowHandle()
//	, isQuitMessage()
//	, WindowClass()
//{
//}
//Window::~Window()
//{
//	UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
//}


/// <summary>
/// ウィンドウの初期化
/// </summary>
/// <param name="WindowName">ウィンドウ名</param>
/// <param name="x">x座標</param>
/// <param name="y">y座標</param>
/// <param name="isFullScreen">フルスクリーンか?</param>
/// <returns>true:成功</returns>
bool Window::Initialize(const char* WindowName, int x, int y)
{
	// ウインドウの設定
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WndProc;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hInstance = GetModuleHandle(nullptr);
	WindowClass.hIcon = LoadIcon(nullptr, IDC_ARROW);
	WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	WindowClass.lpszMenuName = nullptr;
	WindowClass.lpszClassName = WindowName;
	WindowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	RegisterClassEx(&WindowClass);

	RECT Rect = {};
	Rect.left = 0;
	Rect.top = 0;
	Rect.right = static_cast<LONG>(width);
	Rect.bottom = static_cast<LONG>(height);

	//	ウィンドウのスタイルに合わせた適切なサイズを取得する
	const DWORD WindowStyle = WS_OVERLAPPEDWINDOW;
	AdjustWindowRect(&Rect, WindowStyle, false);

	const LONG AdjustWidth = Rect.right - Rect.left;
	const LONG AdjustHeight = Rect.bottom - Rect.top;

	// ウインドウの生成
	WindowHandle = CreateWindow(
		WindowClass.lpszClassName,
		WindowName,
		WindowStyle,
		x, y,
		static_cast<int>(AdjustWidth),
		static_cast<int>(AdjustHeight),
		nullptr,
		nullptr,
		GetModuleHandle(NULL), NULL);
	if (WindowHandle == nullptr)
	{
		return false;
	}

	//	ウインドウの表示
	//	最大サイズで表示
	const int Commond = SW_SHOW;
	ShowWindow(WindowHandle, Commond);
	UpdateWindow(WindowHandle);

	return true;
}

void Window::Finalize()
{
	if (WindowClass.hInstance && WindowClass.lpszClassName)
	{
		UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
	}
	WindowHandle = nullptr;
}

/// <summary>
/// 更新メッセージの取得
/// </summary>
/// <returns></returns>
bool Window::IsUpdateMessage()
{
	MSG msg = {};
	// メッセージが存在するか確認
	if (PeekMessage(&msg, nullptr/*WindowHandle*/, 0, 0, PM_REMOVE))
	{
		// 終了通知が来ている場合は抜ける
		if (msg.message == WM_QUIT)
		{
			this->isQuitMessage = true;
		}
		else
		{
			// メッセージをウインドウプロシージャに転送
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	}
	return false;
}

/// <summary>
/// 終了メッセージ
/// </summary>
/// <returns></returns>
bool Window::IsQuitMessage()const
{
	return isQuitMessage;
}


/// <summary>
/// ウィンドウハンドルの取得
/// </summary>
/// <returns></returns>
const HWND Window::GetWindowHandle()const
{
	return WindowHandle;
}
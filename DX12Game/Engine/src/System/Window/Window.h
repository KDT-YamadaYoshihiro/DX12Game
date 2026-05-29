#pragma once
#include <Windows.h>
//#include "Utility/Singleton/Singleton.h"
#include "../../Utility/Singleton/Singleton.h"

class Window : public Singleton<Window>
{
public:
	/// <summary>
	/// Windowの横幅
	/// </summary>
	static constexpr float width = 1280.0f;
	/// <summary>
	/// Windowの縦幅
	/// </summary>
	static constexpr float height = 720.0f;

public:

	/// <summary>
	/// ウィンドウの初期化
	/// </summary>
	/// <param name="WindowName">ウィンドウ名</param>
	/// <param name="x">x座標</param>
	/// <param name="y">y座標</param>
	/// <param name="isFullScreen">フルスクリーンか?</param>
	/// <returns>true:成功</returns>
	bool Initialize(const char* WindowName, int x, int y);
	/// <summary>
	/// ウィンドウの終了処理（ウィンドウクラスの登録解除など、元のデストラクタの処理）
	/// </summary>
	void Finalize();
	/// <summary>
	/// 更新メッセージの取得
	/// </summary>
	/// <returns></returns>
	bool IsUpdateMessage();
	/// <summary>
	/// 終了メッセージ
	/// </summary>
	/// <returns></returns>
	bool IsQuitMessage()const;
	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	const HWND GetWindowHandle()const;
private:

	/// <summary>
	/// インスタンス
	/// </summary>
	static Window& createInstance()
	{
		static Window instance;
		return instance;
	}
	
	// コピームーブの禁止
	SINGLETON(Window)

	/// <summary>
	/// ウィンドウハンドル
	/// </summary>
	HWND WindowHandle;
	/// <summary>
	/// 終了通知
	/// </summary>
	bool isQuitMessage;
	/// <summary>
	/// ウィンドウクラス
	/// </summary>
	WNDCLASSEX WindowClass;

	static Window* Instance;
};
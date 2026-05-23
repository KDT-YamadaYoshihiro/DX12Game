#include <Windows.h>
#include "src/Utility/Singleton/Singleton.h"
#include "src/System/Window/Window.h"
#include "src/System/DirectX/DirectX.h"

//	メモリリークチェック用
#if _DEBUG
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>
#endif

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
#if _DEBUG
	//	メモリ解放漏れチェック用
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	// ウィンドウ初期化
	Window& window = Window::GetInstance();
	if (!window.Initialize("DirectX12", 0, 0))
	{
		return -1; // 初期化失敗時は終了
	}

	// DirectX初期化
	DirectX& dx = DirectX::GetInstance();
	if (!dx.Initialize(
		window.GetWindowHandle(),
		static_cast<UINT>(Window::width),
		static_cast<UINT>(Window::height)))
	{
		return -1; // 初期化失敗時は終了
	}
	while (window.IsQuitMessage() == false)
	{
		if (window.IsUpdateMessage() == false)
		{
			/*
			* メイン処理
			* ゲームの更新を実行する
			*/
			dx.BegineRendering(0.5f, 0.5f, 0.5f, 1.0f);
			/*
			* 描画処理
			* 画面をクリアしてフリップまで描画処理を行う
			*/
			dx.Flip();
		}
	}
	//	終了処理
	dx.Finalize();
	window.Finalize();;
	return 0;
}
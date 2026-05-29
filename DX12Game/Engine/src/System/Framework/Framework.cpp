#include "Framework.h"
#include "../Window/Window.h"
#include "../DirectX/DirectX.h"

bool Framework::Initialize()
{
    // Window初期化
    if (!Window::GetInstance().Initialize("DirectX12", 1280, 720)) return false;

    // DirectX初期化
    if (!DirectX::GetInstance().Initialize(
        Window::GetInstance().GetWindowHandle(),
        Window::width, Window::height)) return false;

    return true;
}

void Framework::Run()
{
    // メインループ
    while (!Window::GetInstance().IsQuitMessage())
    {
        if (!Window::GetInstance().IsUpdateMessage())
        {
            Update();
            Render();
        }
    }
}

bool Framework::Update()
{
    // ここにゲームの更新処理を追加
    return true;
}

void Framework::Render()
{
    auto& dx = DirectX::GetInstance();
    dx.BegineRendering(0.5f, 0.5f, 0.5f, 1.0f);
    // 描画処理
    dx.Flip();
}

void Framework::Finalize()
{
    DirectX::GetInstance().Finalize();
    Window::GetInstance().Finalize();
}
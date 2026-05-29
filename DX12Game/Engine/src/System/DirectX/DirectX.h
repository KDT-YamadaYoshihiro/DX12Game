#include <d3d12.h>
#include <dxgi1_6.h>
#include "../../Utility/Singleton/Singleton.h"

template<class T>
void SafeRelease(T*& t)
{
	if (t != nullptr)
	{
		t->Release();
		t = nullptr;
	}
}

/// <summary>
/// DirectX用クラス
/// </summary>
class DirectX : public Singleton<DirectX>
{
private:
	/// <summary>
	/// バックバッファ用リソース数
	/// </summary>
	static constexpr int NumBackBuffer = 2;
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="WindowHandle">ウィンドウのハンドル</param>
	/// <param name="Width">スクリーン横幅</param>
	/// <param name="Height">スクリーン縦幅</param>
	/// <returns>true:成功</returns>
	bool Initialize(HWND WindowHandle, UINT Width, UINT Height);

	/// <summary>
	/// 終了処理（元のデストラクタの処理）
	/// </summary>
	void Finalize();
	
	/// <summary>
	/// 描画開始
	/// </summary>
	/// <param name="Red">0~1.0<f/param>
	/// <param name="Green">0~1.0f</param>
	/// <param name="Blue">0~1.0f</param>
	/// <param name="Alpha">0~1.0f</param>
	void BegineRendering(float Red, float Green, float Blue, float Alpha);
	
	/// <summary>
	/// 画面のフリップ
	/// </summary>
	void Flip();

	/// <summary>
	/// GPUの処理待ち
	/// </summary>
	void WaitForGPU();

	/// <summary>
	/// ビューポートの設定
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void SetViewPort(float x, float y, float Width, float Height);

	/// <summary>
	/// Dx12デバイスの取得
	/// </summary>
	ID3D12Device* GetDevice()
	{
		return Device;
	}

	/// <summary>
	/// Dx12コマンドリストの取得
	/// </summary>
	ID3D12GraphicsCommandList* GetCommandList()
	{
		return CommandList;
	}

private:

	static DirectX& createInstance()
	{
		static DirectX instance;
		return instance;
	}
	// コピームーブの禁止
	SINGLETON(DirectX)

	/// <summary>
	/// デバッグレイヤーの起動
	/// </summary>
	void DebugLayerOn();

	/// <summary>
	/// デバイスの初期化
	/// </summary>
	/// <returns>true:成功</returns>
	bool InitializeDevice();

	/// <summary>
	/// ファクトリーの初期化
	/// </summary>
	/// <returns>true:成功</returns>
	bool InitializeFactory();

	/// <summary>
	/// コマンドリストの初期化
	/// </summary>
	/// <returns></returns>
	bool InitializeCommandList();

	/// <summary>
	/// スワップチェインの初期化
	/// </summary>
	/// <param name="WindowHandle"></param>
	/// <param name="Width"></param>
	/// <param name="Height"></param>
	/// <returns>true:成功</returns>
	bool InitializeSwapChain(HWND WindowHandle, UINT Width, UINT Height);
	
	/// <summary>
	/// バックバッファ用ディスクリプタヒープの初期化
	/// </summary>
	/// <returns>true:成功</returns>
	bool InitializeBackBufferHeap();

	/// <summary>
	/// 深度バッファ用ディスクリプタヒープの初期化
	/// </summary>
	/// <returns>true:成功</returns>
	bool InitializeDepthHeap();

	/// <summary>
	/// レンダーターゲットの初期化
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <returns>true:成功</returns>
	bool InitializeRenderTarget(UINT Width, UINT Height);

	/// <summary>
	/// フェンスの初期化
	/// </summary>
	/// <returns>true:成功</returns>
	bool InitializeFence();

private:


	/// <summary>
	/// Dx12デバイス
	/// </summary>
	ID3D12Device* Device;

	/// <summary>
	/// Dx12ファクトリー
	/// </summary>
	IDXGIFactory6* Factory;

	/// <summary>
	/// Dx12スワップチェイン
	/// </summary>
	IDXGISwapChain4* SwapChain;

	/// <summary>
	/// Dx12コマンドアロケーター
	/// </summary>
	ID3D12CommandAllocator* CommandAllocator;

	/// <summary>
	/// Dx12コマンドリスト
	/// </summary>
	ID3D12GraphicsCommandList* CommandList;

	/// <summary>
	/// Dx12コマンドキュー
	/// </summary>
	ID3D12CommandQueue* CommandQueue;

	/// <summary>
	/// Dx12バックバッファー
	/// </summary>
	ID3D12Resource* BackBuffers[NumBackBuffer];

	/// <summary>
	/// Dx12深度バッファ
	/// </summary>
	ID3D12Resource* DepthBuffer;

	/// <summary>
	/// Dx12バックバッファ用ヒープ
	/// </summary>
	ID3D12DescriptorHeap* BackBufferHeap;

	/// <summary>
	/// Dx12深度バッファ用ヒープ
	/// </summary>
	ID3D12DescriptorHeap* DepthHeap;

	/// <summary>
	/// Dx12フェンス
	/// </summary>
	ID3D12Fence* Fence;

	/// <summary>
	/// メモリ解放漏れ検知用
	/// </summary>
	ID3D12DebugDevice* DebugDevice;

	/// <summary>
	/// フェンス用
	/// </summary>
	UINT64 FenceValue;

	/// <summary>
	/// リソースバリア
	/// </summary>
	D3D12_RESOURCE_BARRIER ResourceBarrier;
	/// <summary>
	/// GPU待ちイベントハンドル
	/// </summary>
	HANDLE WaitForGPUEventHandle;
};
#include "DirectX.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

//DirectX::DirectX()
//	: Device(nullptr)
//	, Factory(nullptr)
//	, SwapChain(nullptr)
//	, CommandAllocator(nullptr)
//	, CommandList(nullptr)
//	, CommandQueue(nullptr)
//	, BackBuffers()
//	, DepthBuffer(nullptr)
//	, BackBufferHeap(nullptr)
//	, DepthHeap(nullptr)
//	, Fence(nullptr)
//	, DebugDevice(nullptr)
//	, FenceValue()
//	, ResourceBarrier()
//	, WaitForGPUEventHandle(nullptr)
//{
//}
//DirectX::~DirectX()
//{
//	WaitForGPU();
//
//	CloseHandle(WaitForGPUEventHandle);
//
//	for (auto& Buffer : BackBuffers)
//	{
//		SafeRelease(Buffer);
//	}
//	SafeRelease(DepthBuffer);
//	SafeRelease(BackBufferHeap);
//	SafeRelease(DepthHeap);
//
//	SafeRelease(CommandAllocator);
//	SafeRelease(CommandList);
//	SafeRelease(CommandQueue);
//	SafeRelease(SwapChain);
//
//	SafeRelease(Fence);
//	SafeRelease(Factory);
//
//	SafeRelease(Device);
//	if (DebugDevice != nullptr)
//	{
//		DebugDevice->Release();
//		DebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
//		DebugDevice = nullptr;
//	}
//}

/// <summary>
/// 初期化
/// </summary>
/// <param name="WindowHandle">ウィンドウのハンドル</param>
/// <param name="Width">スクリーン横幅</param>
/// <param name="Height">スクリーン縦幅</param>
/// <returns>true:成功</returns>
bool DirectX::Initialize(HWND WindowHandle, UINT Width, UINT Height)
{
	bool ret = false;
#if _DEBUG
	DebugLayerOn();
#endif
	//	ファクトリー初期化
	ret = InitializeFactory();
	if (ret == false)
	{
		return false;
	}
	//	デバイス初期化
	ret = InitializeDevice();
	if (ret == false)
	{
		return false;
	}
	//コマンドリスト初期化
	ret = InitializeCommandList();
	if (ret == false)
	{
		return false;
	}
	//	スワップチェイン初期化
	ret = InitializeSwapChain(WindowHandle, Width, Height);
	if (ret == false)
	{
		return false;
	}
	//	バックバッファ用ヒープ初期化
	ret = InitializeBackBufferHeap();
	if (ret == false)
	{
		return false;
	}
	//	深度バッファ用ヒープ初期化
	ret = InitializeDepthHeap();
	if (ret == false)
	{
		return false;
	}
	//	レンダーターゲット初期化
	ret = InitializeRenderTarget(Width, Height);
	if (ret == false)
	{
		return false;
	}
	//	フェンスの初期化
	ret = InitializeFence();
	if (ret == false)
	{
		return false;
	}

	return true;
}

void DirectX::Finalize()
{
	WaitForGPU();

	if (WaitForGPUEventHandle != nullptr)
	{
		CloseHandle(WaitForGPUEventHandle);
		WaitForGPUEventHandle = nullptr;
	}

	for (auto& Buffer : BackBuffers)
	{
		SafeRelease(Buffer);
	}
	SafeRelease(DepthBuffer);
	SafeRelease(BackBufferHeap);
	SafeRelease(DepthHeap);

	SafeRelease(CommandAllocator);
	SafeRelease(CommandList);
	SafeRelease(CommandQueue);
	SafeRelease(SwapChain);

	SafeRelease(Fence);
	SafeRelease(Factory);

	SafeRelease(Device);

	if (DebugDevice != nullptr)
	{
		DebugDevice->Release();
		DebugDevice = nullptr;
	}
}

/// <summary>
/// 描画開始
/// </summary>
/// <param name="Red">0~1.0<f/param>
/// <param name="Green">0~1.0f</param>
/// <param name="Blue">0~1.0f</param>
/// <param name="Alpha">0~1.0f</param>
void DirectX::BegineRendering(float Red, float Green, float Blue, float Alpha)
{
	const UINT bufferIndex = SwapChain->GetCurrentBackBufferIndex();

	// リソースバリアの設定
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.pResource = BackBuffers[bufferIndex];
	ResourceBarrier.Transition.Subresource = 0;
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	CommandList->ResourceBarrier(1, &ResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE heap = BackBufferHeap->GetCPUDescriptorHandleForHeapStart();
	const SIZE_T IncrementSize =
		static_cast<SIZE_T>(Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	heap.ptr += static_cast<SIZE_T>(bufferIndex) * IncrementSize;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvheap = DepthHeap->GetCPUDescriptorHandleForHeapStart();

	// レンダーターゲット設定
	CommandList->OMSetRenderTargets(1, &heap, true, &dsvheap);

	// レンダーターゲットクリア
	// 塗りつぶしの作業
	const float color[4] = { Red, Green, Blue, Alpha };
	CommandList->ClearRenderTargetView(heap, color, 0, nullptr);
	CommandList->ClearDepthStencilView(dsvheap, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}

/// <summary>
/// 画面のフリップ
/// </summary>
void DirectX::Flip()
{

	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	CommandList->ResourceBarrier(1, &ResourceBarrier);

	// 命令クローズ
	CommandList->Close();

	// コマンドリストの実行
	ID3D12CommandList* cmdlists[] = { CommandList };
	CommandQueue->ExecuteCommandLists(1, cmdlists);

	// 画面のスワイプ
	SwapChain->Present(0, 0);

	// GPU待ち
	WaitForGPU();

	// キューをクリア
	CommandAllocator->Reset();
	CommandList->Reset(CommandAllocator, nullptr);

}

/// <summary>
/// GPUの処理待ち
/// </summary>
void DirectX::WaitForGPU()
{
	// フェンスとの同期を開始
	CommandQueue->Signal(Fence, ++FenceValue);

	// GPUの処理待ち
	if (Fence->GetCompletedValue() < FenceValue)
	{
		// 完了する前に画面が出てしまうので絶対必要
		Fence->SetEventOnCompletion(FenceValue, WaitForGPUEventHandle);
		WaitForSingleObject(WaitForGPUEventHandle, INFINITE);
	}
}

/// <summary>
/// ビューポートの設定
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="width"></param>
/// <param name="height"></param>
void DirectX::SetViewPort(float x, float y, float Width, float Height)
{
	if (CommandList == nullptr)
	{
		return;
	}

	//	ビューポート設定
	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.Width = Width;
	viewport.Height = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	CommandList->RSSetViewports(1, &viewport);

	//	シザー矩形設定
	D3D12_RECT rect = {};
	rect.top = 0;
	rect.left = 0;
	rect.right = static_cast<LONG>(Width);
	rect.bottom = static_cast<LONG>(Height);
	CommandList->RSSetScissorRects(1, &rect);
}

/// <summary>
/// デバッグレイヤーの起動
/// </summary>
void DirectX::DebugLayerOn()
{
	ID3D12Debug* debugLater = nullptr;
	const HRESULT ret = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLater));
	if (SUCCEEDED(ret))
	{
		debugLater->EnableDebugLayer();
		debugLater->Release();
	}
}

/// <summary>
/// デバイスの初期化
/// </summary>
/// <returns>true:成功</returns>
bool DirectX::InitializeDevice()
{
	bool Success = false;

	for (UINT i = 0;; i++) {
		IDXGIAdapter* Adapter = nullptr;
		if (Factory->EnumAdapters(i, &Adapter) == DXGI_ERROR_NOT_FOUND) {
			break;
		}

		// 
		DXGI_ADAPTER_DESC adapterDesc = {};
		Adapter->GetDesc(&adapterDesc);

		const D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_12_0;
		// デバイスの初期化
		const HRESULT ret = D3D12CreateDevice(Adapter, level, IID_PPV_ARGS(&Device));
		Adapter->Release();
		if (SUCCEEDED(ret)) {
			Success = true;
			break;
		}

	}

#if _DEBUG

	// デバック用のデバイスの有効
	if (Device != nullptr)
	{
		Device->QueryInterface(IID_PPV_ARGS(&DebugDevice));
	}

#endif

	return Success;
}

/// <summary>
/// ファクトリーの初期化
/// </summary>
/// <returns>true:成功</returns>
bool DirectX::InitializeFactory()
{
#if _DEBUG
	const HRESULT ret = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&Factory));
#else
	const HRESULT ret = CreateDXGIFactory1(IID_PPV_ARGS(&Factory));
#endif
	if (FAILED(ret))
	{
		return false;
	}
	return true;
}

/// <summary>
/// コマンドリストの初期化
/// </summary>
/// <returns></returns>
bool DirectX::InitializeCommandList()
{
	//	コマンドアロケーターの初期化
	// 
	HRESULT ret = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator));
	if (FAILED(ret)) {
		return false;
	}

	//	コマンドリストの初期化
	ret = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator, nullptr, IID_PPV_ARGS(&CommandList));
	if (FAILED(ret))
	{
		return false;
	}

	return true;
}

/// <summary>
/// スワップチェインの初期化
/// </summary>
/// <param name="WindowHandle"></param>
/// <param name="Width"></param>
/// <param name="Height"></param>
/// <returns>true:成功</returns>
bool DirectX::InitializeSwapChain(HWND WindowHandle, UINT Width, UINT Height)
{
	//	コマンドキュー初期化
	D3D12_COMMAND_QUEUE_DESC cmdQueue = {};
	cmdQueue.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueue.NodeMask = 0;
	cmdQueue.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueue.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT ret = Device->CreateCommandQueue(&cmdQueue, IID_PPV_ARGS(&CommandQueue));
	if (FAILED(ret))
	{
		return false;
	}

	//	スワップチェイン初期化
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = Width;
	scDesc.Height = Height;
	scDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Stereo = false;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	scDesc.BufferCount = NumBackBuffer;
	scDesc.Scaling = DXGI_SCALING_STRETCH;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ret = Factory->CreateSwapChainForHwnd(CommandQueue, WindowHandle, &scDesc, nullptr, nullptr, (IDXGISwapChain1**)&SwapChain);
	if (FAILED(ret)) {
		return false;
	}

	return true;
}

/// <summary>
/// バックバッファ用ディスクリプタヒープの初期化
/// </summary>
/// <returns>true:成功</returns>
bool DirectX::InitializeBackBufferHeap()
{
	//	ディスクリプタヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC dchDesc = {};
	// レンダーターゲットビュー（ＲＴＶ）
	dchDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	dchDesc.NodeMask = 0;
	dchDesc.NumDescriptors = NumBackBuffer;
	dchDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// ディスクリプタヒープ生成
	const HRESULT ret = Device->CreateDescriptorHeap(&dchDesc, IID_PPV_ARGS(&BackBufferHeap));
	if (FAILED(ret)) {
		return false;
	}

	return true;
}

/// <summary>
/// 深度バッファ用ディスクリプタヒープの初期化
/// </summary>
/// <returns>true:成功</returns>
bool DirectX::InitializeDepthHeap()
{
	//	深度用ディスクリプターヒープ
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	const HRESULT ret = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&DepthHeap));
	if (FAILED(ret))
	{
		return false;
	}

	return true;
}

/// <summary>
/// レンダーターゲットの初期化
/// </summary>
/// <param name="width"></param>
/// <param name="height"></param>
/// <returns>true:成功</returns>
bool DirectX::InitializeRenderTarget(UINT Width, UINT Height)
{
	//	スワップチェインからバックバッファの取得
	HRESULT ret = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE handle = BackBufferHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT index = 0; index < NumBackBuffer; index++)
	{
		// バックバッファー取得
		ret = SwapChain->GetBuffer(index, IID_PPV_ARGS(&BackBuffers[index]));
		if (FAILED(ret)) {
			OutputDebugString("BackBufferの取得に失敗");
			return false;
		}
		Device->CreateRenderTargetView(BackBuffers[index], nullptr, handle);
		handle.ptr += Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// 2次元テクスチャを深度バッファとして
	//	深度バッファ作成
	D3D12_RESOURCE_DESC depthResource = {};
	depthResource.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResource.Width = static_cast<UINT64>(Width);
	depthResource.Height = Height;
	depthResource.DepthOrArraySize = 1;
	depthResource.Format = DXGI_FORMAT_D32_FLOAT;
	depthResource.SampleDesc.Count = 1;
	depthResource.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 深度用ヒープ
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// クリアバリュー
	D3D12_CLEAR_VALUE clearvalue = {};
	clearvalue.DepthStencil.Depth = 1.0f;
	clearvalue.Format = DXGI_FORMAT_D32_FLOAT;

	// リソースを作成
	ret = Device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &depthResource,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearvalue, IID_PPV_ARGS(&DepthBuffer));
	if (FAILED(ret))
	{
		return false;
	}

	// 深度ビュー
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	Device->CreateDepthStencilView(DepthBuffer, &dsvDesc, DepthHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}

/// <summary>
/// フェンスの初期化
/// </summary>
/// <returns>true:成功</returns>
bool DirectX::InitializeFence()
{
	//	フェンスの作成
	// 画面上に出していいかチェックする
	const HRESULT ret = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
	if (FAILED(ret)) {
		return false;
	}

	// 後の同期待ちの為に値が大きくないといけない
	FenceValue = 1;

	//	同期待ち用イベントハンドルの作成
	WaitForGPUEventHandle = CreateEvent(nullptr, false, false, nullptr);
	if (WaitForGPUEventHandle == nullptr)
	{
		return false;
	}

	return true;
}
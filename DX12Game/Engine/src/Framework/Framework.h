#pragma once
#include "../System/pch/pch.h"
#include "../Utility/Singleton/Singleton.h"

class Framework : public Singleton<Framework>
{
private:
	// コピームーブの禁止
	SINGLETON(Framework)
public:
	/// <summary>
	/// フレームワークの初期化
	/// </summary>
	/// <returns>true:成功</returns>
	bool Initialize();
	/// <summary>
	/// フレームワークの終了処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// フレームワークの更新
	/// </summary>
	void Run();

private:

	static Framework& createInstance()
	{
		static Framework instance;
		return instance;
	}

	bool Update();
	void Render();
};
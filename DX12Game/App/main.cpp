#include <Windows.h>
#include "src/System/Framework/Framework.h"

//	メモリリークチェック用
#if _DEBUG
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>
#endif

#if _DEBUG
int main()
#else
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#endif // _DEBUG
{
#if _DEBUG
	//	メモリ解放漏れチェック用
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    auto& framework = Framework::GetInstance();

    if (framework.Initialize())
    {
        framework.Run();
    }

    framework.Finalize();

    return 0;
}
#pragma once
// --------------
// プリコンパイル済みのヘッダー
// --------------

// --------------
// 基本
// --------------
#pragma comment(lib, "winmm.lib")

#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <cassert>

#include <wrl/client.h>

// --------------
// STL
// --------------
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <list>
#include <iterator>
#include <queue>
#include <algorithm>
#include <memory>
#include <random>
#include <fstream>
#include <sstream>
#include <functional>
#include <thread>
#include <mutex>
#include <future>
#include <filesystem>
#include <chrono>

#define _USE_MATH_DEFINES
#include <cmath>

#include "../System.h"

// --------------
// DX12
// --------------
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

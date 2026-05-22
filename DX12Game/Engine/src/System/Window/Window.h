#pragma once
#include "Utility/Singleton/Singleton.h"

class Window : public Singleton<Window>
{
private:
	SINGLETON(Window)

public:
	friend class Singleton<Window>;

	
};


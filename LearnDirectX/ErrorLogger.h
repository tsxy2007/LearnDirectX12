#pragma once

#include "StringConverter.h"
#include <windows.h>
class ErrorLogger
{
public:
	static void Log(std::string message);
	static void Log(HRESULT hr, std::string message);
};
#pragma once

#include <string>

namespace ZenEngine
{
	namespace FileDialog
	{
		std::string OpenFile(const char* filter);
		std::string SaveFile(const char* filter);
	}
}
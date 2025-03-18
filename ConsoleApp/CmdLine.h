#pragma once

#include <string>
#include <vector>

enum class Command {
	Default,
	StartHere,
	Error
};

class CmdLine
{
public:
	Command command{ Command::Error };
	std::wstring path;
	std::vector<std::wstring> addArgs;

	bool Parse(int argc, const wchar_t* const* argv);
};

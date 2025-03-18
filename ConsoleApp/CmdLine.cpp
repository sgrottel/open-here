#include "CmdLine.h"

#include <yaclap.hpp>

bool CmdLine::Parse(int argc, const wchar_t* const* argv)
{
    using Parser = yaclap::Parser<wchar_t>;
    using Command = yaclap::Command<wchar_t>;
    using Argument = yaclap::Argument<wchar_t>;

    Parser parser{ L"OpenHereCon.exe", L"Command line client of Open Here" };

    Command startHereCommand{ L"start", L"Starts another process at the location of the detected open file explorer" };
    Argument startPathArgument{ L"path", L"The executable to be started" };
    startHereCommand.Add(startPathArgument);
    parser.Add(startHereCommand);
    parser.SetErrorOnUnmatchedArguments(false); // should only be for when the command is active

    Parser::Result res = parser.Parse(argc, argv);

    if (res.HasCommand(startHereCommand))
    {
        command = ::Command::StartHere;

        auto pathA = res.GetArgument(startPathArgument);
        if (pathA)
        {
            path = pathA.data();
        }

        for (const auto& a : res.UnmatchedArguments())
        {
            addArgs.push_back(std::wstring{ a.data() });
        }
    }
    else
    {
        command = ::Command::Default;
    }

    parser.PrintErrorAndHelpIfNeeded(res);
    return res.IsSuccess() && !res.ShouldShowHelp();
}

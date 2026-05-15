#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif


#include "run_command.hpp"
#include "utils/file_utills.hpp"
#include "utils/string_utils.hpp"

#include <print>

void RunCommand::Execute(const std::vector<std::string> &args) {
    if (args.empty()) {
        return;
    }

    const auto clean_command = utils::Unquote(args[0]);
    auto path = utils::GetCommandPath(clean_command);
    if (!path) {
        std::println(stderr, "{}: not found", args[0]);
        return;
    }

#ifdef _WIN32
    RunWindows(*path, args);
#else
    RunPosix(*path, args);
#endif
}


#ifdef _WIN32
void RunCommand::RunWindows(const std::string &full_path, const std::vector<std::string> &args) const {
    if (args.empty()) {
        return;
    }

    std::string cmdLine;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) {
            cmdLine += " ";
        }

        const std::string& arg = args[i];

        bool needs_quotes = arg.find(' ') != std::string::npos ||
                            arg.find('\t') != std::string::npos ||
                            arg.find('"') != std::string::npos;

        if (needs_quotes) {
            cmdLine += '"';
            for (char c : arg) {
                if (c == '"') {
                    cmdLine += '\\';
                }
                cmdLine += c;
            }
            cmdLine += '"';
        }
        else {
            cmdLine += arg;
        }
    }

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcessA(
            full_path.c_str(),
            cmdLine.data(),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &si,
            &pi))
    {
        std::println(stderr, "CreateProcess failed: {}", GetLastError());
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
#endif


bool RunCommand::IsStateChanging() const {
    return false;
}

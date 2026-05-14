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
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::string cmdLine = "\"" + args[0] + "\"";

    for (size_t i = 1; i < args.size(); ++i) {
        cmdLine += " " + args[i];
    }

    if (!CreateProcessA(full_path.c_str(),
        cmdLine.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi)) {
        return;
        }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}


#else
void RunCommand::RunPosix(const std::string& path, const std::vector<std::string>& args) const {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
    }
    else if (pid == 0) {
        std::vector<char*> c_args;

        c_args.push_back(const_cast<char*>(args[0].c_str()));

        for (size_t i = 1; i < args.size(); ++i) {
            c_args.push_back(const_cast<char*>(args[i].c_str()));
        }
        c_args.push_back(nullptr);

        execv(path.c_str(), c_args.data());

        perror("execv");
        exit(1);
    }
    else {
        int status;
        waitpid(pid, &status, 0);
    }
}
#endif


bool RunCommand::IsStateChanging() const {
    return false;
}

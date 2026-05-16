#include "executor.hpp"
#include "commands/run_command.hpp"
#include "commands/command_repository/command_repository.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <fcntl.h>
#include <ranges>
#include <sys/stat.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <io.h>
    #define CLOSE _close
    #define DUP2 _dup2
    #define OPEN _open
    #define STDOUT_FD 1
    #define STDERR_FD 2
    #define READ_MODE _S_IREAD
    #define WRITE_MODE _S_IWRITE
    #define DUP _dup
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #define CLOSE close
    #define DUP2 dup2
    #define OPEN open
    #define STDOUT_FD STDOUT_FILENO
    #define READ_MODE 0644
    #define WRITE_MODE 0644
#endif


bool Redirector::Apply(const Redirections& redirects) {
    for (const auto& redir : redirects) {
        switch (redir.redirect_type) {
            case RedirectType::STDOUT_REWRITE:
                if (!SetupStdout(redir.filename, false)) {
                    return false;
                }
                break;
            case RedirectType::STDOUT_APPEND:
                if (!SetupStdout(redir.filename, true)) {
                    return false;
                }
                break;
            case RedirectType::STDERR:
                if (!SetupStderr(redir.filename, false)) {
                    return false;
                }
                break;
            case RedirectType::STDERR_APPEND:
                if (!SetupStderr(redir.filename, true)) {
                    return false;
                }
                break;
            default:
                return false;
        }
    }
    return true;
}


bool Redirector::SetupStdout(const std::string& path, bool append) {
    int flags = O_WRONLY | O_CREAT;

    if (append) {
        flags |= O_APPEND;
    } else {
        flags |= O_TRUNC;
    }

#ifdef _WIN32
    flags |= _O_TEXT;
#endif

    int fd = OPEN(path.c_str(), flags, WRITE_MODE);
    if (fd < 0) {
        return false;
    }

    if (DUP2(fd, STDOUT_FD) < 0) {
        CLOSE(fd);
        return false;
    }

    CLOSE(fd);
    return true;
}


bool Redirector::SetupStderr(const std::string& path, bool append) {
    const char* mode = append ? "ab" : "wb";

    if (std::freopen(path.c_str(), mode, stderr) == nullptr) {
        return false;
    }

    int fd = fileno(stderr);
    if (fd != 2) {
        DUP2(fd, 2);
    }

    return true;
}


void CommandExecutor::Execute(const Args& args, const std::function<void(const Tokens&)>& command) {
    std::fflush(stdout);
    std::fflush(stderr);
    std::cout.flush();
    std::cerr.flush();

#if !defined(_WIN32) && !defined(_WIN64)
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }
    if (pid == 0) {
        if (!Redirector::Apply(redirects)) {
            exit(1);
        }
        command(args);
        exit(0);
    }
    else {
        waitpid(pid, nullptr, 0);
    }
#else
    int saved_stdout = DUP(STDOUT_FD);
    int saved_stderr = DUP(STDERR_FD);

    if (Redirector::Apply(args.redirections)) {
        command(args.tokens);
        std::cout.flush();
        std::cerr.flush();
    }

    if (saved_stdout != -1) {
        DUP2(saved_stdout, STDOUT_FD);
        CLOSE(saved_stdout);
    }
    if (saved_stderr != -1) {
        DUP2(saved_stderr, STDERR_FD);
        CLOSE(saved_stderr);
    }
#endif
}


void CommandExecutor::Process(const Args &args) {
    if (args.tokens.empty()) {
        return;
    }

    const auto& command_name = args.tokens[0];
    auto builtin_opt = CommandRepository::instance().Create(command_name);

    if (builtin_opt) {
        HandleBuiltin(std::move(*builtin_opt), args);
    }
    else {
        HandleExternal(args);
    }
}


void CommandExecutor::HandleBuiltin(std::unique_ptr<ICommand> cmd, const Args &args) {
    if (cmd->IsStateChanging()) {
        cmd->Execute(args.tokens);
    }
    else {
        Execute(args, [&](const Tokens& inner_args) {
            cmd->Execute(inner_args);
        });
    }
}


void CommandExecutor::HandleExternal(const Args &args) {
    static RunCommand external_handler;
    Execute(args,[&](const Tokens& inner_args) {
        external_handler.Execute(inner_args);
    });
}

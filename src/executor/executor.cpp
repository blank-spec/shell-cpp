#include "executor.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <fcntl.h>
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
    int flags = O_WRONLY | O_CREAT;
    flags |= (append ? O_APPEND : O_TRUNC);
    int fd = OPEN(path.c_str(), flags, 0644);
    if (fd == -1) return false;

    if (DUP2(fd, 2) == -1) {
        CLOSE(fd);
        return false;
    }

#ifndef _WIN32
    if (std::freopen(nullptr, append ? "a" : "w", stderr) == nullptr) {
        std::freopen(path.c_str(), append ? "a" : "w", stderr);
    }
#endif

    CLOSE(fd);
    return true;
}


void CommandExecutor::Execute(const std::vector<Redirection> &redirects,
                              const std::function<void(const Tokens &)> &command,
                              const Tokens& args) {
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

    if (Redirector::Apply(redirects)) {
        command(args);
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

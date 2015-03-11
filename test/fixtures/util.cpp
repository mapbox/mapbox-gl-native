#include "util.hpp"

#include <mbgl/platform/log.hpp>

#include <csignal>

namespace mbgl {
namespace test {

pid_t startServer(const char *executable) {
    const std::string parent_pid = std::to_string(getpid());
    pid_t pid = fork();
    if (pid < 0) {
        throw std::runtime_error("Cannot create server process");
    } else if (pid == 0) {
        char *args[] = { const_cast<char *>(executable),
                         const_cast<char *>(parent_pid.c_str()),
                         nullptr };
        int ret = execv(executable, args);
        // This call should not return. In case execve failed, we exit anyway.
        if (ret < 0) {
            Log::Error(Event::Setup, "failed to start server: %s", strerror(errno));
        }
        exit(0);
    } else {
        // Wait until the server process sends SIGCONT.
        raise(SIGSTOP);
    }
    return pid;
}

void stopServer(pid_t pid) {
    kill(pid, SIGTERM);
}

}
}

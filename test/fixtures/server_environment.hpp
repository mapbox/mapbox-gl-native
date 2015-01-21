#ifndef MBGL_TEST_FIXTURES_SERVER_ENVIRONMENT
#define MBGL_TEST_FIXTURES_SERVER_ENVIRONMENT

#include <gtest/gtest.h>

#include <dirent.h>
#include <signal.h>
#include <libgen.h>

class ServerEnvironment : public ::testing::Environment {
public:
    inline ServerEnvironment(const std::string &executable);
    inline virtual void SetUp();
    inline virtual void TearDown();

private:
    const std::string executable;
    const std::string parent_pid = std::to_string(getpid());
    pid_t pid = 0;
};

ServerEnvironment::ServerEnvironment(const std::string &executable_) : executable(executable_) {}

void ServerEnvironment::SetUp() {
    pid = fork();
    if (pid < 0) {
        throw std::runtime_error("Cannot create server process");
    } else if (pid == 0) {
        char *arg[] = {
            const_cast<char *>(executable.c_str()),
            const_cast<char *>(parent_pid.c_str()),
            nullptr
        };
        int ret = execv(executable.c_str(), arg);
        // This call should not return. In case execve failed, we exit anyway.
        if (ret < 0) {
            fprintf(stderr, "Failed to start server: %s\n", strerror(errno));
        }
        exit(0);
    } else {
        // Wait until the server process sends SIGCONT.
        raise(SIGSTOP);
    }
}

void ServerEnvironment::TearDown() {
    ASSERT_TRUE(pid);
    kill(pid, SIGHUP);
}

#endif
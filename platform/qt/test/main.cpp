#include <QApplication>

#include <mbgl/test.hpp>
#include <cstring>
#include <cerrno>
#include <cstdio>

#if defined(_MSC_VER) && !defined(__clang__)
#include <direct.h>
#else
#include <unistd.h>
#endif

#define xstr(s) str(s)
#define str(s) #s

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#ifdef WORK_DIRECTORY
    const int result = chdir(xstr(WORK_DIRECTORY));
    if (result != 0) {
        fprintf(stderr, "failed to change directory: %s\n", strerror(errno));
        return errno;
    }
#endif

    return mbgl::runTests(argc, argv);
}

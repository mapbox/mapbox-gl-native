#include <QApplication>

#include <mbgl/test.hpp>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <cstdio>

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

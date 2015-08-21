#ifndef MBGL_TEST_STORAGE_STORAGE
#define MBGL_TEST_STORAGE_STORAGE

#include "../fixtures/util.hpp"
#include <uv.h>

class Storage : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

protected:
    static pid_t pid;
};

#endif

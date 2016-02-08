#ifndef MBGL_TEST_STORAGE_STORAGE
#define MBGL_TEST_STORAGE_STORAGE

#include "../fixtures/util.hpp"
#include <mbgl/storage/response.hpp>
#include <memory>

class Storage : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

protected:
    static std::unique_ptr<mbgl::test::Server> server;
};

#endif

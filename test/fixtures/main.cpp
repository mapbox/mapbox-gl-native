#include "util.hpp"

#include <libgen.h>

std::string baseDirectory;

namespace mbgl {
namespace test {

const std::string &getBaseDirectory() {
    return baseDirectory;
}

}
}

GTEST_API_ int main(int argc, char *argv[]) {
    // Note: glibc's dirname() **modifies** the argument and can't handle static strings.
    std::string file { __FILE__ };
    file = dirname(const_cast<char *>(file.c_str()));
    if (file[0] == '/') {
        // If __FILE__ is an absolute path, we don't have to guess from the argv 0.
        baseDirectory = file + "/..";
    } else {
        std::string argv0 { argv[0] }; argv0 = dirname(const_cast<char *>(argv0.c_str()));
        baseDirectory = argv0 + "/" + file + "/..";
    }

    fprintf(stderr, "basedir: %s\n", mbgl::test::getBaseDirectory().c_str());

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

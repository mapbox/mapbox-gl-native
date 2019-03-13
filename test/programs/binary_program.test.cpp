#include <mbgl/test/util.hpp>

#include <mbgl/gl/binary_program.hpp>

using namespace mbgl;

TEST(BinaryProgram, ObtainValues) {
    const gl::BinaryProgram binaryProgram{ 42,
                                           "binary code",
                                           "identifier",
                                           { { "a_pos", 1 }, { "a_data", 4 } },
                                           { { "u_world", 1 }, { "u_ratio", 3 } },
                                           { { "u_image", 0 } } };

    EXPECT_EQ(42u, binaryProgram.format());
    EXPECT_EQ("binary code", binaryProgram.code());
    EXPECT_EQ("identifier", binaryProgram.identifier());
    EXPECT_EQ(1u, binaryProgram.attributeLocation("a_pos"));
    EXPECT_FALSE(binaryProgram.attributeLocation("u_world"));
    EXPECT_EQ(4u, binaryProgram.attributeLocation("a_data"));
    EXPECT_EQ(1, binaryProgram.uniformLocation("u_world"));
    EXPECT_EQ(3, binaryProgram.uniformLocation("u_ratio"));
    EXPECT_EQ(-1, binaryProgram.uniformLocation("a_data"));
    EXPECT_EQ(0, binaryProgram.textureLocation("u_image"));
    EXPECT_EQ(-1, binaryProgram.textureLocation("u_image2"));

    auto serialized = binaryProgram.serialize();

    const gl::BinaryProgram binaryProgram2(std::move(serialized));

    EXPECT_EQ(42u, binaryProgram2.format());
    EXPECT_EQ("binary code", binaryProgram2.code());
    EXPECT_EQ("identifier", binaryProgram2.identifier());
    EXPECT_EQ(1u, binaryProgram2.attributeLocation("a_pos"));
    EXPECT_FALSE(binaryProgram2.attributeLocation("u_world"));
    EXPECT_EQ(4u, binaryProgram2.attributeLocation("a_data"));
    EXPECT_EQ(1, binaryProgram2.uniformLocation("u_world"));
    EXPECT_EQ(3, binaryProgram2.uniformLocation("u_ratio"));
    EXPECT_EQ(-1, binaryProgram2.uniformLocation("a_data"));
    EXPECT_EQ(0, binaryProgram.textureLocation("u_image"));
    EXPECT_EQ(-1, binaryProgram.textureLocation("u_image2"));

    EXPECT_THROW(gl::BinaryProgram(""), std::runtime_error);
}

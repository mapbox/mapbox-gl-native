#include <mbgl/test/util.hpp>

#include <mbgl/programs/binary_program.hpp>

using namespace mbgl;

TEST(BinaryProgram, ObtainValues) {
    const BinaryProgram binaryProgram{ 42,
                                       "binary code",
                                       "identifier",
                                       { { "a_pos", 1 }, { "a_data", 4 } },
                                       { { "u_world", 1 }, { "u_ratio", 3 } } };

    EXPECT_EQ(42u, binaryProgram.format());
    EXPECT_EQ("binary code", binaryProgram.code());
    EXPECT_EQ("identifier", binaryProgram.identifier());
    EXPECT_EQ(1, binaryProgram.attributeLocation("a_pos"));
    EXPECT_EQ(0, binaryProgram.attributeLocation("u_world"));
    EXPECT_EQ(4, binaryProgram.attributeLocation("a_data"));
    EXPECT_EQ(1, binaryProgram.uniformLocation("u_world"));
    EXPECT_EQ(3, binaryProgram.uniformLocation("u_ratio"));
    EXPECT_EQ(0, binaryProgram.uniformLocation("a_data"));

    auto serialized = binaryProgram.serialize();

    const BinaryProgram binaryProgram2(std::move(serialized));

    EXPECT_EQ(42u, binaryProgram2.format());
    EXPECT_EQ("binary code", binaryProgram2.code());
    EXPECT_EQ("identifier", binaryProgram2.identifier());
    EXPECT_EQ(1, binaryProgram2.attributeLocation("a_pos"));
    EXPECT_EQ(0, binaryProgram2.attributeLocation("u_world"));
    EXPECT_EQ(4, binaryProgram2.attributeLocation("a_data"));
    EXPECT_EQ(1, binaryProgram2.uniformLocation("u_world"));
    EXPECT_EQ(3, binaryProgram2.uniformLocation("u_ratio"));
    EXPECT_EQ(0, binaryProgram2.uniformLocation("a_data"));

    EXPECT_THROW(BinaryProgram(""), std::runtime_error);
}

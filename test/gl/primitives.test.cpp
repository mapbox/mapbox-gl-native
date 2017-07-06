#include <mbgl/test/util.hpp>

#include <mbgl/renderer/indexed_primitives.hpp>
#include <mbgl/gl/draw_mode.hpp>

#include <mbgl/programs/fill_program.hpp>

namespace mbgl {
namespace gl {

std::ostream& operator<<(std::ostream& os, const SegmentInfo& info) {
    return os << "{ " << info.vertexOffset << ", " << info.indexOffset
               << ", " << info.vertexLength << ", " << info.indexLength << " }";
}

} // namespace gl
} // namespace mbgl

using namespace mbgl;

TEST(Primitives, Adding) {
    IndexedPrimitives<gl::Triangles, FillLayoutVertex, FillAttributes> primitives;

    primitives.add({
        FillProgram::layoutVertex({ 0, 0 }),
        FillProgram::layoutVertex({ util::EXTENT, 0 }),
        FillProgram::layoutVertex({ 0, util::EXTENT }),
        FillProgram::layoutVertex({ util::EXTENT, util::EXTENT }),
    }, {
        {{ 0, 1, 2 }},
        {{ 1, 2, 3 }},
    });

    primitives.add({
        FillProgram::layoutVertex({ 0, 0 }),
        FillProgram::layoutVertex({ util::EXTENT, 0 }),
        FillProgram::layoutVertex({ 0, util::EXTENT }),
        FillProgram::layoutVertex({ util::EXTENT, util::EXTENT }),
    }, {
        {{ 0, 1, 2 }},
        {{ 1, 2, 3 }},
    });

    EXPECT_EQ(
        (std::vector<FillLayoutVertex>{
            FillProgram::layoutVertex({ 0, 0 }),
            FillProgram::layoutVertex({ util::EXTENT, 0 }),
            FillProgram::layoutVertex({ 0, util::EXTENT }),
            FillProgram::layoutVertex({ util::EXTENT, util::EXTENT }),
            FillProgram::layoutVertex({ 0, 0 }),
            FillProgram::layoutVertex({ util::EXTENT, 0 }),
            FillProgram::layoutVertex({ 0, util::EXTENT }),
            FillProgram::layoutVertex({ util::EXTENT, util::EXTENT }),
        }),
        primitives.getVertices().vector());

    EXPECT_EQ(
        (std::vector<uint16_t>{
            0, 1, 2,
            1, 2, 3,
            4, 5, 6,
            5, 6, 7,
        }),
        primitives.getIndices().vector());


    EXPECT_EQ(
        (gl::SegmentInfoVector{
            { 0, 0, 8, 12 }
        }),
        primitives.getSegmentInfo());
}

TEST(Primitives, OverflowVertices) {
    IndexedPrimitives<gl::Triangles, FillLayoutVertex, FillAttributes> primitives;

    for (int32_t i = 0; i < 21846; i++) {
        primitives.add({
            FillProgram::layoutVertex({ 0, 0 }),
            FillProgram::layoutVertex({ 0, 0 }),
            FillProgram::layoutVertex({ 0, 0 }),
        }, {
            {{ 0, 1, 2 }},
        });
    }

    EXPECT_EQ(
        (gl::SegmentInfoVector{
            { 0, 0, 65535, 65535 },
            { 65535, 65535, 3, 3 }
        }),
        primitives.getSegmentInfo());
}

TEST(Primitives, OverflowIndices) {
    IndexedPrimitives<gl::Triangles, FillLayoutVertex, FillAttributes> primitives;

    for (int32_t i = 0; i < 65536; i++) {
        primitives.add({
            FillProgram::layoutVertex({ 0, 0 }),
        }, {
            {{ 0, 0, 0 }},
        });
    }

    EXPECT_EQ(
        (gl::SegmentInfoVector{
            { 0, 0, 65535, 196605 },
            { 65535, 196605, 1, 3 }
        }),
        primitives.getSegmentInfo());
}

TEST(Primitives, InvalidIndex) {
    IndexedPrimitives<gl::Triangles, FillLayoutVertex, FillAttributes> primitives;

    EXPECT_THROW(
        primitives.add({
            FillProgram::layoutVertex({ 0, 0 }),
        }, {
            {{ 0, 1, 2 }},
        }),
        std::out_of_range
    );
}

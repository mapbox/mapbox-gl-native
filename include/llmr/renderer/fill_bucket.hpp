#ifndef LLMR_RENDERER_FILLBUCKET
#define LLMR_RENDERER_FILLBUCKET

#include "bucket.hpp"
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/vao.hpp>
#include <llmr/geometry/fill_buffer.hpp>

#include <vector>
#include <memory>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif

namespace llmr {

class Style;
class FillBuffer;
class BucketDescription;
struct Coordinate;
struct pbf;

class FillBucket : public Bucket {
public:
    FillBucket(const std::shared_ptr<FillBuffer>& buffer, const BucketDescription& bucket_desc);

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);

    void addGeometry(pbf& data);
    void addGeometry(const std::vector<Coordinate>& line);
    uint32_t size() const;

    template <typename Shader> void drawElements(Shader& shader);
    template <typename Shader> void drawVertices(Shader& shader);

public:
    const BucketGeometryDescription geom_desc;

private:
    std::shared_ptr<FillBuffer> buffer;

    struct group {
        VertexArrayObject array;
        uint32_t vertex_length;
        uint32_t elements_length;

        group() : vertex_length(0), elements_length(0) {}
        group(uint32_t vertex_length, uint32_t elements_length)
            : vertex_length(vertex_length),
              elements_length(elements_length) {
        }
    };

    // hold information on where the vertices are located in the FillBuffer
    uint32_t vertex_start;
    uint32_t elements_start;
    uint32_t length;
    VertexArrayObject array;
    std::vector<group> groups;
};


template <typename Shader>
void FillBucket::drawElements(Shader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * 2 * sizeof(int16_t));
    char *elements_index = BUFFER_OFFSET(elements_start * 3 * sizeof(int16_t));
    for (group& group : groups) {
        if (!group.array) {
            group.array.setup(shader, *buffer, vertex_index);
        } else {
            group.array.bind();
        }
        glDrawElements(GL_TRIANGLES, group.elements_length * 3 - 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * 2 * sizeof(uint16_t);
        elements_index += group.elements_length * 3 * sizeof(uint16_t);
    }
}

template <typename Shader>
void FillBucket::drawVertices(Shader& shader) {
    // Draw the entire line
    if (!array) {
        char *vertex_index = BUFFER_OFFSET(vertex_start * 2 * sizeof(int16_t));
        array.setup(shader, *buffer, vertex_index);
    } else {
        array.bind();
    }
    glDrawArrays(GL_LINE_STRIP, 0, length);
}


}

#endif

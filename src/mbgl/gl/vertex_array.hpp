#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/state.hpp>
#include <mbgl/gl/value.hpp>

#include <array>
#include <memory>

namespace mbgl {
namespace gl {

class Context;

class VertexArrayState {
public:
    VertexArrayState(UniqueVertexArray vertexArray_, Context& context)
        : vertexArray(std::move(vertexArray_)),
          bindings(makeBindings(context, std::make_index_sequence<MAX_ATTRIBUTES>())) {
    }

    void setDirty() {
        indexBuffer.setDirty();
        for (auto& binding : bindings) {
            binding.setDirty();
        }
    }

    UniqueVertexArray vertexArray;
    State<value::BindElementBuffer> indexBuffer;

    using AttributeState = State<value::VertexAttribute, Context&, AttributeLocation>;
    std::array<AttributeState, MAX_ATTRIBUTES> bindings;

private:
    template <std::size_t... I>
    std::array<AttributeState, MAX_ATTRIBUTES> makeBindings(Context& context, std::index_sequence<I...>) {
        return {{ AttributeState { context, I }... }};
    }
};

class VertexArrayStateDeleter {
public:
    VertexArrayStateDeleter(bool destroy_)
        : destroy(destroy_) {}

    void operator()(VertexArrayState* ptr) const {
        if (destroy) {
            delete ptr;
        }
    }

private:
    bool destroy;
};

using UniqueVertexArrayState = std::unique_ptr<VertexArrayState, VertexArrayStateDeleter>;

class VertexArray {
public:
    VertexArray(UniqueVertexArrayState state_)
        : state(std::move(state_)) {
    }

    void bind(Context&, BufferID indexBuffer, const AttributeBindingArray&);

private:
    UniqueVertexArrayState state;
};

} // namespace gl
} // namespace mbgl

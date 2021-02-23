#pragma once

#include <mbgl/map/camera.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/size.hpp>

#include <memory>
#include <string>
#include <vector>

class TestOperationSerializer;

class TestWriter final {
public:
    TestWriter();
    ~TestWriter();

    TestWriter& withCameraOptions(const mbgl::CameraOptions&);
    TestWriter& withStyle(const mbgl::style::Style&);
    TestWriter& withInitialSize(const mbgl::Size&);

    bool write(const std::string& dir) const;

private:
    std::string serialize() const;

    std::vector<std::unique_ptr<TestOperationSerializer>> operations;
    std::unique_ptr<TestOperationSerializer> initialSize;
};

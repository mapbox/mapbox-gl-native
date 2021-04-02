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

    auto withCameraOptions(const mbgl::CameraOptions&) -> TestWriter&;
    auto withStyle(const mbgl::style::Style&) -> TestWriter&;
    auto withInitialSize(const mbgl::Size&) -> TestWriter&;

    auto write(const std::string& dir) const -> bool;

private:
    auto serialize() const -> std::string;

    std::vector<std::unique_ptr<TestOperationSerializer>> operations;
    std::unique_ptr<TestOperationSerializer> initialSize;
};

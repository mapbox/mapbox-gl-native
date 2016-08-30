#pragma once

#include <mbgl/util/timer.hpp>

#include <QObject>
#include <QTimer>

namespace mbgl {
namespace util {

class Timer::Impl : public QObject {
    Q_OBJECT

public:
    Impl();

    void start(uint64_t timeout, uint64_t repeat, std::function<void ()> &&);
    void stop();

public slots:
    void timerFired();

private:
    uint64_t repeat;
    std::function<void()> callback;

    QTimer timer;
};

} // namespace util
} // namespace mbgl

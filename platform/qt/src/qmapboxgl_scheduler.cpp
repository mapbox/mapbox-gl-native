#include "qmapboxgl_scheduler.hpp"

#include <mbgl/util/util.hpp>

#include <cassert>

QMapboxGLScheduler::QMapboxGLScheduler()
{
}

QMapboxGLScheduler::~QMapboxGLScheduler()
{
    MBGL_VERIFY_THREAD(tid);
}

void QMapboxGLScheduler::schedule(std::function<void()> function) {
    std::lock_guard<std::mutex> lock(m_taskQueueMutex);
    m_taskQueue.push(std::move(function));

    // Need to force the main thread to wake
    // up this thread and process the events.
    emit needsProcessing();
}

void QMapboxGLScheduler::processEvents()
{
    std::queue<std::function<void()>> taskQueue;
    {
        std::unique_lock<std::mutex> lock(m_taskQueueMutex);
        std::swap(taskQueue, m_taskQueue);
    }

    while (!taskQueue.empty()) {
        auto& function = taskQueue.front();
        if (function) function();
        taskQueue.pop();
    }
}

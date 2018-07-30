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

void QMapboxGLScheduler::schedule(std::weak_ptr<mbgl::Mailbox> mailbox)
{
    std::lock_guard<std::mutex> lock(m_taskQueueMutex);
    m_taskQueue.push(mailbox);

    // Need to force the main thread to wake
    // up this thread and process the events.
    emit needsProcessing();
}

void QMapboxGLScheduler::processEvents()
{
    std::queue<std::weak_ptr<mbgl::Mailbox>> taskQueue;
    {
        std::unique_lock<std::mutex> lock(m_taskQueueMutex);
        std::swap(taskQueue, m_taskQueue);
    }

    while (!taskQueue.empty()) {
        mbgl::Mailbox::maybeReceive(taskQueue.front());
        taskQueue.pop();
    }
}

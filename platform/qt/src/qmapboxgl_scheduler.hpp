#pragma once

#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/util.hpp>

#include <QObject>

#include <memory>
#include <mutex>
#include <queue>

class QMapboxGLScheduler : public QObject, public mbgl::Scheduler
{
    Q_OBJECT

public:
    QMapboxGLScheduler();
    virtual ~QMapboxGLScheduler();

    // mbgl::Scheduler implementation.
    void schedule(std::weak_ptr<mbgl::Mailbox> scheduled) final;

    void processEvents();

signals:
    void needsProcessing();

private:
    MBGL_STORE_THREAD(tid);

    std::mutex m_taskQueueMutex;
    std::queue<std::weak_ptr<mbgl::Mailbox>> m_taskQueue;
};

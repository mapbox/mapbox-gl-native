#include "run_loop_impl.hpp"

#include <mbgl/util/thread_local.hpp>

#include <QCoreApplication>

#include <cassert>
#include <functional>
#include <utility>

namespace {

using namespace mbgl::util;
static ThreadLocal<RunLoop>& current = *new ThreadLocal<RunLoop>;

}

namespace mbgl {
namespace util {

void RunLoop::Impl::onReadEvent(int fd) {
    readPoll[fd].second(fd, Event::Read);
}

void RunLoop::Impl::onWriteEvent(int fd) {
    writePoll[fd].second(fd, Event::Write);
}

RunLoop* RunLoop::Get() {
    return current.get();
}

RunLoop::RunLoop(Type type) : impl(std::make_unique<Impl>()) {
    // XXX: We should probably throw an runtime exception
    // here instead of creating a QCoreApplication which is
    // way too intrusive. This is a hack mostly for the unit
    // tests to work, as you always need a QCoreApplication
    // prior to run a Qt app.
    if (!QCoreApplication::instance()) {
        static const char* argv[] = { "mbgl" };
        static int argc = 1;

        // We need to keep this around because it would otherwise crash
        // on Qt4 due to a bug on QNetworkConfigurationManager when recreating
        // a QCoreApplication: https://bugreports.qt.io/browse/QTBUG-36897
        static auto* app = new QCoreApplication(argc, const_cast<char**>(argv));
        Q_UNUSED(app);
    }

    switch (type) {
    case Type::New:
        impl->loop = std::make_unique<QEventLoop>();
        break;
    case Type::Default:
        // Use QCoreApplication::instance().
        break;
    }

    impl->type = type;

    current.set(this);
    impl->async = std::make_unique<AsyncTask>(std::bind(&RunLoop::process, this));
}

RunLoop::~RunLoop() {
    MBGL_VERIFY_THREAD(tid);

    current.set(nullptr);
}

LOOP_HANDLE RunLoop::getLoopHandle() {
    throw std::runtime_error("Should not be used in Qt.");

    return nullptr;
}

void RunLoop::push(std::shared_ptr<WorkTask> task) {
    withMutex([&] { queue.push(task); });
    impl->async->send();
}

void RunLoop::run() {
    MBGL_VERIFY_THREAD(tid);

    if (impl->type == Type::Default) {
        QCoreApplication::instance()->exec();
    } else {
        impl->loop->exec();
    }
}

void RunLoop::stop() {
    invoke([&] {
        if (impl->type == Type::Default) {
            QCoreApplication::instance()->exit();
        } else {
            impl->loop->exit();
        }
    });
}

void RunLoop::runOnce() {
    MBGL_VERIFY_THREAD(tid);

    if (impl->type == Type::Default) {
        QCoreApplication::instance()->processEvents();
    } else {
        impl->loop->processEvents();
    }
}

void RunLoop::addWatch(int fd, Event event, std::function<void(int, Event)>&& cb) {
    MBGL_VERIFY_THREAD(tid);

    if (event == Event::Read || event == Event::ReadWrite) {
        auto notifier = std::make_unique<QSocketNotifier>(fd, QSocketNotifier::Read);
        QObject::connect(notifier.get(), SIGNAL(activated(int)), impl.get(), SLOT(onReadEvent(int)));
        impl->readPoll[fd] = WatchPair(std::move(notifier), std::move(cb));
    }

    if (event == Event::Write || event == Event::ReadWrite) {
        auto notifier = std::make_unique<QSocketNotifier>(fd, QSocketNotifier::Write);
        QObject::connect(notifier.get(), SIGNAL(activated(int)), impl.get(), SLOT(onWriteEvent(int)));
        impl->writePoll[fd] = WatchPair(std::move(notifier), std::move(cb));
    }
}

void RunLoop::removeWatch(int fd) {
    MBGL_VERIFY_THREAD(tid);

    auto writePollIter = impl->writePoll.find(fd);
    if (writePollIter != impl->writePoll.end()) {
        impl->writePoll.erase(writePollIter);
    }

    auto readPollIter = impl->readPoll.find(fd);
    if (readPollIter != impl->readPoll.end()) {
        impl->readPoll.erase(readPollIter);
    }
}

}
}

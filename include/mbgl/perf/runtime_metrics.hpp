#pragma once

#define MBGL_COLLECT_RUNTIME_METRICS

#ifndef MBGL_COLLECT_RUNTIME_METRICS
#define MBGL_TRACE_RENDERER_BEGIN(range, ...)
#define MBGL_TRACE_RENDERER_END(range, ...)
#define MBGL_TRACE_RENDERER_BEGINRECORD(range, ...)
#define MBGL_TRACE_RENDERER_ENDRECORD(range, ...)
#define MBGL_IF_TRACING(x)
#else
#include <mutex>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/logging.hpp>
#include <vector>

#define MBGL_IF_TRACING(x) x

//#define MBGL_TRACER_USE_MUTEX
#ifndef MBGL_TRACER_USE_MUTEX
#define LOCK_RENDERER(t)
#define UNLOCK_RENDERER(t)
#define RENDERER_LOCKER()
#else
#define LOCK_RENDERER(t) t.frame_mutex.lock();
#define UNLOCK_RENDERER(t) t.frame_mutex.unlock();
#define RENDERER_LOCKER() std::unique_lock<std::mutex> locker(frame_mutex);
#endif

// First argument intended to be rangeName
#define RANGE_NAME(X, ...) X
#define MBGL_TRACE_RENDERER_BEGIN(...) { \
    mbgl::util::Tracer &t = mbgl::util::Tracer::get(); \
    LOCK_RENDERER(t); \
    t.frameTrace.RANGE_NAME(__VA_ARGS__).scope = t.currentScope; \
    t.currentScope += 1; \
    t.frameTrace.RANGE_NAME(__VA_ARGS__).begin = mbgl::util::Tracer::nanoseconds(); \
    UNLOCK_RENDERER(t); \
                                              }
#define MBGL_TRACE_RENDERER_END(...) { \
    mbgl::util::Tracer &t = mbgl::util::Tracer::get(); \
    LOCK_RENDERER(t); \
    t.frameTrace.RANGE_NAME(__VA_ARGS__).end = mbgl::util::Tracer::nanoseconds(); \
    t.currentScope -= 1; \
    t.frameTrace.RANGE_NAME(__VA_ARGS__).totalCalls += 1; \
    t.frameTrace.RANGE_NAME(__VA_ARGS__).totalMs += t.frameTrace.RANGE_NAME(__VA_ARGS__).end - t.frameTrace.RANGE_NAME(__VA_ARGS__).begin; \
    UNLOCK_RENDERER(t); \
                                            }
#define MBGL_TRACE_RENDERER_BEGINRECORD(...) { \
    mbgl::util::Tracer &t = mbgl::util::Tracer::get(); \
    LOCK_RENDERER(t); \
    t.frameTraceReset(t.frameTrace); \
    t.frameTrace.RANGE_NAME(__VA_ARGS__).scope = t.currentScope; \
    t.currentScope += 1; \
    t.frameTrace.RANGE_NAME(__VA_ARGS__).begin = mbgl::util::Tracer::nanoseconds(); \
    UNLOCK_RENDERER(t); \
                                                }
#define MBGL_TRACE_RENDERER_ENDRECORD(...) { \
    mbgl::util::Tracer &t = mbgl::util::Tracer::get(); \
    LOCK_RENDERER(t); \
    t.frameTrace.RANGE_NAME(__VA_ARGS__).end = mbgl::util::Tracer::nanoseconds(); \
    t.currentScope -= 1; \
    t.frameTrace.RANGE_NAME(__VA_ARGS__).totalCalls += 1; \
    t.frameTrace.RANGE_NAME(__VA_ARGS__).totalMs += t.frameTrace.RANGE_NAME(__VA_ARGS__).end - t.frameTrace.RANGE_NAME(__VA_ARGS__).begin; \
    if (t.collectData) t.frameList.push_back(t.frameTrace); \
    UNLOCK_RENDERER(t); }

#define MBGL_TRACE_APPSTART(...) { mbgl::util::Tracer::get().appStart = mbgl::util::Tracer::nanoseconds(); }

#define NS2MS(x) ((x) * 0.000001)

namespace mbgl {
namespace util {

struct TraceInterval {
    unsigned char scope = 0;    // overwritten by each range
    uint64_t begin = 0u;        // overwritten by each range
    uint64_t end = 0u;          // overwritten by each range
    uint64_t totalMs = 0u;
    uint64_t totalCalls = 0u;
};

#define STRUCT_BEGIN(name) struct name {
#define STRUCT_END() };
#define STRUCT_MEMBER_UINT64_RANGE(name, store_range, store_counter)   TraceInterval name;
#include <mbgl/perf/trace_data.in>
#undef STRUCT_BEGIN
#undef STRUCT_END
#undef STRUCT_MEMBER_UINT64_RANGE

// Instrumentation-less tracing implementation, possibly requiring mutexing
class Tracer
{
public:
    static uint64_t nanoseconds() {
        return Clock::now().time_since_epoch().count();
    }
    static Tracer& get() {
        static Tracer i; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return i;
    }
private:
    Tracer() {}

public:
    Tracer(Tracer const&)               = delete;
    void operator=(Tracer const&)       = delete;
    bool collectData = false;


    std::mutex frame_mutex;
    std::vector<mbgl::util::FrameTrace> frameList;
    mbgl::util::FrameTrace frameTrace;
    unsigned int currentScope = 0u;
    unsigned int appStart = 0u;

/* frameTraceReset */
#define STRUCT_BEGIN(name) static void frameTraceReset(name &data) { data = name();  // clear it by assigning to a new struct
#define STRUCT_END() };
#define STRUCT_MEMBER_UINT64_RANGE(name, store_range, store_counter)
#include <mbgl/perf/trace_data.in>
#undef STRUCT_BEGIN
#undef STRUCT_END
#undef STRUCT_MEMBER_UINT64_RANGE

/* framesToJson */
#define STRUCT_BEGIN(name) \
static std::string frameToJson(const name &data) { \
    std::string res; \
    res += "{\n";
// The following assumes that one of { store_range, store_counter } is true.
#define STRUCT_MEMBER_UINT64_RANGE(name, store_range, store_counter) \
res += std::string("\"") + std::string(#name) + std::string("\" : { "); \
if (store_range) { \
    res += std::string("\"scope\" : ") + std::to_string(data.name.scope) + std::string(" , ") \
    + std::string("\"begin\" : ") + std::to_string(data.name.begin) + std::string(" , ") \
    + std::string("\"end\" : ") + std::to_string(data.name.end); } \
if (store_counter) { if (store_range) res += std::string(","); \
    res += std::string("\"totalMs\" : ") + std::to_string(data.name.totalMs) + std::string(" , ") \
    + std::string("\"totalCalls\" : ") + std::to_string(data.name.totalCalls); \
} \
res += std::string(" },");
#define STRUCT_END() \
    res.pop_back(); res += "}"; \
    return res; \
};
#include <mbgl/perf/trace_data.in>
#undef STRUCT_BEGIN
#undef STRUCT_END
#undef STRUCT_MEMBER_UINT64_RANGE

    inline bool isCollectingData() const {
        return collectData;
    }
    inline void setCollectData(bool enabled) {
        collectData = enabled;
    }

    inline std::string framesJson(bool steal = false) const {
        LOCK_RENDERER(*this);
        std::vector<mbgl::util::FrameTrace> frameList_;
        if (steal)
            frameList_ = std::move(frameList);
        else
            frameList_ = frameList;
        UNLOCK_RENDERER(*this);

        std::string res;
        res += "[\n";
        for (const auto &f: frameList)
            res += frameToJson(f) + std::string(",");
        res.pop_back();
        res += "]\n";
        return res;
    }

    inline std::string dumpAll(bool steal = false) const {
        std::string res;
        res += "{\n";
        // meta
        res += "\"global\": {\n";

        res += "}\n";

        //  frames
        res += ", \"frames\": ";
        res += framesJson(steal);

        res += "}\n";
        return res;
    }

    inline void clearFrames() {
        RENDERER_LOCKER();
        frameList.clear();
    }
};
} // namespace util
} // namespace mbgl
#endif

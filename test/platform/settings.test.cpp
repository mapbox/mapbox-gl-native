#include <gtest/gtest.h>
#include <mbgl/platform/settings.hpp>

using namespace mbgl;
using namespace mbgl::platform;

TEST(Settings, SetAndGet) {
    using Value = mapbox::base::Value;
    using Object = mapbox::base::ValueObject;

    auto& settings = Settings::getInstance();
    auto value = settings.get(EXPERIMENTAL_THREAD_PRIORITY_WORKER);
    EXPECT_TRUE(value.is<mapbox::base::NullValue>());

    Value lowPrioValue{19.0};
    settings.set(EXPERIMENTAL_THREAD_PRIORITY_WORKER, lowPrioValue);
    auto threadPriority = settings.get(EXPERIMENTAL_THREAD_PRIORITY_WORKER);
    EXPECT_EQ(lowPrioValue, threadPriority);
    EXPECT_EQ(lowPrioValue, settings.get("mapbox_thread_priority_worker"));

    Value networkPrioValue{-20.0};
    Value databasePrioValue{-10.0};
    Object multipleValues{{EXPERIMENTAL_THREAD_PRIORITY_DATABASE, databasePrioValue},
                          {EXPERIMENTAL_THREAD_PRIORITY_NETWORK, networkPrioValue}};
    settings.set(std::move(multipleValues));

    std::vector<std::string> keys{EXPERIMENTAL_THREAD_PRIORITY_WORKER,
                                  EXPERIMENTAL_THREAD_PRIORITY_DATABASE,
                                  EXPERIMENTAL_THREAD_PRIORITY_NETWORK,
                                  EXPERIMENTAL_THREAD_PRIORITY_FILE};

    auto object = settings.get(keys);

    Object values{{EXPERIMENTAL_THREAD_PRIORITY_WORKER, lowPrioValue},
                  {EXPERIMENTAL_THREAD_PRIORITY_DATABASE, databasePrioValue},
                  {EXPERIMENTAL_THREAD_PRIORITY_NETWORK, networkPrioValue},
                  {EXPERIMENTAL_THREAD_PRIORITY_FILE, Value{}}};

    EXPECT_EQ(values, object);

    values = Object{{EXPERIMENTAL_THREAD_PRIORITY_WORKER, Value{}},
                    {EXPERIMENTAL_THREAD_PRIORITY_DATABASE, Value{}},
                    {EXPERIMENTAL_THREAD_PRIORITY_NETWORK, Value{}},
                    {EXPERIMENTAL_THREAD_PRIORITY_FILE, Value{}}};

    settings.set(values);
    EXPECT_EQ(values, settings.get(keys));
}

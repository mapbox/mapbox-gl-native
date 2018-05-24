
/**
 MGL_CHECK_IF_PENDING_TEST_SHOULD_RUN()

 Include this macro at the top of a "pending" test - one that would normally fail,
 but you want to include is as a compile-time test or as a reminder.

 This will output the included warning in the test log, which `xcpretty` will detect.
 This allows such pending tests to be identified in CI logs.

 If you provide `MAPBOX_RUN_PENDING_TESTS` as an environment variable ALL pending
 tests will run.

 In addition, it's a good idea to append `PENDING` to the test method name, as this
 will also be picked up by xcpretty. PENDING tests will be distinguished by a `⧖`
 and `[PENDING]`
 */
#define MGL_CHECK_IF_PENDING_TEST_SHOULD_RUN() \
    /* By default, skip pending tests. Otherwise check environment for MAPBOX_RUN_PENDING_TESTS */ \
    { \
        NSString *runPendingTests = [[NSProcessInfo processInfo] environment][@"MAPBOX_RUN_PENDING_TESTS"]; \
        if (![runPendingTests boolValue]) { \
            /* The following warning will be picked up by xcpretty */ \
            printf("warning: '%s' is a pending test - skipping\n", __PRETTY_FUNCTION__); \
            return; \
        } \
    }

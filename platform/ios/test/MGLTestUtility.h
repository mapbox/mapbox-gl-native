#define MGL_CHECK_PENDING_TEST() \
    /* By default, skip pending tests. Otherwise check environment for MAPBOX_RUN_PENDING_TESTS */ \
    { \
        NSString *runPendingTests = [[NSProcessInfo processInfo] environment][@"MAPBOX_RUN_PENDING_TESTS"]; \
        if (![runPendingTests boolValue]) { \
            /* The following warning will be picked up by xcpretty */ \
            printf("warning: '%s' is a pending test - skipping\n", __PRETTY_FUNCTION__); \
            return; \
        } \
    }

#define MGL_PENDING_TEST(name) \
    /* Appending `PENDING` to the method name allows xcpretty to mark the test as pending */ \
    - (void)name ## PENDING { MGL_CHECK_PENDING_TEST()

#define MGL_ENABLED_TEST(name) \
    - (void)name {

#define MGL_DISABLED_TEST(name) \
    - (void)DISABLED ## name {



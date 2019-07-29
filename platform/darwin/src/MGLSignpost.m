@import Foundation;
#include "MGLSignpost.h"

os_log_t MGLDefaultSignpostLog = NULL;
os_signpost_id_t MGLDefaultSignpost = OS_SIGNPOST_ID_INVALID;

void destroyDefaultSignpostVaribles(void) __attribute__((destructor));

void destroyDefaultSignpostVaribles() {
    MGLDefaultSignpostLog = NULL;
    MGLDefaultSignpost = OS_SIGNPOST_ID_INVALID;
}

void createDefaultSignpostVaribles(void) __attribute__((constructor));

void createDefaultSignpostVaribles() {
    MGLDefaultSignpostLog = MGLSignpostLogCreate("MGLSignposts");
    
    if (__builtin_available(iOS 12.0, macOS 10.14, *)) {
        MGLDefaultSignpost = os_signpost_id_generate(MGLDefaultSignpostLog);
    }
}

os_log_t MGLSignpostLogCreate(const char* name) {
    if (name && [NSUserDefaults.standardUserDefaults boolForKey:@"MGLSignpostsEnabled"]) {
        return os_log_create("com.mapbox.mapbox", name);
    }
    else {
        return OS_LOG_DISABLED;
    }
}


@import Foundation;
#include "MGLSignpost.h"

os_log_t MGLDefaultSignpostLog = NULL;
os_signpost_id_t MGLDefaultSignpostId = OS_SIGNPOST_ID_INVALID;

void createDefaultSignpost(void) __attribute__((constructor));
void destroyDefaultSignpost(void) __attribute__((destructor));

void destroyDefaultSignpost() {
    MGLDefaultSignpostLog = NULL;
    MGLDefaultSignpostId = OS_SIGNPOST_ID_INVALID;
}

void createDefaultSignpost() {
    MGLDefaultSignpostLog = MGLSignpostLogCreate("MGLSignposts");
    
    if (MGLDefaultSignpostLog) {
        if (__builtin_available(iOS 12.0, macOS 10.14, *)) {
            MGLDefaultSignpostId = os_signpost_id_generate(MGLDefaultSignpostLog);
        }
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

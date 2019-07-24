@import Foundation;
#include "MGLSignpost.h"

os_log_t MGLDefaultSignpostLog = NULL;

void createDefaultSignpostLog(void) __attribute__((constructor));
void destroyDefaultSignpostLog(void) __attribute__((destructor));

void destroyDefaultSignpostLog() {
    MGLDefaultSignpostLog = NULL;
}

void createDefaultSignpost() {
    MGLDefaultSignpostLog = MGLSignpostLogCreate("MGLSignposts");
}

os_log_t MGLSignpostLogCreate(const char* name) {
    if (name && [NSUserDefaults.standardUserDefaults boolForKey:@"MGLSignpostsEnabled"]) {
        return os_log_create("com.mapbox.mapbox", name);
    }
    else {
        return OS_LOG_DISABLED;
    }
}


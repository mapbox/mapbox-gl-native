#pragma once

#define EXPORT_API __attribute__ ((visibility ("default")))

enum GfxDeviceRenderer {
    kGfxRendererOpenGL = 0,
};

extern "C" void EXPORT_API UnitySetGraphicsDevice(void* device, int deviceType, int eventType);
extern "C" void EXPORT_API UnityRenderEvent(int eventID);

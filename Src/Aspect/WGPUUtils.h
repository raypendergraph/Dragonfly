#ifndef ASPECT_WEBGPU_H
#define ASPECT_WEBGPU_H

#include <webgpu/webgpu.h>

const char *queueWorkDoneStatusGetString(WGPUQueueWorkDoneStatus status);

const char *deviceLostReasonGetString(WGPUDeviceLostReason reason);

void adapterPrintFeatures(WGPUAdapter adapter);

#endif

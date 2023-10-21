//
// Created by Ray Pendergraph on 10/10/23.
//

#ifndef LEARNING_ASPECT_H
#define LEARNING_ASPECT_H

#include <webgpu/webgpu.h>

typedef struct GLFWwindow GLFWWwindow;

const char *
aspectFeatureNameGetString(WGPUFeatureName featureName);

const char *
aspectErrorTypeGetString(WGPUErrorType type);

const char *
aspectQueueWorkDoneStatusGetString(WGPUQueueWorkDoneStatus status);

const char *
aspectDeviceLostReasonGetString(WGPUDeviceLostReason reason);

void
aspectAdapterPrintFeatures(WGPUAdapter adapter);

WGPUSurface
aspectNewSurfaceForGLFW(WGPUInstance instance, GLFWwindow *window);

WGPUShaderModule
aspectNewWGSLShaderModule(WGPUDevice device, const char *shaderSource, const char *label);

#endif //LEARNING_ASPECT_H

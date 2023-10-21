#include <webgpu/webgpu.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#import <QuartzCore/CAMetalLayer.h>

WGPUSurface newMetalSurface(WGPUInstance instance, GLFWwindow *window)
{
    id metalLayer = NULL;
    NSWindow *nsWindow = glfwGetCocoaWindow(window);
    [nsWindow.contentView setWantsLayer:YES];
    [nsWindow.contentView setNeedsDisplay:YES];
    metalLayer = [CAMetalLayer layer];
    [nsWindow.contentView setLayer:metalLayer];
    [nsWindow makeKeyAndOrderFront:nil];
    return wgpuInstanceCreateSurface(
       instance,
       &(WGPUSurfaceDescriptor) {
          .nextInChain =
          (const WGPUChainedStruct *) &(WGPUSurfaceDescriptorFromMetalLayer) {
             .chain =
             (WGPUChainedStruct) {
                .sType = WGPUSType_SurfaceDescriptorFromMetalLayer},
             .layer = metalLayer}});
}
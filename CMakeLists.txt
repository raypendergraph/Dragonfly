cmake_minimum_required(VERSION 3.26)

project(learning)

set(CMAKE_C_STANDARD 11)
set(DAWN_FETCH_DEPENDENCIES 1)

add_definitions(-DGLFW_EXPOSE_NATIVE_COCOA)
include_directories(
        /usr/local/include
        ~/thirdparty/usr/local/include
        ~/thirdparty/usr/local/
        ~/thirdparty/dawn/include
        ~/thirdparty/dawn/out/Debug/gen/include
        External/HandmadeMath)

link_directories(
        /usr/local/lib
        ~/thirdparty/usr/local/lib
        ~/thirdparty/dawn/out/Debug/src/dawn/common/
        ~/thirdparty/dawn/out/Debug/src/dawn/platform/
        ~/thirdparty/dawn/out/Debug/src/dawn/native/
        ~/thirdparty/dawn/out/Debug/src/tint/
        ~/thirdparty/dawn/out/Debug/third_party/abseil/absl/strings
        ~/thirdparty/dawn/out/Debug/third_party/abseil/absl/base
        ~/thirdparty/dawn/out/Debug/third_party/abseil/absl/numeric
)

add_executable(learning
        Src/main.c
        Src/Aspect/Aspect.h
        Src/Aspect/MetalAdapter.m
        Src/Aspect/Scene.c
        Src/Aspect/Renderer.c
        Src/Aspect/Camera.c
        Src/Aspect/Material.c
        Src/Aspect/Internal.h
        Src/Aspect/Types.h
        Src/Aspect/AspectGLFW.c
        Src/Aspect/RenderContext.c
        Src/Aspect/Scene.h
        Src/Aspect/Renderer.h
        Src/Aspect/PlatformSpecifics.c
        Src/Aspect/PlatformSpecifics.h
        Src/Aspect/Material.h
        Src/Aspect/RenderContext.h
        Src/Aspect/WGPUUtils.c
)

add_subdirectory(Modules/Platform)
add_subdirectory(External/dawn)
add_subdirectory(External/libxml2)

target_link_libraries(learning PRIVATE
        Platform
        spng
        xml2
        "-framework IOSurface"
        "-framework Quartz"
        "-framework Cocoa"
        "-framework Foundation"
        "-framework IOKit"
        "-framework Metal"
        absl_int128
        absl_raw_logging_internal
        absl_str_format_internal
        absl_strings
        dawn_common
        dawn_native
        dawn_platform
        glfw3
        stdc++
        tint_api
        tint_api_common
        tint_api_options
        tint_cmd_common
        tint_lang_core
        tint_lang_core_constant
        tint_lang_core_intrinsic
        tint_lang_core_ir
        tint_lang_core_ir_transform
        tint_lang_core_type
        tint_lang_hlsl_writer_common
        tint_lang_msl_validate
        tint_lang_msl_writer
        tint_lang_msl_writer_ast_printer
        tint_lang_msl_writer_ast_raise
        tint_lang_msl_writer_common
        tint_lang_msl_writer_printer
        tint_lang_msl_writer_raise
        tint_lang_spirv
        tint_lang_spirv_intrinsic
        tint_lang_spirv_reader_common
        tint_lang_spirv_type
        tint_lang_wgsl
        tint_lang_wgsl_ast
        tint_lang_wgsl_ast_transform
        tint_lang_wgsl_helpers
        tint_lang_wgsl_inspector
        tint_lang_wgsl_intrinsic
        tint_lang_wgsl_ir
        tint_lang_wgsl_program
        tint_lang_wgsl_reader
        tint_lang_wgsl_reader_lower
        tint_lang_wgsl_reader_parser
        tint_lang_wgsl_reader_program_to_ir
        tint_lang_wgsl_resolver
        tint_lang_wgsl_sem
        tint_lang_wgsl_writer
        tint_lang_wgsl_writer_ast_printer
        tint_lang_wgsl_writer_ir_to_program
        tint_lang_wgsl_writer_raise
        tint_lang_wgsl_writer_syntax_tree_printer
        tint_utils_cli
        tint_utils_debug
        tint_utils_diagnostic
        tint_utils_file
        tint_utils_generator
        tint_utils_ice
        tint_utils_id
        tint_utils_macros
        tint_utils_math
        tint_utils_memory
        tint_utils_rtti
        tint_utils_result
        tint_utils_socket
        tint_utils_strconv
        tint_utils_symbol
        tint_utils_text
        tint_utils_traits
        webgpu_dawn
)

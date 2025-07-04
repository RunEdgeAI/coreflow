"""
BUILD file for build defs for third party deps
"""

# OpenCV
cc_import(
    name="opencv-highgui",
    shared_library=select({
        "@platforms//os:macos": "opencv/lib/macos/libopencv_highgui.4.11.0.dylib",
        "//conditions:default": None,
    }),
)

cc_import(
    name="opencv-photo",
    shared_library=select({
        "@platforms//os:macos": "opencv/lib/macos/libopencv_photo.4.11.0.dylib",
        "//conditions:default": None,
    }),
)

cc_import(
    name="opencv-video",
    shared_library=select({
        "@platforms//os:macos": "opencv/lib/macos/libopencv_video.4.11.0.dylib",
        "//conditions:default": None,
    }),
)

cc_import(
    name="opencv-videoio",
    shared_library=select({
        "@platforms//os:macos": "opencv/lib/macos/libopencv_videoio.4.11.0.dylib",
        "//conditions:default": None,
    }),
)

cc_library(
    name = "opencv-prebuilt",
    hdrs = glob(["opencv/include/**/*.hpp"]),
    includes = ["opencv/include"],
    deps = [
        ":opencv-highgui",
        ":opencv-photo",
        ":opencv-video",
        ":opencv-videoio",
    ],
    visibility = ["//visibility:public"],
)

# ONNX Runtime
cc_import(
    name="onnxruntime-providers",
    shared_library=select({
        "@platforms//os:linux": "onnxruntime/lib/libonnxruntime_providers_shared.so",
        "//conditions:default": None,
    }),
)

cc_import(
    name="onnxruntime-prebuilt",
    shared_library=select({
        "@platforms//os:macos": "onnxruntime/lib/libonnxruntime.1.22.0.dylib",
        "@platforms//os:linux": "onnxruntime/lib/libonnxruntime.so.1",
        "//conditions:default": None,
    }),
    deps=[":onnxruntime-providers"],
)

cc_library(
    name="onnxruntime",
    hdrs=glob(["onnxruntime/include/**/*.h"]),
    includes=["onnxruntime/include"],
    deps=[":onnxruntime-prebuilt"],
    visibility=["//visibility:public"],
)

# TensorFlow Lite
cc_import(
    name = "tflite",
    shared_library = select({
        "@platforms//os:linux": "tflite-hdrs/libtensorflowlite.so",
        "@platforms//os:macos": "tflite-hdrs/libtensorflowlite.dylib",
        "//conditions:default": None,
    }),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "tflite-hdrs",
    hdrs = glob(["tflite-hdrs/**/*.h"]),
    includes = ["tflite-hdrs"],
    deps = ["@flatbuffers"],
    visibility = ["//visibility:public"],
)

# Executorch
cc_library(
    name = "executorch",
    srcs = select({
        "@platforms//os:linux": [
            "executorch/lib/linux/libbundled_program.a",
            "executorch/lib/linux/libetdump.a",
            "executorch/lib/linux/libexecutorch.a",
            "executorch/lib/linux/libexecutorch_core.a",
            "executorch/lib/linux/libextension_module_static.a",
            "executorch/lib/linux/libextension_module.so",
            "executorch/lib/linux/libextension_data_loader.a",
            "executorch/lib/linux/libextension_threadpool.a",
            "executorch/lib/linux/libextension_tensor.a",
            "executorch/lib/linux/libextension_flat_tensor.a",
            "executorch/lib/linux/libgflags_nothreads.a",
            "executorch/lib/linux/libflatccrt.a",
            "executorch/lib/linux/libcpublas.a",
            "executorch/lib/linux/libeigen_blas.a",
            "executorch/lib/linux/liboptimized_native_cpu_ops_lib.a",
            "executorch/lib/linux/libportable_kernels.a",
            "executorch/lib/linux/liboptimized_kernels.a",
            "executorch/lib/linux/liboptimized_portable_kernels.a",
            "executorch/lib/linux/libportable_ops_lib.a",
            "executorch/lib/linux/liboptimized_ops_lib.a",
            "executorch/lib/linux/libxnnpack_backend.a",
            "executorch/lib/linux/libXNNPACK.a",
            "executorch/lib/linux/libmicrokernels-prod.a",
            "executorch/lib/linux/libpthreadpool.a",
            "executorch/lib/linux/libcpuinfo.a",
        ],
        "@platforms//os:macos": [
            "executorch/lib/macos/libbundled_program.a",
            "executorch/lib/macos/libetdump.a",
            "executorch/lib/macos/libexecutorch.a",
            "executorch/lib/macos/libexecutorch_core.a",
            "executorch/lib/macos/libextension_module_static.a",
            "executorch/lib/macos/libextension_data_loader.a",
            "executorch/lib/macos/libextension_threadpool.a",
            "executorch/lib/macos/libextension_tensor.a",
            "executorch/lib/macos/libflatccrt.a",
            "executorch/lib/macos/libcpublas.a",
            "executorch/lib/macos/libeigen_blas.a",
            "executorch/lib/macos/liboptimized_native_cpu_ops_lib.a",
            "executorch/lib/macos/libportable_kernels.a",
            "executorch/lib/macos/liboptimized_kernels.a",
            "executorch/lib/macos/liboptimized_portable_kernels.a",
            "executorch/lib/macos/libportable_ops_lib.a",
            "executorch/lib/macos/liboptimized_ops_lib.a",
            "executorch/lib/macos/libxnnpack_backend.a",
            "executorch/lib/macos/libXNNPACK.a",
            "executorch/lib/macos/libmicrokernels-prod.a",
            "executorch/lib/macos/libpthreadpool.a",
            "executorch/lib/macos/libcpuinfo.a",
        ],
        "//conditions:default": None,
    }),
    hdrs = glob(["executorch/**/*.h"]),
    defines = ["C10_USING_CUSTOM_GENERATED_MACROS"],
    includes = [
        ".",
        "executorch/runtime/core/portable_type/c10",
    ],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"],
)

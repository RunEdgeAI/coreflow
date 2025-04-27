"""
BUILD file for build defs for third party deps
"""

cc_import(
    name = "tflite",
    shared_library = select({
        "@platforms//os:linux": "tflite-hdrs/libtensorflowlite.so",
        "@platforms//os:macos": "tflite-hdrs/libtensorflowlite.dylib",
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
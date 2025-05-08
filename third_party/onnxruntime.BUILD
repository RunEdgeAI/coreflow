load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")
load("@cuda_env//:cuda_home.bzl", "CUDA_HOME")
load("@cuda_env//:cudnn_home.bzl", "CUDNN_HOME")

filegroup(
    name = "all_srcs",
    srcs = glob(["**"], exclude = ["**/foo.onnx"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "hdrs",
    hdrs = glob(["include/onnxruntime/**/*.h"]),
    strip_include_prefix = "include/onnxruntime",
    visibility = ["//visibility:public"],
)

config_setting(
    name = "with_cuda",
    define_values = { "use_cuda": "" }
)

__POSTFIX = """
mkdir -p $INSTALLDIR/lib
mkdir -p $INSTALLDIR/lib/_deps
mkdir -p $INSTALLDIR/lib/_deps/onnx-build
mkdir -p $INSTALLDIR/lib/_deps/re2-build
mkdir -p $INSTALLDIR/lib/_deps/abseil_cpp-build
mkdir -p $INSTALLDIR/lib/_deps/protobuf-build
mkdir -p $INSTALLDIR/lib/_deps/abseil_cpp-build/absl/container
mkdir -p $INSTALLDIR/lib/_deps/abseil_cpp-build/absl/hash
mkdir -p $INSTALLDIR/lib/_deps/pytorch_cpuinfo-build
mkdir -p $INSTALLDIR/lib/_deps/pytorch_cpuinfo-build/deps
mkdir -p $INSTALLDIR/lib/_deps/pytorch_clog-build
mkdir -p $INSTALLDIR/lib/_deps/google_nsync-build
mkdir -p $INSTALLDIR/lib/_deps/opencv-build
mkdir -p $INSTALLDIR/lib/_deps/opencv-build/lib
mkdir -p $INSTALLDIR/lib/_deps/opencv-build/3rdparty
mkdir -p $INSTALLDIR/lib/_deps/opencv-build/3rdparty/lib
cp $BUILD_TMPDIR/_deps/onnx-build/libonnx.a $INSTALLDIR/lib/_deps/onnx-build
cp $BUILD_TMPDIR/_deps/onnx-build/libonnx_proto.a $INSTALLDIR/lib/_deps/onnx-build
cp $BUILD_TMPDIR/_deps/re2-build/libre2.a $INSTALLDIR/lib/_deps/re2-build
cp -r $BUILD_TMPDIR/_deps/abseil_cpp-build/. $INSTALLDIR/lib/_deps/abseil_cpp-build
cp $BUILD_TMPDIR/_deps/google_nsync-build/libnsync_cpp.a $INSTALLDIR/lib/_deps/google_nsync-build
cp $BUILD_TMPDIR/_deps/pytorch_clog-build/libclog.a $INSTALLDIR/lib/_deps/pytorch_clog-build
cp $BUILD_TMPDIR/_deps/pytorch_cpuinfo-build/libcpuinfo.a $INSTALLDIR/lib/_deps/pytorch_cpuinfo-build
cp $BUILD_TMPDIR/_deps/protobuf-build/libprotobuf.a $INSTALLDIR/lib/_deps/protobuf-build
# cp $BUILD_TMPDIR/_deps/opencv-build/lib/libopencv_imgcodecs.a $INSTALLDIR/lib/_deps/opencv-build/lib
# cp $BUILD_TMPDIR/_deps/opencv-build/lib/libopencv_imgproc.a $INSTALLDIR/lib/_deps/opencv-build/lib
# cp $BUILD_TMPDIR/_deps/opencv-build/lib/libopencv_core.a $INSTALLDIR/lib/_deps/opencv-build/lib
# cp $BUILD_TMPDIR/_deps/opencv-build/3rdparty/lib/liblibjpeg-turbo.a $INSTALLDIR/lib/_deps/opencv-build/3rdparty/lib
# cp $BUILD_TMPDIR/_deps/opencv-build/3rdparty/lib/liblibpng.a $INSTALLDIR/lib/_deps/opencv-build/3rdparty/lib
# cp $BUILD_TMPDIR/lib/libnoexcep_operators.a $INSTALLDIR/lib
# cp $BUILD_TMPDIR/lib/libocos_operators.a $INSTALLDIR/lib
# cp $BUILD_TMPDIR/lib/libortcustomops.a $INSTALLDIR/lib
"""

__POSTFIX_WITH_CUDA = __POSTFIX + """
cp $BUILD_TMPDIR/libonnxruntime_providers_shared.so $INSTALLDIR/../../../
cp $BUILD_TMPDIR/libonnxruntime_providers_cuda.so $INSTALLDIR/../../../
"""

__ONNXRUNTIME_WITHOUT_CUDA = {
'onnxruntime_RUN_ONNX_TESTS':'OFF',
'onnxruntime_GENERATE_TEST_REPORTS':'OFF',
'onnxruntime_USE_MIMALLOC':'OFF',
'onnxruntime_ENABLE_PYTHON':'OFF',
'onnxruntime_BUILD_CSHARP':'OFF',
'onnxruntime_BUILD_JAVA':'OFF',
'onnxruntime_BUILD_NODEJS':'OFF',
'onnxruntime_BUILD_OBJC':'OFF',
'onnxruntime_BUILD_SHARED_LIB':'OFF',
'onnxruntime_BUILD_APPLE_FRAMEWORK':'OFF',
'onnxruntime_USE_DNNL':'OFF',
'onnxruntime_USE_NNAPI_BUILTIN':'OFF',
'onnxruntime_USE_RKNPU':'OFF',
'onnxruntime_USE_LLVM':'OFF',
'onnxruntime_ENABLE_MICROSOFT_INTERNAL':'OFF',
'onnxruntime_USE_VITISAI':'OFF',
'onnxruntime_USE_TENSORRT':'OFF',
'onnxruntime_SKIP_AND_PERFORM_FILTERED_TENSORRT_TESTS':'OFF',
'onnxruntime_USE_TENSORRT_BUILTIN_PARSER':'OFF',
'onnxruntime_TENSORRT_PLACEHOLDER_BUILDER':'OFF',
'onnxruntime_USE_TVM':'OFF',
'onnxruntime_TVM_CUDA_RUNTIME':'OFF',
'onnxruntime_TVM_USE_HASH':'OFF',
'onnxruntime_USE_MIGRAPHX':'OFF',
'onnxruntime_CROSS_COMPILING':'OFF',
'onnxruntime_DISABLE_CONTRIB_OPS':'OFF',
'onnxruntime_DISABLE_ML_OPS':'OFF',
'onnxruntime_DISABLE_RTTI':'OFF',
'onnxruntime_DISABLE_EXCEPTIONS':'OFF',
'onnxruntime_MINIMAL_BUILD':'OFF',
'onnxruntime_EXTENDED_MINIMAL_BUILD':'OFF',
'onnxruntime_MINIMAL_BUILD_CUSTOM_OPS':'OFF',
'onnxruntime_REDUCED_OPS_BUILD':'OFF',
'onnxruntime_ENABLE_LANGUAGE_INTEROP_OPS':'OFF',
'onnxruntime_USE_DML':'OFF',
'onnxruntime_USE_WINML':'OFF',
'onnxruntime_BUILD_MS_EXPERIMENTAL_OPS':'OFF',
'onnxruntime_USE_TELEMETRY':'OFF',
'onnxruntime_ENABLE_LTO':'OFF',
'onnxruntime_USE_ACL':'OFF',
'onnxruntime_USE_ACL_1902':'OFF',
'onnxruntime_USE_ACL_1905':'OFF',
'onnxruntime_USE_ACL_1908':'OFF',
'onnxruntime_USE_ACL_2002':'OFF',
'onnxruntime_USE_ARMNN':'OFF',
'onnxruntime_ARMNN_RELU_USE_CPU':'ON',
'onnxruntime_ARMNN_BN_USE_CPU':'ON',
'onnxruntime_ENABLE_NVTX_PROFILE':'OFF',
'onnxruntime_ENABLE_TRAINING':'OFF',
'onnxruntime_ENABLE_TRAINING_OPS':'OFF',
'onnxruntime_ENABLE_TRAINING_APIS':'OFF',
'onnxruntime_ENABLE_CPU_FP16_OPS':'OFF',
'onnxruntime_USE_NCCL':'OFF',
'onnxruntime_BUILD_BENCHMARKS':'OFF',
'onnxruntime_USE_ROCM':'OFF',
'Onnxruntime_GCOV_COVERAGE':'OFF',
'onnxruntime_USE_MPI':'OFF',
'onnxruntime_ENABLE_MEMORY_PROFILE':'OFF',
'onnxruntime_ENABLE_CUDA_LINE_NUMBER_INFO':'OFF',
'onnxruntime_BUILD_WEBASSEMBLY':'OFF',
'onnxruntime_BUILD_WEBASSEMBLY_STATIC_LIB':'OFF',
'onnxruntime_ENABLE_WEBASSEMBLY_EXCEPTION_CATCHING':'ON',
'onnxruntime_ENABLE_WEBASSEMBLY_API_EXCEPTION_CATCHING':'OFF',
'onnxruntime_ENABLE_WEBASSEMBLY_EXCEPTION_THROWING':'ON',
'onnxruntime_ENABLE_WEBASSEMBLY_THREADS':'OFF',
'onnxruntime_ENABLE_WEBASSEMBLY_DEBUG_INFO':'OFF',
'onnxruntime_ENABLE_WEBASSEMBLY_PROFILING':'OFF',
'onnxruntime_ENABLE_EAGER_MODE':'OFF',
'onnxruntime_ENABLE_LAZY_TENSOR':'OFF',
'onnxruntime_ENABLE_EXTERNAL_CUSTOM_OP_SCHEMAS':'OFF',
'onnxruntime_ENABLE_CUDA_PROFILING':'OFF',
'onnxruntime_ENABLE_ROCM_PROFILING':'OFF',
'onnxruntime_USE_XNNPACK':'OFF',
'onnxruntime_USE_CANN':'OFF',
'CMAKE_TLS_VERIFY':'ON',
'FETCHCONTENT_QUIET':'OFF',
'onnxruntime_PYBIND_EXPORT_OPSCHEMA':'OFF',
'onnxruntime_ENABLE_MEMLEAK_CHECKER':'OFF',
'CMAKE_BUILD_TYPE':'Release',
'onnxruntime_USE_EXTENSIONS': 'OFF',
'onnxruntime_EXTENSIONS_PATH': '$EXT_BUILD_ROOT/external/onnx_runtime/cmake/external/onnxruntime-extensions',
'onnxruntime_USE_FULL_PROTOBUF': 'ON',
'Protobuf_USE_STATIC_LIBS': 'ON',
'OCOS_ENABLE_BLINGFIRE': 'OFF'
}

__ONNXRUNTIME_WITH_CUDA = {
'onnxruntime_RUN_ONNX_TESTS':'OFF',
'onnxruntime_GENERATE_TEST_REPORTS':'OFF',
'onnxruntime_USE_MIMALLOC':'OFF',
'onnxruntime_ENABLE_PYTHON':'OFF',
'onnxruntime_BUILD_CSHARP':'OFF',
'onnxruntime_BUILD_JAVA':'OFF',
'onnxruntime_BUILD_NODEJS':'OFF',
'onnxruntime_BUILD_OBJC':'OFF',
'onnxruntime_BUILD_SHARED_LIB':'OFF',
'onnxruntime_BUILD_APPLE_FRAMEWORK':'OFF',
'onnxruntime_USE_DNNL':'OFF',
'onnxruntime_USE_NNAPI_BUILTIN':'OFF',
'onnxruntime_USE_RKNPU':'OFF',
'onnxruntime_USE_LLVM':'OFF',
'onnxruntime_ENABLE_MICROSOFT_INTERNAL':'OFF',
'onnxruntime_USE_VITISAI':'OFF',
'onnxruntime_USE_TENSORRT':'OFF',
'onnxruntime_SKIP_AND_PERFORM_FILTERED_TENSORRT_TESTS':'OFF',
'onnxruntime_USE_TENSORRT_BUILTIN_PARSER':'OFF',
'onnxruntime_TENSORRT_PLACEHOLDER_BUILDER':'OFF',
'onnxruntime_USE_TVM':'OFF',
'onnxruntime_TVM_CUDA_RUNTIME':'OFF',
'onnxruntime_TVM_USE_HASH':'OFF',
'onnxruntime_USE_MIGRAPHX':'OFF',
'onnxruntime_CROSS_COMPILING':'OFF',
'onnxruntime_DISABLE_CONTRIB_OPS':'OFF',
'onnxruntime_DISABLE_ML_OPS':'OFF',
'onnxruntime_DISABLE_RTTI':'OFF',
'onnxruntime_DISABLE_EXCEPTIONS':'OFF',
'onnxruntime_MINIMAL_BUILD':'OFF',
'onnxruntime_EXTENDED_MINIMAL_BUILD':'OFF',
'onnxruntime_MINIMAL_BUILD_CUSTOM_OPS':'OFF',
'onnxruntime_REDUCED_OPS_BUILD':'OFF',
'onnxruntime_ENABLE_LANGUAGE_INTEROP_OPS':'OFF',
'onnxruntime_USE_DML':'OFF',
'onnxruntime_USE_WINML':'OFF',
'onnxruntime_BUILD_MS_EXPERIMENTAL_OPS':'OFF',
'onnxruntime_USE_TELEMETRY':'OFF',
'onnxruntime_ENABLE_LTO':'OFF',
'onnxruntime_USE_ACL':'OFF',
'onnxruntime_USE_ACL_1902':'OFF',
'onnxruntime_USE_ACL_1905':'OFF',
'onnxruntime_USE_ACL_1908':'OFF',
'onnxruntime_USE_ACL_2002':'OFF',
'onnxruntime_USE_ARMNN':'OFF',
'onnxruntime_ARMNN_RELU_USE_CPU':'ON',
'onnxruntime_ARMNN_BN_USE_CPU':'ON',
'onnxruntime_ENABLE_NVTX_PROFILE':'OFF',
'onnxruntime_ENABLE_TRAINING':'OFF',
'onnxruntime_ENABLE_TRAINING_OPS':'OFF',
'onnxruntime_ENABLE_TRAINING_APIS':'OFF',
'onnxruntime_ENABLE_CPU_FP16_OPS':'OFF',
'onnxruntime_USE_NCCL':'OFF',
'onnxruntime_BUILD_BENCHMARKS':'OFF',
'onnxruntime_USE_ROCM':'OFF',
'Onnxruntime_GCOV_COVERAGE':'OFF',
'onnxruntime_USE_MPI':'OFF',
'onnxruntime_ENABLE_MEMORY_PROFILE':'OFF',
'onnxruntime_ENABLE_CUDA_LINE_NUMBER_INFO':'OFF',
'onnxruntime_BUILD_WEBASSEMBLY':'OFF',
'onnxruntime_BUILD_WEBASSEMBLY_STATIC_LIB':'OFF',
'onnxruntime_ENABLE_WEBASSEMBLY_EXCEPTION_CATCHING':'ON',
'onnxruntime_ENABLE_WEBASSEMBLY_API_EXCEPTION_CATCHING':'OFF',
'onnxruntime_ENABLE_WEBASSEMBLY_EXCEPTION_THROWING':'ON',
'onnxruntime_ENABLE_WEBASSEMBLY_THREADS':'OFF',
'onnxruntime_ENABLE_WEBASSEMBLY_DEBUG_INFO':'OFF',
'onnxruntime_ENABLE_WEBASSEMBLY_PROFILING':'OFF',
'onnxruntime_ENABLE_EAGER_MODE':'OFF',
'onnxruntime_ENABLE_LAZY_TENSOR':'OFF',
'onnxruntime_ENABLE_EXTERNAL_CUSTOM_OP_SCHEMAS':'OFF',
'onnxruntime_ENABLE_CUDA_PROFILING':'OFF',
'onnxruntime_ENABLE_ROCM_PROFILING':'OFF',
'onnxruntime_USE_XNNPACK':'OFF',
'onnxruntime_USE_CANN':'OFF',
'CMAKE_TLS_VERIFY':'ON',
'FETCHCONTENT_QUIET':'OFF',
'onnxruntime_PYBIND_EXPORT_OPSCHEMA':'OFF',
'onnxruntime_ENABLE_MEMLEAK_CHECKER':'OFF',
'CMAKE_BUILD_TYPE':'Release',
'onnxruntime_USE_CUDA':'ON',
'onnxruntime_USE_CUDNN':'ON',
'onnxruntime_USE_EXTENSIONS': 'OFF',
'onnxruntime_EXTENSIONS_PATH': '$EXT_BUILD_ROOT/external/onnx_runtime/cmake/external/onnxruntime-extensions',
'onnxruntime_CUDA_HOME': CUDA_HOME,
'onnxruntime_CUDNN_HOME': CUDNN_HOME,
'CMAKE_CUDA_COMPILER': CUDA_HOME + "/bin/nvcc",
'onnxruntime_USE_FULL_PROTOBUF': 'ON',
'Protobuf_USE_STATIC_LIBS': 'ON',
'OCOS_ENABLE_BLINGFIRE': 'OFF'
}

cmake(
    name = "onnxruntime",
    lib_source = "//:all_srcs",
    cache_entries = select({
        ":with_cuda":   __ONNXRUNTIME_WITH_CUDA,
        "//conditions:default": __ONNXRUNTIME_WITHOUT_CUDA
    }),
    working_directory="cmake",
    generate_args=["-GNinja"],
    build_args= [
        "--config Release",
    ],
    tags=[
        "requires-network",
        # "no-sandbox"
    ],
    features=[
        "-default_compile_flags",
        "-fno-canonical-system-headers",
        "-Wno-builtin-macro-redefined",
        "-Wno-deprecated-declarations"
    ],
    out_static_libs=[
        "libonnxruntime_common.a",
        "libonnxruntime_flatbuffers.a",
        "libonnxruntime_framework.a",
        "libonnxruntime_graph.a",
        "libonnxruntime_lora.a",
        "libonnxruntime_mlas.a",
        "libonnxruntime_optimizer.a",
        "libonnxruntime_session.a",
        "libonnxruntime_providers.a",
        "libonnxruntime_util.a",
        # "libortcustomops.a",
        # "libocos_operators.a",
        # "libnoexcep_operators.a",
        "_deps/onnx-build/libonnx.a",
        "_deps/onnx-build/libonnx_proto.a",
        "_deps/re2-build/libre2.a",
        "_deps/abseil_cpp-build/absl/strings/libabsl_cord.a",
        "_deps/abseil_cpp-build/absl/strings/libabsl_string_view.a",
        "_deps/abseil_cpp-build/absl/strings/libabsl_cord_internal.a",
        "_deps/abseil_cpp-build/absl/strings/libabsl_cordz_info.a",
        "_deps/abseil_cpp-build/absl/strings/libabsl_strings.a",
        "_deps/abseil_cpp-build/absl/strings/libabsl_str_format_internal.a",
        "_deps/abseil_cpp-build/absl/strings/libabsl_strings_internal.a",
        "_deps/abseil_cpp-build/absl/strings/libabsl_cordz_handle.a",
        "_deps/abseil_cpp-build/absl/strings/libabsl_cordz_functions.a",
        "_deps/abseil_cpp-build/absl/types/libabsl_bad_optional_access.a",
        "_deps/abseil_cpp-build/absl/types/libabsl_bad_any_cast_impl.a",
        "_deps/abseil_cpp-build/absl/types/libabsl_bad_variant_access.a",
        "_deps/abseil_cpp-build/absl/flags/libabsl_flags_commandlineflag.a",
        "_deps/abseil_cpp-build/absl/flags/libabsl_flags_usage_internal.a",
        "_deps/abseil_cpp-build/absl/flags/libabsl_flags_private_handle_accessor.a",
        "_deps/abseil_cpp-build/absl/flags/libabsl_flags_usage.a",
        "_deps/abseil_cpp-build/absl/flags/libabsl_flags_program_name.a",
        "_deps/abseil_cpp-build/absl/flags/libabsl_flags_parse.a",
        "_deps/abseil_cpp-build/absl/flags/libabsl_flags_config.a",
        "_deps/abseil_cpp-build/absl/flags/libabsl_flags_commandlineflag_internal.a",
        "_deps/abseil_cpp-build/absl/flags/libabsl_flags_marshalling.a",
        "_deps/abseil_cpp-build/absl/flags/libabsl_flags_reflection.a",
        "_deps/abseil_cpp-build/absl/flags/libabsl_flags_internal.a",
        "_deps/abseil_cpp-build/absl/synchronization/libabsl_kernel_timeout_internal.a",
        "_deps/abseil_cpp-build/absl/synchronization/libabsl_graphcycles_internal.a",
        "_deps/abseil_cpp-build/absl/synchronization/libabsl_synchronization.a",
        "_deps/abseil_cpp-build/absl/hash/libabsl_city.a",
        "_deps/abseil_cpp-build/absl/hash/libabsl_hash.a",
        "_deps/abseil_cpp-build/absl/hash/libabsl_low_level_hash.a",
        "_deps/abseil_cpp-build/absl/debugging/libabsl_failure_signal_handler.a",
        "_deps/abseil_cpp-build/absl/debugging/libabsl_utf8_for_code_point.a",
        "_deps/abseil_cpp-build/absl/debugging/libabsl_decode_rust_punycode.a",
        "_deps/abseil_cpp-build/absl/debugging/libabsl_debugging_internal.a",
        "_deps/abseil_cpp-build/absl/debugging/libabsl_symbolize.a",
        "_deps/abseil_cpp-build/absl/debugging/libabsl_stacktrace.a",
        "_deps/abseil_cpp-build/absl/debugging/libabsl_demangle_rust.a",
        "_deps/abseil_cpp-build/absl/debugging/libabsl_demangle_internal.a",
        "_deps/abseil_cpp-build/absl/debugging/libabsl_examine_stack.a",
        "_deps/abseil_cpp-build/absl/crc/libabsl_crc_cord_state.a",
        "_deps/abseil_cpp-build/absl/crc/libabsl_crc32c.a",
        "_deps/abseil_cpp-build/absl/crc/libabsl_crc_internal.a",
        "_deps/abseil_cpp-build/absl/crc/libabsl_crc_cpu_detect.a",
        "_deps/abseil_cpp-build/absl/time/libabsl_time_zone.a",
        "_deps/abseil_cpp-build/absl/time/libabsl_time.a",
        "_deps/abseil_cpp-build/absl/time/libabsl_civil_time.a",
        "_deps/abseil_cpp-build/absl/container/libabsl_raw_hash_set.a",
        "_deps/abseil_cpp-build/absl/container/libabsl_hashtablez_sampler.a",
        "_deps/abseil_cpp-build/absl/numeric/libabsl_int128.a",
        "_deps/abseil_cpp-build/absl/profiling/libabsl_exponential_biased.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_internal_conditions.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_internal_nullguard.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_internal_log_sink_set.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_internal_format.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_internal_fnmatch.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_internal_globals.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_sink.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_internal_check_op.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_internal_message.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_globals.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_internal_proto.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_log_entry.a",
        "_deps/abseil_cpp-build/absl/log/libabsl_vlog_config_internal.a",
        "_deps/abseil_cpp-build/absl/base/libabsl_spinlock_wait.a",
        "_deps/abseil_cpp-build/absl/base/libabsl_log_severity.a",
        "_deps/abseil_cpp-build/absl/base/libabsl_raw_logging_internal.a",
        "_deps/abseil_cpp-build/absl/base/libabsl_base.a",
        "_deps/abseil_cpp-build/absl/base/libabsl_throw_delegate.a",
        "_deps/abseil_cpp-build/absl/base/libabsl_malloc_internal.a",
        "_deps/abseil_cpp-build/absl/base/libabsl_strerror.a",
        "_deps/google_nsync-build/libnsync_cpp.a",
        "_deps/protobuf-build/libprotobuf.a",
        "_deps/pytorch_cpuinfo-build/libcpuinfo.a",
        "_deps/pytorch_clog-build/libclog.a",
        # "_deps/opencv-build/lib/libopencv_imgcodecs.a",
        # "_deps/opencv-build/lib/libopencv_imgproc.a",
        # "_deps/opencv-build/lib/libopencv_core.a",
        # "_deps/opencv-build/3rdparty/lib/liblibjpeg-turbo.a",
        # "_deps/opencv-build/3rdparty/lib/liblibpng.a"
    ],
    postfix_script= select({
        ":with_cuda": __POSTFIX_WITH_CUDA,
        "//conditions:default": __POSTFIX
    }),
)

cc_library(
    name = "onnxruntime_lib",
    linkopts = select({
        "@platforms//os:linux": ["-static-libstdc++", "-static-libgcc"],
        "//conditions:default": [],
    }),
    deps = [
        ":hdrs",
        ":onnxruntime",
        # "@onnx_runtime_extensions//:operators",
    ],
    includes= ["onnxruntime/include/onnxruntime"],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"]
)
"""
CUDA Check Repo Tool
"""
def cuda_impl(repository_ctx):
    repository_ctx.file("cuda_home.bzl", "CUDA_HOME = \"%s\"" % repository_ctx.os.environ.get("CUDA_HOME", ""))
    repository_ctx.file("cudnn_home.bzl", "CUDNN_HOME = \"%s\"" % repository_ctx.os.environ.get("CUDNN_HOME", ""))
    repository_ctx.file("BUILD", """
exports_files(["cuda_home.bzl", "cudnn_home.bzl"])
visibility = ["//visibility:public"]
""")

cuda_home_repository = repository_rule(
    implementation = cuda_impl,
    environ = ["CUDA_HOME", "CUDNN_HOME"],
)
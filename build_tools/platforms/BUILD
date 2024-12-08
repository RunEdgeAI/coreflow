"""
platforms.bzl
"""

# Define an x86_64 generic platform
platform(
    name = "x86_64_generic",
    constraint_values = [
        "@platforms//cpu:x86_64",
    ],
)

# Define an x86_64 linux platform
platform(
    name = "x86_64",
    parents = [
        ":x86_64_generic"
    ],
    constraint_values = [
        "@platforms//os:linux",
    ],
)

# Define an x86_64 macos platform
platform(
    name = "x86_64_macos",
    parents = [
        ":x86_64_generic"
    ],
    constraint_values = [
        "@platforms//os:macos",
    ],
)

# Define an ARM platform
platform(
    name = "arm",
    constraint_values = [
        "@platforms//cpu:arm",
        "@platforms//os:none",
    ],
)

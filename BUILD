package(default_visibility = ["//visibility:public"])

config_setting(
    name = "dbg",
    values = {
        "compilation_mode": "dbg"
    }
)

COPTS = ["-std=c++17", "-DFRUIT_USES_BOOST=0", "-fdiagnostics-color=always"]

cc_binary(
    name = "cconfd",
    srcs = glob(["cmd/main.cpp"]),
    deps = [
        ":common",
        ":components",
        "@args//:includes",
    ] + select({
        ":dbg": ["@backward//:includes"],
        "//conditions:default": [],
    }),
    linkopts = select({
        ":dbg": ["-ldl", "-ldwarf", "-lelf", "-lz", "-g"],
        "//conditions:default": [],
    }),
    copts = COPTS + select({
        ":dbg": ["-I/usr/include/libdwarf"],
        "//conditions:default": [],
    }),
    defines = select({
        ":dbg": ["DEBUG"],
        "//conditions:default": []
    })
)

cc_library(
    name = "components",
    srcs = glob(["components/*.cpp"]),
    hdrs = glob(["*.h", "components/*.h", "interfaces/*.h"]),
    deps = [
        ":common",
        "@json//:includes",
        "@fruit//:fruit",
        "@date//:includes",
        "@jsonnet//core:libjsonnet"
    ],
    linkstatic = True,
    linkopts = [
        "-lcurl",
        "-lstdc++fs"
    ],
    copts = COPTS,
    defines = select({
        ":dbg": ["DEBUG"],
        "//conditions:default": []
    })
)

cc_library(
    name = "common",
    srcs = glob(["common/*.cpp"]),
    hdrs = glob(["common/*.h"]),
    deps = [
        "@spdlog//:includes"
    ],
    linkstatic = True,
    copts = COPTS,
    defines = select({
        ":dbg": ["DEBUG"],
        "//conditions:default": []
    })
)
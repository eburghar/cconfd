config_setting(
    name = "dbg",
    values = {
        "compilation_mode": "dbg"
    }
)

COPTS = ["-std=c++17", "-DFRUIT_USES_BOOST=0", "-fdiagnostics-color=always"]

cc_test(
    name = "cconfd_test",
    srcs = glob(["cmd/main.cpp", "suites/*Test.cpp", "*.h"]),
    #hdrs = glob(["*.h"]),
    data = ["token", "secret.txt"],
    deps = [
        "//:common",
        ":components",
        "@googletest//:gtest",
        "@args//:includes",
    ] + select({
        ":dbg": ["@backward//:includes"],
        "//conditions:default": [],
    }),
    linkopts = select({
        ":dbg": ["-ldl", "-ldwarf", "-lelf", "-lz", "-g", "-rdynamic"],
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
    hdrs = glob(["components/*.h"]),
    includes = ["."],
    deps = [
        "//:components",
    ],
    linkstatic = True,
    copts = COPTS,
    defines = select({
        ":dbg": ["DEBUG"],
        "//conditions:default": []
    })
)
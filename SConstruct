#!/usr/bin/env python
import os
import sys

from methods import print_error


libname = "OpusGdextension"
projectdir = "project"

localEnv = Environment(tools=["default"], PLATFORM="")

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"api_version": 4.4, "env": env, "customs": customs})

# Add WIN32 define on Windows (required by config.h)
if env["platform"] == "windows":
    env.Append(CPPDEFINES=["WIN32"])

env.Append(CPPDEFINES=[
    "HAVE_CONFIG_H",
])

env.Append(CPPPATH=[
    "src/",
    "#thirdparty/libogg/",
    "#thirdparty/libogg/ogg/",
    "#thirdparty/libopus/",
    "#thirdparty/libopus/opus/",
    "#thirdparty/libopus/src/",
    "#thirdparty/libopus/celt/",
    "#thirdparty/libopus/silk/",
    "#thirdparty/libopus/silk/fixed/",
    "#thirdparty/libopus/silk/float/",
    "#thirdparty/libopusfile/",
    "#thirdparty/libopusfile/opus/",
    "#thirdparty/libopusfile/src/",
])

sources = Glob("src/*.cpp") \
    + Glob("#thirdparty/libogg/*.c") \
    + Glob("#thirdparty/libogg/ogg/*.c") \
    + Glob("#thirdparty/libopus/src/*.c") \
    + Glob("#thirdparty/libopus/celt/*.c") \
    + Glob("#thirdparty/libopus/silk/*.c") \
    + Glob("#thirdparty/libopus/silk/float/*.c") \
    + Glob("#thirdparty/libopusfile/src/*.c")

# Remove /fp:strict flag
if env["CCFLAGS"]:
    if "/fp:strict" in env["CCFLAGS"]:
        env["CCFLAGS"].remove("/fp:strict")
        # Add /fp:precise flag
        env.Append(CCFLAGS=["/fp:precise"])

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("src/doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# .dev doesn't inhibit compatibility, so we don't need to key it.
# .universal just means "compatible with all relevant arches" so we don't need to key it.
suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/addons/{}/{}".format(libname, lib_filename),
    source=sources,
)
ext = env.Substfile(
    "bin/addons/{}/lib/{}.gdextension".format(libname, libname),
    "#misc/cfg.gdextension", SUBST_DICT={"LIBGDEXTENSION": lib_filename},
)

copy = env.Install("{}/addons/OpusGdextension/bin/{}/".format(projectdir, env["platform"]), library)

default_args = [library, ext, copy]
Default(*default_args)

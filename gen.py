#!/usr/bin/env python3

import glob, sys, subprocess, os, platform
sys.dont_write_bytecode = True

shader_format="spv"
match platform.system():
    case "Windows":
        shader_format = "dxil"
    case "Darwin":
        shader_format = "msl"

binary="super-tux-brawl"
libs="sdl3"
release = False
builddir = "objs"
prefix = "/usr"
destdir = ""
outputdir=""

for arg in sys.argv[1:]:
    if "--release" == arg:
        release = True
    elif "--builddir" in arg:
        builddir = arg.split("=")[1]
    elif "--prefix" in arg:
        prefix = arg.split("=")[1]
    elif "--destdir" in arg:
        destdir = arg.split("=")[1]

CFLAGS=os.getenv("CFLAGS", default="") + f' -std=c99 -DSHADER_FORMAT="{shader_format}"'
LDFLAGS=os.getenv("LDFLAGS", default="")
CC=os.getenv("CC", default="gcc")

if release:
    CFLAGS += " -O2 -DDEBUG=0"
    builddir += "/release"
    outputdir = "out-release"
else:
    CFLAGS += " -Wall -g -DDEBUG=1"
    builddir += "/debug"
    outputdir = "out-debug"

LDFLAGS += " " + subprocess.run(f"pkg-config --libs {libs}".split(" "), capture_output=True).stdout.strip().decode()
CFLAGS += " " + subprocess.run(f"pkg-config --cflags {libs}".split(" "), capture_output=True).stdout.strip().decode()

# get files
c_files = glob.glob("src/**/*.c", recursive=True)
all_o_files = " ".join([x.replace("src", builddir).replace(".c", ".o") for x in c_files])
shader_files = glob.glob("src/**/*.glsl", recursive=True)
all_spv_files = " ".join([x.replace("src", outputdir).replace(".glsl", ".spv") for x in shader_files])
all_json_shader_files = " ".join([x.replace("src", outputdir).replace(".glsl", ".json") for x in shader_files])
all_system_shader_files = ""
if shader_format != "spv":
    all_system_shader_files = " ".join([x.replace("src", outputdir).replace(".glsl", f".{shader_format}") for x in shader_files])

def writeln(file, string):
    file.write(string.encode() + b"\n")

# write compile_flags.txt
with open("compile_flags.txt", "wb") as f:
    writeln(f, "\n".join(CFLAGS.strip().split(" ")))

    print("Wrote compile_flags.txt")

# write build.ninja
with open("build.ninja", "wb") as f:
    writeln(f, f"builddir = {builddir}")
    writeln(f, f"cflags = {CFLAGS.strip()}")
    writeln(f, f"libs = {LDFLAGS.strip()}")

    writeln(f, f'rule cc')
    writeln(f, f'  deps = gcc')
    writeln(f, f'  depfile = $out.d')
    writeln(f, f'  description = CC $out')
    writeln(f, f'  command = {CC} -MD -MF $out.d $cflags -c $in -o $out')

    writeln(f, f'rule glslc_vert')
    writeln(f, f'  deps = gcc')
    writeln(f, f'  depfile = $out.d')
    writeln(f, f'  description = GLSLC $out')
    writeln(f, f'  command = glslc -MD -MF $out.d -fshader-stage=vert $in -o $out')

    writeln(f, f'rule glslc_frag')
    writeln(f, f'  deps = gcc')
    writeln(f, f'  depfile = $out.d')
    writeln(f, f'  description = GLSLC $out')
    writeln(f, f'  command = glslc -MD -MF $out.d -fshader-stage=frag $in -o $out')

    writeln(f, f'rule link')
    writeln(f, f'  description = LD $out')
    writeln(f, f'  command = {CC} $libs $in -o $out')

    writeln(f, f'rule shadercross')
    writeln(f, f'  description = SHADERCROSS $out')
    writeln(f, f'  command = shadercross $in -o $out')

    for file in c_files:
        ofile = file.replace("src", builddir).replace(".c", ".o")
        writeln(f, f"build {ofile}: cc {file}")

    for file in shader_files:
        spirv_ofile = file.replace("src", outputdir).replace(".glsl", ".spv")
        system_ofile = file.replace("src", outputdir).replace(".glsl", f".{shader_format}")
        json_ofile = file.replace("src", outputdir).replace(".glsl", f".json")
        stage = file.split(".")[-2]
        writeln(f, f"build {spirv_ofile}: glslc_{stage} {file}")
        if shader_format != "spv":
            writeln(f, f"build {system_ofile}: shadercross {spirv_ofile}")
        writeln(f, f"build {json_ofile}: shadercross {spirv_ofile}")

    writeln(f, f"build {outputdir}/{binary}: link {all_o_files}")
    writeln(f, f"build {binary}: phony {outputdir}/{binary}")
    writeln(f, f"build shaders: phony {all_spv_files} {all_json_shader_files} {all_system_shader_files}")
    writeln(f, f"build all: phony {binary} shaders")
    writeln(f, f"default all")

    print("Wrote build.ninja")

#!/usr/bin/env python3

import glob
import sys
import subprocess
import os
sys.dont_write_bytecode = True

binary="super-tux-brawl"
libs="sdl3"
release = 0
builddir = "objs"
prefix = "/usr"
destdir = ""
outputdir=""

for arg in sys.argv[1:]:
    if "--release" == arg:
        release = 1
    elif "--builddir" in arg:
        builddir = arg.split("=")[1]
    elif "--prefix" in arg:
        prefix = arg.split("=")[1]
    elif "--destdir" in arg:
        destdir = arg.split("=")[1]


CFLAGS=os.getenv("CFLAGS", default="")
LDFLAGS=os.getenv("LDFLAGS", default="")

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

c_files = glob.glob("src/**/*.c", recursive=True)
all_o_files = " ".join([x.replace("src", builddir).replace(".c", ".o") for x in c_files])
shader_files = glob.glob("src/**/*.glsl", recursive=True)
all_spv_files = " ".join([x.replace("src", outputdir).replace(".glsl", ".spv") for x in shader_files])

def write(file, string):
    file.write(string.encode() + b"\n")

with open("compile_flags.txt", "wb") as f:
    write(f, "\n".join(CFLAGS.strip().split(" ")))

with open("build.ninja", "wb") as f:
    write(f, f"builddir = {builddir}")
    write(f, f"cflags = {CFLAGS.strip()}")
    write(f, f"libs = {LDFLAGS.strip()}")

    write(f, 'rule cc')
    write(f, '  deps = gcc')
    write(f, '  depfile = $out.d')
    write(f, '  description = CC $out')
    write(f, '  command = gcc -MD -MF $out.d $cflags -c $in -o $out')

    write(f, 'rule glslc_vert')
    write(f, '  deps = gcc')
    write(f, '  depfile = $out.d')
    write(f, '  description = GLSLC $out')
    write(f, '  command = glslc -MD -MF $out.d -fshader-stage=vert $in -o $out')

    write(f, 'rule glslc_frag')
    write(f, '  deps = gcc')
    write(f, '  depfile = $out.d')
    write(f, '  description = GLSLC $out')
    write(f, '  command = glslc -MD -MF $out.d -fshader-stage=frag $in -o $out')

    write(f, 'rule link')
    write(f, '  description = LD $out')
    write(f, '  command = gcc $libs $in -o $out')

    for file in c_files:
        ofile = file.replace("src", builddir).replace(".c", ".o")
        write(f, f"build {ofile}: cc {file}")

    for file in shader_files:
        ofile = file.replace("src", outputdir).replace(".glsl", ".spv")
        stage = file.split(".")[-2]
        write(f, f"build {ofile}: glslc_{stage} {file}")

    write(f, f"build {outputdir}/{binary}: link {all_o_files}")
    write(f, f"build {binary}: phony {outputdir}/{binary}")
    write(f, f"build shaders: phony {all_spv_files}")
    write(f, f"build all: phony {binary} shaders")
    write(f, f"default all")

print("Wrote build.ninja file")

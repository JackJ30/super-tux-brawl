#!/usr/bin/env python3

import glob, sys, subprocess, os, platform
sys.dont_write_bytecode = True

binary="super-tux-brawl"
libs="sdl3"
release = False
builddir = "objs"
prefix = "/usr"
destdir = ""
outputdir = ""

# get user config
for arg in sys.argv[1:]:
    if "--release" == arg:
        release = True
    elif "--builddir" in arg:
        builddir = arg.split("=")[1]
    elif "--prefix" in arg:
        prefix = arg.split("=")[1]
    elif "--destdir" in arg:
        destdir = arg.split("=")[1]

CFLAGS=os.getenv("CFLAGS", default="") + f' -std=c99 -Isrc/deps/'
LDFLAGS=os.getenv("LDFLAGS", default="")
CC=os.getenv("CC", default="gcc")

# add our config
shader_format="spv"
match platform.system():
    case "Windows":
        shader_format = "dxil"
        CFLAGS += " -DSHADER_FORMAT_SPV -DSHADER_FORMAT_DXIL"
    case "Darwin":
        shader_format = "msl"
        CFLAGS += " -DSHADER_FORMAT_SPV -DSHADER_FORMAT_MSL"
    case _:
        CFLAGS += " -DSHADER_FORMAT_SPV"

if release:
    CFLAGS += " -O2 -DDEBUG=0"
    builddir += "/release"
    outputdir = "out-release"
else:
    CFLAGS += " -Wall -g -DDEBUG=1"
    builddir += "/debug"
    outputdir = "out-debug"

# pkg-config
LDFLAGS += " " + subprocess.run(f"pkg-config --libs {libs}".split(" "), capture_output=True).stdout.strip().decode()
CFLAGS += " " + subprocess.run(f"pkg-config --cflags {libs}".split(" "), capture_output=True).stdout.strip().decode()

# get files
c_files = glob.glob("src/**/*.c", recursive=True)
shader_files = glob.glob("src/**/*.glsl", recursive=True)

def writeln(file, string):
    file.write(string.encode() + b"\n")

# write compile_flags.txt
with open("compile_flags.txt", "wb") as f:
    writeln(f, "\n".join(CFLAGS.strip().split(" ")))

    print("Wrote compile_flags.txt")

# write build.ninja
with open("build.ninja", "wb") as f:
    # config
    writeln(f, f"cflags = {CFLAGS.strip()}")
    writeln(f, f"libs = {LDFLAGS.strip()}")

    # rules
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

    # build c files
    all_o_files = []
    for file in c_files:
        ofile = file.replace("src", builddir).replace(".c", ".o")
        all_o_files.append(ofile)
        writeln(f, f"build {ofile}: cc {file}")

    # compile shader files
    all_shader_outs = []
    for file in shader_files:
        # spirv
        spirv_ofile = file.replace("src", outputdir).replace(".glsl", ".spv")
        all_shader_outs.append(spirv_ofile)
        stage = file.split(".")[-2]
        writeln(f, f"build {spirv_ofile}: glslc_{stage} {file}")

        # json
        json_ofile = file.replace("src", outputdir).replace(".glsl", f".json")
        all_shader_outs.append(json_ofile)
        writeln(f, f"build {json_ofile}: shadercross {spirv_ofile}")

        # system format
        if shader_format != "spv":
            system_ofile = file.replace("src", outputdir).replace(".glsl", f".{shader_format}")
            all_shader_outs.append(system_ofile)
            writeln(f, f"build {system_ofile}: shadercross {spirv_ofile}")

    # targets
    writeln(f, f"build {outputdir}/{binary}: link {" ".join(all_o_files)}")
    writeln(f, f"build {binary}: phony {outputdir}/{binary}")
    writeln(f, f"build shaders: phony {" ".join(all_shader_outs)}")
    writeln(f, f"build all: phony {binary} shaders")
    writeln(f, f"default all")

    print("Wrote build.ninja")

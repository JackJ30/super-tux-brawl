#!/usr/bin/env python3

import glob, sys, subprocess, os, platform
sys.dont_write_bytecode = True

binary="super-tux-brawl"
libs="sdl3"
builddir = "objs"
prefix = "/usr"
destdir = ""
outputdir = ""

# get user config
for arg in sys.argv[1:]:
    if "--builddir" in arg:
        builddir = arg.split("=")[1]
    elif "--prefix" in arg:
        prefix = arg.split("=")[1]
    elif "--destdir" in arg:
        destdir = arg.split("=")[1]

# get files
c_files = glob.glob("src/**/*.c", recursive=True)
shader_files = glob.glob("src/**/*.glsl", recursive=True)

CFLAGS=os.getenv("CFLAGS", default="") + f' -std=c11 -Isrc/ -Isrc/deps/'
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

# pkg-config
LDFLAGS += " " + subprocess.run(f"pkg-config --libs {libs}".split(" "), capture_output=True).stdout.strip().decode()
CFLAGS += " " + subprocess.run(f"pkg-config --cflags {libs}".split(" "), capture_output=True).stdout.strip().decode()

def writeln(file, string):
    file.write(string.encode() + b"\n")

# write compile_flags.txt
with open("compile_flags.txt", "wb") as f:
    writeln(f, "\n".join((CFLAGS + " -Wall -g -DDEBUG=1").strip().split(" ")))
    print("Wrote compile_flags.txt")

# write build.ninja
with open("build.ninja", "wb") as f:

    # shared variables
    writeln(f, f"libs = {LDFLAGS.strip()}")
    writeln(f, f"builddir = {builddir}")

    # shared rules
    writeln(f, "");
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

    # generate rules for each configuration
    configurations = [
        {
            "type":  "release",
            "cflags":  CFLAGS + " -O2 -DDEBUG=0",
            "builddir": builddir + "/release",
            "outputdir": outputdir + "out-release"
        },
        {
            "type":  "debug",
            "cflags":  CFLAGS + " -Wall -g -DDEBUG=1",
            "builddir": builddir + "/debug",
            "outputdir": outputdir + "out-debug"
        },
    ]
    for conf in configurations:
        builddir = conf['builddir']
        outputdir = conf['outputdir']
        CFLAGS = conf['cflags']
        t = conf['type']

        # configuration variables
        writeln(f, f"\n# ============ {t} ============\n");
        writeln(f, f"builddir-{t} = {builddir}")
        writeln(f, f"cflags-{t} = {CFLAGS.strip()}")

        #configuration cc rule
        writeln(f, "");
        writeln(f, f'rule cc-{t}')
        writeln(f, f'  deps = gcc')
        writeln(f, f'  depfile = $out.d')
        writeln(f, f'  description = CC $out')
        writeln(f, f'  command = {CC} -MD -MF $out.d $cflags-{t} -c $in -o $out')

        # build c files
        all_o_files = []
        writeln(f, "");
        for file in c_files:
            ofile = file.replace("src", builddir).replace(".c", ".o")
            all_o_files.append(ofile)
            writeln(f, f"build {ofile}: cc-{t} {file}")

        # build shader files and cross compile
        all_shader_outs = []
        writeln(f, "");
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

        # configuration targets
        writeln(f, "");
        writeln(f, f"build {outputdir}/{binary}: link {" ".join(all_o_files)}")
        writeln(f, f"build shaders-{t}: phony {" ".join(all_shader_outs)}")
        writeln(f, f"build {t}: phony {outputdir}/{binary} shaders-{t}")

    # global targets
    writeln(f, f"\n# ============ global ============\n");
    writeln(f, f"build all: phony release debug")
    writeln(f, f"default debug")
    print("Wrote build.ninja")

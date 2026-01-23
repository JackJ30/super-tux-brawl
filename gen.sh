#!/bin/bash

bin="super-tux-brawl"
libs="sdl3"
release=0
builddir="objs"
installing=0
PREFIX="/usr"
DESTDIR=""

run_install() {
    echo "Installing ${bin}"
    install -m 755 ./${bin} ${DESTDIR}${PREFIX}/bin
    exit 0
}

while [ "$1" ]; do
	case "$1" in
        --release|-r) release=1 ;;
        --builddir*) builddir="$(echo $1 | cut -d= -f2)" ;;
        --prefix*) PREFIX="$(echo $1 | cut -d= -f2)" ;;
        --destdir*) DESTDIR="$(echo $1 | cut -d= -f2)" ;;
        --install) installing=1 ;;
		-*) exit 1 ;;
	esac
	shift
done

[ $installing -eq 1 ] && run_install

CFLAGS=${CFLAGS:-""}
LDFLAGS=${LDFLAGS:-""}

if [ $release -eq 1 ]; then
    CFLAGS="$CFLAGS -O2 -DDEBUG=0"
    builddir="$builddir/release"
    outputdir="out-release"
else
    builddir="$builddir/debug"
    outputdir="out-debug"
    CFLAGS="$CFLAGS -Wall -g -DDEBUG=1"
fi

write() {
    echo "$@" >> build.ninja
}

CFLAGS=$(echo "$CFLAGS $(pkg-config --cflags $libs 2>/dev/null)")
LDFLAGS=$(echo "$LDFLAGS $(pkg-config --libs $libs 2>/dev/null)")
echo $CFLAGS | tr ' ' '\n' > compile_flags.txt

rm -f build.ninja

write "builddir = $builddir"
write "cflags = $(echo $CFLAGS)"
write "libs = $(echo $LDFLAGS)"

write 'rule cc'
write '  deps = gcc'
write '  depfile = $out.d'
write '  description = CC $out'
write '  command = gcc -MD -MF $out.d $cflags -c $in -o $out'

write 'rule glslc_vert'
write '  deps = gcc'
write '  depfile = $out.d'
write '  description = GLSLC $out'
write '  command = glslc -MD -MF $out.d -fshader-stage=vert $in -o $out'

write 'rule glslc_frag'
write '  deps = gcc'
write '  depfile = $out.d'
write '  description = GLSLC $out'
write '  command = glslc -MD -MF $out.d -fshader-stage=frag $in -o $out'

write 'rule link'
write '  description = LD $out'
write '  command = gcc $libs $in -o $out'

write 'rule run_install'
write '  description = Installing Project'
write "  command = ./gen.sh --install --prefix=${PREFIX} --destdir=${DESTDIR}"

c_files="$(find src -name '*.c')"
c_sedstr=$(printf 's,^src/\(.*\)\.c$,%s/\\1.o,g' ${builddir})
o_files=$(echo "$c_files" | sed ${c_sedstr})

shader_files="$(find src/shaders -name '*.glsl')"
glsl_sedstr=$(printf 's,^src/\(.*\)\.glsl$,%s/\\1.spv,g' ${outputdir})
spirv_files=$(echo "$shader_files" | sed ${glsl_sedstr})

for f in $c_files; do
    of=$(echo $f | sed ${c_sedstr})
    write "build $of: cc $f"
done

for f in $shader_files; do
    of=$(echo $f | sed ${glsl_sedstr})
    write "build $of: glslc_$(echo "$f" | sed 's/.*\.\(.*\)\.glsl/\1/') $f"
done

write "build install: run_install"
write "build $outputdir/$bin: link $(echo $o_files)"
write "build $bin: phony $outputdir/$bin"
write "build shaders: phony $(echo $spirv_files)"
write "build all: phony $bin shaders"
write "default all"

echo "Wrote build.ninja file"

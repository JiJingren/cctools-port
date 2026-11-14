#!/usr/bin/env bash
# Build cctools-port (ld64 and friends) natively on Windows with MSYS2 UCRT64.
#
# Requirements (MSYS2 UCRT64):
#   pacman -S --needed git make autoconf automake libtool pkg-config python \
#     diffutils patch texinfo bison flex binutils \
#     mingw-w64-ucrt-x86_64-clang mingw-w64-ucrt-x86_64-lld \
#     mingw-w64-ucrt-x86_64-libdispatch \
#     mingw-w64-ucrt-x86_64-libblocksruntime-swift \
#     mingw-w64-ucrt-x86_64-mman-win32 mingw-w64-ucrt-x86_64-dlfcn
#
# Usage (from the repository root):
#   ./windows-ucrt64.sh [configure args...]
#
# Environment overrides:
#   PREFIX         install prefix (default /tmp/cc-install)
#   TARGET         target triple (default aarch64-apple-darwin11)
#   LIBTAPI_DIR    path to a libtapi install (enables --with-libtapi)
#   JOBS           parallel build jobs (default nproc)

set -e

HERE=$(cd "$(dirname "$0")" && pwd)
cd "$HERE/cctools"

PREFIX=${PREFIX:-/tmp/cc-install}
TARGET=${TARGET:-aarch64-apple-darwin11}
JOBS=${JOBS:-$(nproc)}

CC=${CC:-/ucrt64/bin/clang}
CXX=${CXX:-/ucrt64/bin/clang++}

CFLAGS="-O2 -Wno-error -Wno-padded -Wno-padded-bitfield -include $PWD/include/foreign/minwg_compat.h"
CXXFLAGS="$CFLAGS"
LDFLAGS="-L/ucrt64/lib -Wl,--allow-multiple-definition -ldl -ldispatch -lws2_32 -lmman"

EXTRA=()
if [ -n "$LIBTAPI_DIR" ]; then
	EXTRA+=("--with-libtapi=$LIBTAPI_DIR")
fi

# MSYS2's libdispatch ships a generic_win_base.h whose mode_t/pthread_attr_t
# typedefs conflict with the mingw-w64 CRT / winpthreads headers.
if [ -f /ucrt64/include/os/generic_win_base.h ]; then
	sed -i 's/^typedef int mode_t;$//' /ucrt64/include/os/generic_win_base.h
	sed -i 's/^typedef void pthread_attr_t;$//' /ucrt64/include/os/generic_win_base.h
fi

./configure \
	--prefix="$PREFIX" \
	--target="$TARGET" \
	CC="$CC" CXX="$CXX" \
	CFLAGS="$CFLAGS" CXXFLAGS="$CXXFLAGS" LDFLAGS="$LDFLAGS" \
	"${EXTRA[@]}" "$@"

# ld64 links against libBlocksRuntime/-ldispatch; make sure the dispatch and
# socket/mman libraries are linked in as well (configure only records the
# former in BLOCKS_RUNTIME_LIB).
for mk in $(grep -rl '^BLOCKS_RUNTIME_LIB = ' --include=Makefile .); do
	sed -i 's|^BLOCKS_RUNTIME_LIB = \(.*\)$|BLOCKS_RUNTIME_LIB = \1 -ldispatch -lws2_32 -lmman|' "$mk"
done

make -j"$JOBS"
make install

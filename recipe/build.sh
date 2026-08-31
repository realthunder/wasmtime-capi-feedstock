#!/bin/bash
set -euxo pipefail

# The release artifact unpacks to include/ + lib/, with a `min/` variant
# beside it -- a smaller build with features compiled out. We ship the full
# one: the min build is not what an embedder with an unknown guest wants.
mkdir -p "$PREFIX/include" "$PREFIX/lib"
cp -R include/. "$PREFIX/include/"

SOVER="${PKG_VERSION%%.*}"

if [[ "$(uname)" == "Darwin" ]]; then
    cp lib/libwasmtime.dylib "$PREFIX/lib/libwasmtime.$SOVER.dylib"
    # conda relocates by rpath; an install name that is anything else
    # follows the build machine into the package.
    install_name_tool -id "@rpath/libwasmtime.$SOVER.dylib" \
        "$PREFIX/lib/libwasmtime.$SOVER.dylib"
    ln -s "libwasmtime.$SOVER.dylib" "$PREFIX/lib/libwasmtime.dylib"
else
    cp lib/libwasmtime.so "$PREFIX/lib/libwasmtime.so.$SOVER"
    # The upstream library carries NO SONAME. Linking it then records
    # whatever path the linker was handed as the DT_NEEDED entry, so the
    # consumer goes looking for the build machine's directory at run time.
    # Stamping the SONAME here is the whole reason this is a package and
    # not a downloaded tarball.
    patchelf --set-soname "libwasmtime.so.$SOVER" "$PREFIX/lib/libwasmtime.so.$SOVER"
    ln -s "libwasmtime.so.$SOVER" "$PREFIX/lib/libwasmtime.so"
fi

# The static library is deliberately NOT packaged; see README.md.

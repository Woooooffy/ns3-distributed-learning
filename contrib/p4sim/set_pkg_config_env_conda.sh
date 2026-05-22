#!/bin/bash

# Ensure a conda environment is activated
if [ -z "$CONDA_PREFIX" ]; then
    echo "Error: no conda environment activated."
    exit 1
fi

PREFIX="$CONDA_PREFIX"

mkdir -p "$PREFIX/lib/pkgconfig"

cat > "$PREFIX/lib/pkgconfig/bm.pc" <<EOF
prefix=$PREFIX
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include/bm

Name: BMv2
Description: Behavioral Model
Version: 1.15.0
Libs: -L\${libdir} -lbmall
Cflags: -I\${includedir}
EOF

cat > "$PREFIX/lib/pkgconfig/boost_system.pc" <<EOF
prefix=$PREFIX
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: boost_system
Description: Boost System
Version: 1.83.0
Libs: -L\${libdir} -lboost_system
Cflags: -I\${includedir}
EOF

cat > "$PREFIX/lib/pkgconfig/simple_switch.pc" <<EOF
prefix=$PREFIX
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include/bm

Name: simple_switch
Description: Behavioral Model Target Simple Switch
Version: 1.15.0
Libs: -L\${libdir} -lsimpleswitch_thrift
Cflags: -I\${includedir}
EOF

echo "pkg-config files installed to:"
echo "$PREFIX/lib/pkgconfig"

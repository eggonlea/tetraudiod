#!/bin/sh

# Compiles fftw3 for Android
# Make sure you have NDK_DIR defined in .bashrc or .bash_profile

#NDK Version r9c  http://dl.google.com/android/ndk/android-ndk-r9c-linux-x86_64.tar.bz2
NDK_ROOT="/srv/Android/Sdk/ndk-bundle"

export PATH="$NDK_ROOT/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/bin/:$PATH"
export SYS_ROOT="$NDK_ROOT/platforms/android-24/arch-arm64/"
export CC="aarch64-linux-android-gcc --sysroot=$SYS_ROOT"
export LD="aarch64-linux-android-ld"
export AR="aarch64-linux-android-ar"
export RANLIB="aarch64-linux-android-ranlib"
export STRIP="aarch64-linux-android-strip"
#export CFLAGS="-mfpu=neon -mfloat-abi=softfp"

./configure \
	--host=aarch64-linux-android \
	LIBS="-lc -lgcc" \
	NEON_CFLAGS=-D__ARM_NEON__ \
	--enable-single \
	--enable-float \
	--enable-neon

# --enable-threads
# --with-combined-threads

make

exit 0

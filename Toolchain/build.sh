#!/usr/bin/env bash


set -eo pipefail


DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TARGET=i686-elf
export PREFIX="$DIR/$TARGET-cross"


echo DIR is "$DIR"
echo TARGET is "$TARGET"
echo PREFIX is "$PREFIX"

MAKE="make"
MD5SUM="md5sum"

MAKEJOBS=$(nproc)

BINUTILS_VERSION="2.37"
BINUTILS_MD5SUM="1e55743d73c100b7a0d67ffb32398cdb"
BINUTILS_NAME="binutils-$BINUTILS_VERSION"
BINUTILS_PKG="${BINUTILS_NAME}.tar.gz"
BINUTILS_BASE_URL="http://ftp.gnu.org/gnu/binutils"

GCC_VERSION="11.2.0"
GCC_MD5SUM="dc6886bd44bb49e2d3d662aed9729278"
GCC_NAME="gcc-$GCC_VERSION"
GCC_PKG="${GCC_NAME}.tar.gz"
GCC_BASE_URL="http://ftp.gnu.org/gnu/gcc"

echo

mkdir -p "$DIR/Tarballs"

pushd "$DIR/Tarballs"

	echo "Checking if we need to download Binutils"
	md5=""
	if [ -e "$BINUTILS_PKG" ]; then
		md5="$($MD5SUM $BINUTILS_PKG | cut -f1 -d' ')"
	fi
	if [ "$md5" != ${BINUTILS_MD5SUM} ] ; then
		echo "We do need to. Downloading Binutils..."
		rm -f $BINUTILS_PKG
		curl -LO "$BINUTILS_BASE_URL/$BINUTILS_PKG"
	else
		echo "We do not need to download Binutils"
	fi

	echo

	echo "Checking if we need to download GCC"
	md5=""
	if [ -e "$GCC_PKG" ]; then
		md5="$($MD5SUM $GCC_PKG | cut -f1 -d' ')"
	fi
	if [ "$md5" != ${GCC_MD5SUM} ] ; then
		echo "We do need to. Downloading GCC..."
		rm -f $GCC_PKG
		curl -LO "$GCC_BASE_URL/$GCC_NAME/$GCC_PKG"
	else
		echo "We do not need to download GCC"
	fi

	echo

	rm -rf ${GCC_NAME} ${BINUTILS_NAME}

	echo "Extracting Binutils..."
	tar -xzf ${BINUTILS_PKG}

	echo "Extracting GCC..."
	tar -xzf ${GCC_PKG}

popd


rm -rf "$DIR/Build"
rm -rf "$PREFIX"

mkdir -p "$DIR/Build/Binutils"
mkdir -p "$DIR/Build/gcc"
mkdir -p "$PREFIX"

export CFLAGS="-g0 -O2 -mtune=native"
export CXXFLAGS="-g0 -O2 -mtune=native"

pushd "$DIR/Build/Binutils"
	echo "Configuring Binutils..."
	../../Tarballs/${BINUTILS_NAME}/configure --target="$TARGET" --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror > >(sed 's/^/[binutils conf]: /')

	echo "Building Binutils..."
	make -j $MAKEJOBS > >(sed 's/^/[binutils build]: /')
	make install > >(sed 's/^/[binutils install]: /')
popd

PATH="$PREFIX/bin:$PATH"

pushd "$DIR/Build/gcc"
	echo "Configuring GCC..."
	../../Tarballs/${GCC_NAME}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers > >(sed 's/^/[gcc conf]: /')
	
	echo "Building GCC..."
	make all-gcc -j $MAKEJOBS  > >(sed 's/^/[gcc build]: /')
	make all-target-libgcc -j $MAKEJOBS > >(sed 's/^/[libgcc build]: /')
	make install-gcc > >(sed 's/^/[gcc install]: /')
	make install-target-libgcc > >(sed 's/^/[libgcc install]: /')
popd

echo "Done. Toolchain should be functional"
echo "You can delete the Build and Tarballs folders to free up space"
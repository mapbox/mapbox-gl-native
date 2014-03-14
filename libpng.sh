mkdir -p libpngtmp

echo "setting platform ${PLATFORM_PREFIX}"
cd libpngtmp


# wget "http://prdownloads.sourceforge.net/libpng/libpng-1.6.9.tar.gz?download" -O libpng-1.6.9.tar.gz
# tar xzvf "libpng-1.6.9.tar.gz"
cd libpng-1.6.9
./configure \
    --host=arm-linux-androideabi \
    CC=arm-linux-androideabi-clang3.3 \
    STRIP=arm-linux-androideabi-strip \
    --prefix=$PLATFORM_PREFIX \
    AR=arm-linux-androideabi-ar \
    RANLIB=arm-linux-androideabi-ranlib
make
make install
cd ../../
cp ./android-ndk-r9c/platforms/android-19/lib/libpng* jni

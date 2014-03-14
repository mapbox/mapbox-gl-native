mkdir -p libpngtmp

cd libpngtmp


wget "http://prdownloads.sourceforge.net/libpng/libpng-1.6.9.tar.gz?download"
tar xzvf "libpng-1.6.9.tar.gz?download"
cd libpng-1.6.9
./configure --host=arm-linux-androideabi --prefix=$PLATFORM_PREFIX
make
make install

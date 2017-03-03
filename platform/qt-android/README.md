How to build

1) build qt for android with ssl support
https://wiki.openssl.org/index.php/Android
http://doc.qt.io/qt-5/opensslsupport.html

2) build qmapboxgl for android
make qt-android  CMAKE_PREFIX_PATH=path_to_qt_root

for example
make  qt-android CMAKE_PREFIX_PATH=/opt/android-qt5/5.8.0/armeabi-v7a 

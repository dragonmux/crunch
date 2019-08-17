call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %1
meson build --prefix=%HOME%/local -Dcpp_std=c++14 -Db_coverage=%2
cd build
ninja

cd build || exit
meson compile
./raytracing >image.ppm
open image.ppm

set -x
BASE_INC=gstreamer/1.0/mingw_x86_64/include

x86_64-w64-mingw32-gcc -I. -I$BASE_INC/gstreamer-1.0  -I$BASE_INC/glib-2.0 \
    -I$BASE_INC/../lib/glib-2.0/include -I$BASE_INC/libsoup-2.4 -I$BASE_INC/json-glib-1.0 -Ilibyaml/include \
    -std=c++17 \
    appstream.c -o appstream.exe \
    -Lgstreamer/1.0/mingw_x86_64/lib -lglib-2.0 -lgobject-2.0 -ljson-glib-1.0 -lsoup-2.4 -lgstreamer-1.0 -lgstsdp-1.0 -lgstwebrtc-1.0 \
    -Llibyaml/src/.libs -lyaml -pthread

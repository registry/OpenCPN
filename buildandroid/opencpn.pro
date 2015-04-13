#-------------------------------------------------
#
# Project created by QtCreator 2014-03-27T23:08:24
#
#-------------------------------------------------

QT       += core gui opengl widgets

#QT += androidextras
LIBS += -lEGL

TARGET = opencpn

TEMPLATE = app



# Qt target
wxQt_Base=/home/dsr/Projects/wxqt/wxWidgets
wxQt_Build=build_androidgl_52

# OCPN target
OCPN_Base=/home/dsr/Projects/opencpn_sf/opencpn
OCPN_Build=build_android


INCLUDEPATH += $${wxQt_Base}/include/

INCLUDEPATH += $${OCPN_Base}/include/
INCLUDEPATH += $${OCPN_Base}/src/nmea0183

#LIBS += $${OCPN_Base}/$${OCPN_Build}/libopencpn.a
LIBS += $${OCPN_Base}/$${OCPN_Build}/libgorp.a
LIBS += $${OCPN_Base}/$${OCPN_Build}/libGARMINHOST.a
LIBS += $${OCPN_Base}/$${OCPN_Build}/libNMEA0183.a
LIBS += $${OCPN_Base}/$${OCPN_Build}/libS57ENC.a
LIBS += $${OCPN_Base}/$${OCPN_Build}/libSYMBOLS.a
LIBS += $${OCPN_Base}/$${OCPN_Build}/libTEXCMP.a
LIBS += $${OCPN_Base}/$${OCPN_Build}/lib/libGLUES.a


LIBS += -L$${wxQt_Base}/$${wxQt_Build}/lib

CONFIG(SHARED){
    INCLUDEPATH += $${wxQt_Base}/$${wxQt_Build}/lib/wx/include/arm-linux-androideabi-qt-unicode-3.1

LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_html-3.1-arm-linux-androideabi.so
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_baseu_xml-3.1-arm-linux-androideabi.so
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_qa-3.1-arm-linux-androideabi.so
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_adv-3.1-arm-linux-androideabi.so
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_core-3.1-arm-linux-androideabi.so
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_baseu-3.1-arm-linux-androideabi.so
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_aui-3.1-arm-linux-androideabi.so
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwxexpat-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwxregexu-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwxjpeg-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwxpng-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_baseu_net-3.1-arm-linux-androideabi.a

}
else{
INCLUDEPATH += $${wxQt_Base}/$${wxQt_Build}/lib/wx/include/arm-linux-androideabi-qt-unicode-static-3.1

LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_html-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_baseu_xml-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_qa-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_adv-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_core-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_baseu-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_aui-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwxexpat-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwxregexu-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwxjpeg-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwxpng-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_gl-3.1-arm-linux-androideabi.a
LIBS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_baseu_net-3.1-arm-linux-androideabi.a
LIBS += $${OCPN_Base}/$${OCPN_Build}/lib/libGL.a
}

LIBS += /home/dsr/Qt/5.3/android_armv7/lib/libQt5AndroidExtras.so

TARGETDEPS += $${OCPN_Base}/$${OCPN_Build}/libgorp.a
TARGETDEPS += $${OCPN_Base}/$${OCPN_Build}/libS57ENC.a
TARGETDEPS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_core-3.1-arm-linux-androideabi.a
TARGETDEPS += $${wxQt_Base}/$${wxQt_Build}/lib/libwx_qtu_gl-3.1-arm-linux-androideabi.a

DEFINES += __WXQT__

SOURCES += $$PWD/../buildandroid/ocpn_wrapper.cpp


CONFIG += mobility
CONFIG += debug
MOBILITY =

ANDROID_EXTRA_LIBS = $$PWD/../buildandroid/assetbridge/libs/armeabi/libassetbridge.so


# To execute the assetbridge runtime code, we make a custom modification to the android Activity method.
# so we include the sources for this patched version here

ANDROID_PACKAGE_SOURCE_DIR = $${OCPN_Base}/buildandroid/android
OTHER_FILES += $${OCPN_Base}/buildandroid/android/AndroidManifest.xml

s57_deployment.files += $$PWD/../data/s57data/chartsymbols.xml
s57_deployment.files += $$PWD/../data/s57data/attdecode.csv
s57_deployment.files += $$PWD/../data/s57data/rastersymbols-dark.png
s57_deployment.files += $$PWD/../data/s57data/rastersymbols-day.png
s57_deployment.files += $$PWD/../data/s57data/rastersymbols-dusk.png
s57_deployment.files += $$PWD/../data/s57data/s57attributes.csv
s57_deployment.files += $$PWD/../data/s57data/s57expectedinput.csv
s57_deployment.files += $$PWD/../data/s57data/s57objectclasses.csv
s57_deployment.path = /assets/s57data
INSTALLS += s57_deployment

ui_deployment.files += $$PWD/../src/bitmaps/styles.xml
ui_deployment.files += $$PWD/../src/bitmaps/toolicons_traditional.png
ui_deployment.files += $$PWD/../src/bitmaps/toolicons_journeyman.png
ui_deployment.files += $$PWD/../src/bitmaps/toolicons_journeyman_flat.png
ui_deployment.path = /assets/uidata
INSTALLS += ui_deployment

gshhs_deployment.files += $$PWD/../data/gshhs/wdb_borders_i.b
gshhs_deployment.files += $$PWD/../data/gshhs/wdb_rivers_i.b
gshhs_deployment.files += $$PWD/../data/gshhs/poly-i-1.dat
gshhs_deployment.path = /assets/gshhs
INSTALLS += gshhs_deployment

styles_deployment.files += $$PWD/../data/styles/qtstylesheet.qss
styles_deployment.path = /assets/styles
INSTALLS += styles_deployment

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = /home/dsr/Projects/opencpn_android/buildandroid/../buildandroid/assetbridge/libs/armeabi/libassetbridge.so
}


file:///usr/share/opencpn/s57data/S52RAZDS.RLE
file:///usr/share/opencpn/s57data/attdecode.csv
file:///usr/share/opencpn/s57data/chartsymbols.xml
file:///usr/share/opencpn/s57data/rastersymbols-dark.png
file:///usr/share/opencpn/s57data/rastersymbols-day.png
file:///usr/share/opencpn/s57data/rastersymbols-dusk.png
file:///usr/share/opencpn/s57data/s57attributes.csv
file:///usr/share/opencpn/s57data/s57expectedinput.csv
file:///usr/share/opencpn/s57data/s57objectclasses.csv


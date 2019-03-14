message("including $$PWD")

QT -= core gui

CONFIG += C++11
CONFIG += hide_symbols

TEMPLATE = lib
TARGET = shvchainpack

isEmpty(SHV_PROJECT_TOP_BUILDDIR) {
	SHV_PROJECT_TOP_BUILDDIR=$$shadowed($$PWD)/..
}
message ( SHV_PROJECT_TOP_BUILDDIR: '$$SHV_PROJECT_TOP_BUILDDIR' )

unix:DESTDIR = $$SHV_PROJECT_TOP_BUILDDIR/lib
win32:DESTDIR = $$SHV_PROJECT_TOP_BUILDDIR/bin

message ( DESTDIR: $$DESTDIR )

android {
DEFINES += ANDROID_BUILD
}

DEFINES += SHVCHAINPACK_BUILD_DLL

INCLUDEPATH += \
	../3rdparty/necrolog/include
	#$$QUICKBOX_HOME/libqf/libqfcore/include \
	#$$PROJECT_TOP_SRCDIR/qfopcua/libqfopcua/include \

LIBS += \
    -L$$DESTDIR \
    -lnecrolog
    #-lqfcore

include($$PWD/src/src.pri)
include($$PWD/c/c.pri)

#install
target.path=$$[INSTALL_ROOT]/opt/shv/lib/$$MULTIARCH
INSTALLS += target


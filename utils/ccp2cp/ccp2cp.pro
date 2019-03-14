TEMPLATE = app

QT -= core widgets gui

isEmpty(SHV_PROJECT_TOP_BUILDDIR) {
	SHV_PROJECT_TOP_BUILDDIR=$$shadowed($$PWD)/..
}
message ( SHV_PROJECT_TOP_BUILDDIR: '$$SHV_PROJECT_TOP_BUILDDIR' )

DESTDIR = $$SHV_PROJECT_TOP_BUILDDIR/bin

CCPCP_SRC_DIR = ../../libshvchainpack/c

INCLUDEPATH += \
	$$CCPCP_SRC_DIR \

SOURCES += \
	main.c \
    $$CCPCP_SRC_DIR/ccpcp.c \
	$$CCPCP_SRC_DIR/ccpon.c \
	$$CCPCP_SRC_DIR/cchainpack.c \
	$$CCPCP_SRC_DIR/ccpcp_convert.c \


HEADERS += \
	$$CCPCP_SRC_DIR/ccpcp.h \
	$$CCPCP_SRC_DIR/ccpon.h \
	$$CCPCP_SRC_DIR/cchainpack.h \
	$$CCPCP_SRC_DIR/ccpcp_convert.h \

#install
target.path=$$[INSTALL_ROOT]/opt/shv/bin
INSTALLS += target


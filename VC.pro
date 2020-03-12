include(qmake-target-platform.pri)
include(qmake-destination-path.pri)

QT       += core gui widgets network

TARGET    = VC
TEMPLATE  = app

CONFIG   += c++17

DESTDIR      = $$PWD/binaries/$$DESTINATION_PATH
OBJECTS_DIR  = $$PWD/build/$$DESTINATION_PATH/.obj
MOC_DIR      = $$PWD/build/$$DESTINATION_PATH/.moc
RCC_DIR      = $$PWD/build/$$DESTINATION_PATH/.qrc
UI_DIR       = $$PWD/build/$$DESTINATION_PATH/.ui

DEFINES     *= QT_USE_QSTRINGBUILDER

CONFIG(relase, debug|release) {
    CONFIG  += optimize_full
    DEFINES += NDEBUG
}

SOURCES     += \
    src/main.cpp \
    src/mainwindow.cpp

HEADERS     += \
    include/test.hpp \
    include/mainwindow.hpp

FORMS       += \
    src/mainwindow.ui 

INCLUDEPATH += \
    external/philslib/include \
    external/optional \
    external/expected


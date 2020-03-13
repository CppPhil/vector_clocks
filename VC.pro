include(qmake-target-platform.pri)
include(qmake-destination-path.pri)

QT       += core gui widgets network

TARGET    = VC
TEMPLATE  = app

CONFIG   += c++17 warn_off

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
    src/actor_id.cpp \
    src/main.cpp \
    src/main_window.cpp

HEADERS     += \
    include/actor_id.hpp \
    include/main_window.hpp \
    include/vector_timestamp.hpp

FORMS       += \
    src/main_window.ui

INCLUDEPATH += \
    include \
    external/philslib/include \
    external/optional \
    external/expected


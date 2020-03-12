win32 {
    CONFIG += PLATFORM_WIN

    win32-g++ {
        CONFIG += COMPILER_GCC
    }

    win32-msvc2017 {
        CONFIG += COMPILER_MSVC2017
        win32-msvc2017:QMAKE_TARGET.arch = x86_64
    }
}

linux {
    CONFIG += PLATFORM_LINUX
    # Make QMAKE_TARGET arch available for Linux
    !contains(QT_ARCH, x86_64) {
        QMAKE_TARGET.arch = x86
    } else {
        QMAKE_TARGET.arch = x86_64
    }

    linux-g++ {
        CONFIG += COMPILER_GCC
    }
}

macx {
    CONFIG += PLATFORM_OSX

    macx-clang {
        CONFIG += COMPILER_CLANG
        QMAKE_TARGET.arch = x86_64
    }

    macx-clang-32 {
        CONFIG += COMPILER_CLANG
        QMAKE_TARGET.arch = x86
    }
}

contains(QMAKE_TARGET.arch, x86_64) {
    CONFIG += PROCESSOR_x64
} else {
    CONFIG += PROCESSOR_x86
}

CONFIG(debug, release|debug) {
    CONFIG += BUILD_DEBUG
} else {
    CONFIG += BUILD_RELEASE
}


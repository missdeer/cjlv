# Find Boost library.

win32 : {
    # Try to use qmake variable's value.
    _BOOST_ROOT = $$BOOST_ROOT
    isEmpty(_BOOST_ROOT) {
        # Try to use the system environment value.
        _BOOST_ROOT = $$(BOOST_ROOT)
    }

    isEmpty(_BOOST_ROOT) {
        message(BOOST_ROOT not detected. You may set the environment variable BOOST_ROOT. For example, BOOST_ROOT=/path/to/boost_root, or run qmake with argument: qmake BOOST_ROOT=/path/to/boost_root)
        !build_pass:error(Please set the environment variable `BOOST_ROOT`. For example, BOOST_ROOT=/path/to/boost_root, or run qmake with argument: qmake BOOST_ROOT=/path/to/boost_root)
    } else {
        message(Boost Library headers detected in BOOST_ROOT = $$_BOOST_ROOT)
        INCLUDEPATH += $$_BOOST_ROOT
    }

    _BOOST_LIBS = $$BOOST_LIBS
    isEmpty(_BOOST_LIBS) {
        # Try to use the system environment value.
        _BOOST_LIBS = $$(BOOST_LIBS)
    }

    isEmpty(_BOOST_LIBS) {
        message(BOOST_LIBS not detected. You may set the environment variable BOOST_ROOT. For example, BOOST_LIBS=/path/to/boost_root/libs, or run qmake with argument: qmake BOOST_LIBS=/path/to/boost_root/libs)
        !build_pass:error(Please set the environment variable `_BOOST_LIBS`. For example, BOOST_LIBS=/path/to/boost_root/libs, or run qmake with argument: qmake BOOST_LIBS=/path/to/boost_root/libs)
    } else {
        message(Boost Library libs detected in BOOST_LIBS = $$_BOOST_LIBS)
        LIBS += -L$$_BOOST_LIBS
    }
}

unix : {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib

    !isEmpty($$BOOST_ROOT) {
        INCLUDEPATH += $$BOOST_ROOT
    }

    !isEmpty($$(BOOST_ROOT)) {
        INCLUDEPATH += $$(BOOST_ROOT)
    }

    !isEmpty($$BOOST_LIBS) {
        LIBS += -L$$BOOST_LIBS
    }

    !isEmpty($$(BOOST_LIBS)) {
        LIBS += -L$$(BOOST_LIBS)
    }
}

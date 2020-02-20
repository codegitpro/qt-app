QT       += core gui network
QT += widgets
QT += svg

CONFIG += c++14
CONFIG += sdk_no_version_check

TARGET = qtdtapp
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
unix:QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-unused-function -Wno-weak-vtables -Wno-sign-conversion -Wno-padded -Wno-sign-conversion -Wno-sign-compare

requires(qtConfig(treeview))

win32: LIBS += Ws2_32.lib
win32: LIBS += AdvAPI32.lib
win32: LIBS += userenv.lib
win32: LIBS += psapi.lib
win32: LIBS += iphlpapi.lib
win32: LIBS += user32.lib

RC_ICONS = icon.ico

ICON = mac.icns

RESOURCES += \
    src/platform/qt/qtdtapp.qrc

FORMS +=

HEADERS += \
    src/openstack/list_parser.h \
    src/openstack/object_detail.h \
    src/openstack/openstack.h \
    src/platform/device_platform.h \
    src/platform/lambda_scheduler.h \
    src/platform/lambda_task.h \
    src/platform/plat.h \
    src/platform/portal.h \
    src/platform/std_task_scheduler.h \
    src/platform/task_scheduler.h \
    src/platform/qt/qt_directory_view.h \
    src/platform/qt/qt_os_io.h \
    src/platform/qt/qt_plat.h \
    src/platform/qt/qt_task_scheduler.h \
    src/platform/qt/qt_worker.h \
    src/platform/qt/qtdtmainwindow.h \
    src/byteswap.h \
    src/caching_engine.h \
    src/directory.h \
    src/endian.h \
    src/general.h \
    src/ai_http.h \
    src/json.h \
    src/lookup_field.h \
    src/lsb.h \
    src/pack.h \
    src/text_field.h \
    src/text_map.h \
    src/platform/qt/theme.h \
    src/bridge/cpp/bool_result.hpp \
    src/bridge/cpp/directories_detail.hpp \
    src/bridge/cpp/directory_view.hpp \
    src/bridge/cpp/file_state.hpp \
    src/bridge/cpp/file_type.hpp \
    src/bridge/cpp/files_detail.hpp \
    src/bridge/cpp/header.hpp \
    src/bridge/cpp/http_progress_result.hpp \
    src/bridge/cpp/http_result.hpp \
    src/bridge/cpp/http_verb.hpp \
    src/bridge/cpp/long_result.hpp \
    src/bridge/cpp/message.hpp \
    src/bridge/cpp/os_form.hpp \
    src/bridge/cpp/os_io.hpp \
    src/bridge/cpp/portal.hpp \
    src/bridge/cpp/progress.hpp \
    src/bridge/cpp/status.hpp \
    src/bridge/cpp/task.hpp \
    src/bridge/cpp/worker.hpp \
    deps/stb/stb_image.h \
    deps/stb/stb_image.h \
    src/gui/about_form.h \
    src/gui/confirm_form.h \
    src/gui/login_form.h

SOURCES += \
    src/openstack/openstack.cpp \
    src/platform/qt/qtdtmain.cpp \
    src/platform/qt/qtdtmainwindow.cpp \
    src/platform/portal.cpp \
    src/caching_engine.cpp \
    src/directory.cpp \
    src/platform/std_plat.cpp


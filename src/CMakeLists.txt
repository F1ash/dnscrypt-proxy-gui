
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_DIR} ${ECM_KDE_MODULE_DIR})
include(KDEInstallDirs)

set(HEADERS
    enums.h
    mainwindow.h
    server_panel.h
    button_panel.h
    info_panel.h
    app_settings.h
    test_respond.h
    test_widget.h
    help_thread.h
    server_info.h
    resolver_entries.h
    click_label.h
    port_settings.h
    tray/traywidget.h)
set(SOURCES
    main.cpp
    mainwindow.cpp
    server_panel.cpp
    button_panel.cpp
    info_panel.cpp
    app_settings.cpp
    test_respond.cpp
    test_widget.cpp
    help_thread.cpp
    server_info.cpp
    resolver_entries.cpp
    click_label.cpp
    port_settings.cpp
    tray/traywidget.cpp)

find_package(Qt5Core            REQUIRED)
find_package(Qt5Widgets         REQUIRED)
find_package(Qt5DBus            REQUIRED)
find_package(KF5Auth            REQUIRED)
find_package(KF5Notifications   REQUIRED)
include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5DBus_INCLUDE_DIRS}
    ${Qt5DBus_PRIVATE_INCLUDE_DIRS})
#message("DBusPrivDirs: ${Qt5DBus_PRIVATE_INCLUDE_DIRS}")
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})
qt5_add_resources(RCCs dnscrypt_proxy_icons.qrc)
set (
    BUILD_PROJECT_LIBRARIES
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    KF5::Auth
    KF5::Notifications)
add_definitions(
    ${Qt5Core_DEFINITIONS}
    ${Qt5Widgets_DEFINITIONS}
    ${Qt5DBus_DEFINITIONS})

add_executable(
    ${APP_NAME}
    ${HEADERS}
    ${SOURCES}
    ${MOC_SOURCES}
    ${RCCs})
target_link_libraries(${APP_NAME} ${BUILD_PROJECT_LIBRARIES})

install ( TARGETS     ${APP_NAME}
          DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
#message("KNOTIFYRC_INSTALL_DIR: ${KNOTIFYRC_INSTALL_DIR}")
install(  FILES       ${APP_NAME}.notifyrc
          DESTINATION ${KNOTIFYRC_INSTALL_DIR})

add_subdirectory(helpers)

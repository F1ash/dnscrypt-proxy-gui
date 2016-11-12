
set(dnscrypt_client_helper_SRCS
    dnscrypt_client_helper.cpp)

find_package(Qt5 COMPONENTS DBus REQUIRED)
qt5_wrap_cpp (HELPER_MOC_SOURCES dnscrypt_client_helper.h)

set (
    HELPER_BUILD_PROJECT_LIBRARIES
    ${Qt5DBus_LIBRARIES}
    KF5::Auth
    KF5::Notifications)
add_definitions(
    ${Qt5Widgets_DEFINITIONS}
    ${Qt5DBus_DEFINITIONS})

# FYI: helper names don't like a CAPs letters !!!
add_executable(
    dnscrypt_client_helper
    ${dnscrypt_client_helper_SRCS}
    ${HELPER_MOC_SOURCES})
target_link_libraries(
    dnscrypt_client_helper
    ${HELPER_BUILD_PROJECT_LIBRARIES})
install(
    TARGETS     dnscrypt_client_helper
    DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
#message("KAUTH_HELPER_INSTALL_DIR: ${KAUTH_HELPER_INSTALL_DIR}")
#message("KAUTH_POLICY_FILES_INSTALL_DIR: ${KAUTH_POLICY_FILES_INSTALL_DIR}")
#message("DBUS_SYSTEM_SERVICES_INSTALL_DIR: ${DBUS_SYSTEM_SERVICES_INSTALL_DIR}")

kauth_install_helper_files(
    dnscrypt_client_helper
    pro.russianfedora.dnscrypt_client
    root)
kauth_install_actions(
    pro.russianfedora.dnscrypt_client
    actions.actions)


set (DNS_UTILITY "../dns_utility")
include_directories(${DNS_UTILITY})

set (dnscrypt_client_test_helper_SRCS
    ${DNS_UTILITY}/dns.c
    dnscrypt_client_test_helper.cpp)

find_package (Qt5Core   REQUIRED)
find_package (Qt5DBus   REQUIRED)
find_package (KF5Auth   REQUIRED)
qt5_wrap_cpp (TEST_HELPER_MOC_SOURCES
    dnscrypt_client_test_helper.h
    ${DNS_UTILITY}/dns.h)

set (
    TEST_HELPER_BUILD_PROJECT_LIBRARIES
    ${Qt5Core_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    KF5::Auth)
add_definitions(
    ${Qt5Core_DEFINITIONS}
    ${Qt5DBus_DEFINITIONS})

# FYI: helper names don't like a CAPs letters !!!
add_executable(
    dnscrypt_client_test_helper
    ${dnscrypt_client_test_helper_SRCS}
    ${TEST_HELPER_MOC_SOURCES})
target_link_libraries(
    dnscrypt_client_test_helper
    ${TEST_HELPER_BUILD_PROJECT_LIBRARIES})
install(
    TARGETS     dnscrypt_client_test_helper
    DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
#message("KAUTH_HELPER_INSTALL_DIR: ${KAUTH_HELPER_INSTALL_DIR}")
#message("KAUTH_POLICY_FILES_INSTALL_DIR: ${KAUTH_POLICY_FILES_INSTALL_DIR}")
#message("DBUS_SYSTEM_SERVICES_INSTALL_DIR: ${DBUS_SYSTEM_SERVICES_INSTALL_DIR}")

kauth_install_helper_files(
    dnscrypt_client_test_helper
    pro.russianfedora.dnscryptclienttest
    root)
kauth_install_actions(
    pro.russianfedora.dnscryptclienttest
    actions.actions)

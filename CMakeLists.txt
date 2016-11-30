cmake_minimum_required(VERSION 3.0)
set (CMAKE_INSTALL_PREFIX /usr)


project (dnscrypt-proxy-gui)
set (APP_NAME DNSCryptClient)

find_package(KF5Auth          REQUIRED)
find_package(KF5Notifications REQUIRED)
add_subdirectory(src)

install ( FILES       ${APP_NAME}.desktop
          DESTINATION ${SHARE_INSTALL_PREFIX}/applications )
install ( FILES       ${APP_NAME}@.service
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system )
install ( FILES src/icons/64x64/status/${APP_NAME}.png
          DESTINATION ${SHARE_INSTALL_PREFIX}/icons/hicolor/64x64/apps
          RENAME ${APP_NAME}.png )

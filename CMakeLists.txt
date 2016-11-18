cmake_minimum_required(VERSION 3.0)
set (CMAKE_INSTALL_PREFIX /usr)


project (DNSCryptClient)

find_package(KF5Auth          REQUIRED)
find_package(KF5Notifications REQUIRED)
add_subdirectory(src)

install ( FILES       ${PROJECT_NAME}.desktop
          DESTINATION ${SHARE_INSTALL_PREFIX}/applications )
install ( FILES       ${PROJECT_NAME}@.service
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system )
install ( FILES       ${PROJECT_NAME}.socket
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system )
install ( FILES src/icons/64x64/status/${PROJECT_NAME}.png
          DESTINATION ${SHARE_INSTALL_PREFIX}/icons/hicolor/64x64/apps
          RENAME ${PROJECT_NAME}.png )

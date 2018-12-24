cmake_minimum_required(VERSION 3.0)
set (CMAKE_INSTALL_PREFIX /usr)


project (dnscrypt-proxy-gui)
set (APP_NAME DNSCryptClient)

include(CheckFunctionExists)
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if (COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_subdirectory(src)

install ( FILES       ${APP_NAME}.desktop
          DESTINATION ${SHARE_INSTALL_PREFIX}/applications )
install ( FILES       ${APP_NAME}@.service
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system )
install ( FILES       ${APP_NAME}_test@.service
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system )
install ( FILES       ${APP_NAME}_test_v2.service
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system )
install ( FILES src/icons/64x64/status/${APP_NAME}.png
          DESTINATION ${SHARE_INSTALL_PREFIX}/icons/hicolor/64x64/apps
          RENAME ${APP_NAME}.png )

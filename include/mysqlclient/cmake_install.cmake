# Install script for directory: /home/arshia/repos/wasmer/python/mariadb-connector-c/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mariadb" TYPE FILE FILES
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/mariadb_com.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/mysql.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/mariadb_stmt.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/ma_pvio.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/ma_tls.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/build/include/mariadb_version.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/ma_list.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/errmsg.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/mariadb_dyncol.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/mariadb_ctype.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/mariadb_rpl.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/mysqld_error.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mariadb/mysql" TYPE FILE FILES
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/mysql/client_plugin.h"
    "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/mysql/plugin_auth.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mariadb/mariadb" TYPE FILE FILES "/home/arshia/repos/wasmer/python/mariadb-connector-c/include/mariadb/ma_io.h")
endif()


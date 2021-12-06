
cmake_minimum_required(VERSION 3.10)

set(VCPKG_INCLUDE_WIN32 "D:\\WorkStation\\Source\\vcpkg\\installed\\x86-windows\\include")
set(SDK_INCLUDE ../../sdk/include)

# set the project name
project(libcc VERSION 1.0)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${VCPKG_INCLUDE_WIN32}
    ${SDK_INCLUDE}
)

aux_source_directory(. SRC_LIST)
aux_source_directory(crypto SRC_CRYPTO_LIST)
aux_source_directory(element SRC_ELEMENT_LIST)
aux_source_directory(network SRC_NETWORK_LIST)
aux_source_directory(test SRC_TEST_LIST)

add_executable(libcc 
    ${SRC_LIST}
    ${SRC_CRYPTO_LIST}
    ${SRC_ELEMENT_LIST}
    ${SRC_NETWORK_LIST}
    ${SRC_TEST_LIST}
 )



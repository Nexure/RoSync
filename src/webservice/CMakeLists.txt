# CMakeLists.txt : CMake Project for RoSync WebService

set (SOURCES src/webservice.cpp)

find_path(UWEBSOCKETS_INCLUDE_DIRS "uwebsockets/App.h")

add_definitions(-DLIBUS_NO_SSL -DUWS_NO_ZLIB)

# TODO: Shout at vcpkg for being bad. This shouldn't happen!
include_directories (include ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
link_directories(${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)

add_executable (webservice ${SOURCES})
target_link_libraries (webservice core usockets)
target_include_directories (webservice PRIVATE ${UWEBSOCKETS_INCLUDE_DIRS})
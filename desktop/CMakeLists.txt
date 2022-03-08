cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${ARCH_TARGET})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${ARCH_TARGET})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${ARCH_TARGET})
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

project(desktop CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

function(install_package package)
   conan_cmake_configure(REQUIRES ${package} GENERATORS cmake_find_package)
   conan_cmake_autodetect(settings BUILD_TYPE Release)
   conan_cmake_install(PATH_OR_REFERENCE . BUILD missing REMOTE conancenter SETTINGS ${settings})
   message("########## INSTALLED ##########   ${package}")
endfunction()

install_package(sdl/2.0.20)
install_package(glew/2.2.0)
install_package(openvr/1.16.8)
install_package(catch2/2.13.8)
install_package(yaml-cpp/0.7.0)

find_package(SDL2)
find_package(GLEW)
find_package(openvr)
find_package(yaml-cpp)
find_package(Catch2)

set(CORE_MODULE "${CMAKE_SOURCE_DIR}/../core")
include_directories("${CORE_MODULE}/src")
if (WIN32)
   link_libraries("${CORE_MODULE}/bin/Release/core.lib")
endif (WIN32)
if (UNIX)
   link_libraries("${CORE_MODULE}/bin/libcore.so")
endif (UNIX)

set(vr_client_LIBS SDL2::SDL2main GLEW::GLEW openvr::openvr yaml-cpp::yaml-cpp)

add_library(vr_client SHARED "src/application.cpp" "src/rendering.cpp")
target_link_libraries(vr_client ${vr_client_LIBS})

find_package(Catch2)
add_library(catch2 INTERFACE)
target_sources(catch2 INTERFACE ${Catch2_INCLUDE_DIR}/catch2/catch.hpp)
add_executable(test "test/main.cpp")
target_include_directories(test PRIVATE ${Catch2_INCLUDE_DIRS})
target_link_libraries(test vr_client ${vr_client_LIBS} ${Catch2_LIBS})

add_executable(HelloVR "src/main.cpp")
target_link_libraries(HelloVR vr_client ${desktop_LIBS})

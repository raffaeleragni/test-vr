cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${ARCH_TARGET})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${ARCH_TARGET})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${ARCH_TARGET})
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

project(core CXX)
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

install_package(lua/5.3.5)
install_package(sol2/3.2.3)
install_package(catch2/2.13.8)
install_package(yaml-cpp/0.7.0)
find_package(lua)
find_package(sol2)
find_package(yaml-cpp)
find_package(Catch2)

link_libraries(lua::lua sol2::sol2 yaml-cpp::yaml-cpp)

add_library(catch2 INTERFACE)
target_sources(catch2 INTERFACE ${Catch2_INCLUDE_DIR}/catch2/catch.hpp)

add_library(core SHARED "src/core.cpp" "src/scripting.cpp")

add_executable(test "test/main.cpp" "test/test_types.cpp" "test/test_scripting.cpp" "test/test_core.cpp")
target_include_directories(test PRIVATE "src" ${Catch2_INCLUDE_DIRS})
target_link_libraries(test core ${Catch2_LIBS})

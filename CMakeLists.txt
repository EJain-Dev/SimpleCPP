cmake_minimum_required(VERSION 3.10)

project(SimpleCPP LANGUAGES CXX)

set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 0)
set(CMAKE_PROJECT_VERSION_TWEAK 2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TEST "Enable testing" ON)

add_library(SimpleCPP INTERFACE)
target_include_directories(SimpleCPP INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

if (TEST)

enable_testing()
find_package(GTest CONFIG REQUIRED)
add_subdirectory(tests)

endif()
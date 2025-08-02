cmake_minimum_required(VERSION 3.10)

project(SimpleCPP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

find_package(GTest CONFIG REQUIRED)

add_library(SimpleCPP INTERFACE)
target_include_directories(SimpleCPP INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

add_subdirectory(tests)
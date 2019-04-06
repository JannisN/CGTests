cmake_minimum_required(VERSION 3.8)

project(CGTests)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE src
	src/*.cpp
	src/*.h)

add_executable(CGTests ${src})
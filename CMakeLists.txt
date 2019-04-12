cmake_minimum_required(VERSION 3.8)

project(CGTests)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory("extern/glfw")

find_package(OpenGL)
find_package(OpenCL REQUIRED)

set(Includes
	"extern/opencl"
	"extern/glfw/include"
)

set(LibrariesRelease
    OpenCL::OpenCL
	"${OPENGL_gl_LIBRARY}"
    glfw
)
set(LibrariesDebug
    OpenCL::OpenCL
	"${OPENGL_gl_LIBRARY}"
    glfw
)

file(GLOB_RECURSE src
	src/*.cpp
	src/*.h)

include_directories(${Includes})

add_executable(CGTests ${src})

foreach(lib ${LibrariesDebug})
	target_link_libraries(CGTests debug ${lib})
endforeach()
foreach(lib ${LibrariesRelease})
	target_link_libraries(CGTests optimized ${lib})
endforeach()

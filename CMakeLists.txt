cmake_minimum_required(VERSION 3.8)

project(CGTests)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory("extern/glfw")

find_package(OpenGL)
find_package(OpenCL REQUIRED)

#set(OpenCLInclude "" CACHE FILEPATH "OpenCL include directory")
#if(WIN32)
#	set(OpenCLLib64 "" CACHE FILEPATH "OpenCL .lib 64")
#	set(OpenCLLib32 "" CACHE FILEPATH "OpenCL .lib 32")
#else()
#    set(OpenCLLib "" CACHE FILEPATH "OpenCL .lib")
#endif()

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

#if(WIN32)
#	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#		set(LibrariesRelease "${LibrariesRelease}" "${OpenCLLib64}")
#		set(LibrariesDebug "${LibrariesDebug}" "${OpenCLLib64}")
#	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
#		set(LibrariesRelease "${LibrariesRelease}" "${OpenCLLib32}")
#		set(LibrariesDebug "${LibrariesDebug}" "${OpenCLLib32}")
#	endif()
#else()
#    set(LibrariesRelease "${LibrariesRelease}" "${OpenCLLib}")
#    set(LibrariesDebug "${LibrariesDebug}" "${OpenCLLib}")
#endif()

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

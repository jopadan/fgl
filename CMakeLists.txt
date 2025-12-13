cmake_minimum_required( VERSION 3.10 )

project( FGL VERSION 0.1 LANGUAGES C )
set(PROJECT_DESCRIPTION "Funky Graphics Library is an OpenGL platform framework written in C")

set( CMAKE_C_STANDARD 23 )
set( CMAKE_C_STANDARD_REQUIRED ON )
set( CMAKE_C_EXTENSIONS ON )

include(GNUInstallDirs)

option(BUILD_SHARED_LIBS  "Build shared libraries."                                     ON)
option(BUILD_STATIC_LIBS  "Build static libraries."                                     ON)
option(BUILD_EXAMPLES     "Build examples/samples."                                     ON)
option(INSTALL_EXAMPLE_SOURCES "Install example/sample sources."                        ON)
option(GARBAGE_COLLECT    "Link time garbage collect sections using -Wl,--gc-sections." ON)
option(STRIP_BINARIES     "Strip unused sections of generated binaries using -Wl,-s."   ON)

# provide Release and Debug build types
if( NOT CMAKE_BUILD_TYPE )
        set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build."   FORCE )
endif()
if( CMAKE_BUILD_TYPE STREQUAL Debug )
	set(STRIP_BINARIES OFF)
	# enable debug symbols
	add_compile_options(-g)
	# enable debug output for garbage collection
	add_link_options(-Wl,--print-gc-sections)
endif()
# use native architecture and set optimization level
if( NOT OPTIMIZATION_LEVEL )
	set( OPTIMIZATION_LEVEL 3     CACHE STRING "Compile optimization level." FORCE )
endif()
add_compile_options(-march=native -O${OPTIMIZATION_LEVEL})
# use SSE SIMD math for x86_64
if( CMAKE_GENERATOR_PLATFORM STREQUAL "x64" OR CMAKE_GENERATOR_PLATFORM STREQUAL "x86_64" OR CMAKE_GENERATOR_PLATFORM STREQUAL "AMD64" )
	add_compile_options(-mfpmath=sse)
endif()
if(STRIP_BINARIES)
	add_link_options(-Wl,-s)
endif()
if(GARBAGE_COLLECT)
	add_link_options(-Wl,--gc-sections)
endif()
add_compile_options(-fstdarg-opt)
add_compile_options(-fdata-sections)
add_compile_options(-fpermissive)
add_compile_options(-ffunction-sections)
add_compile_options(-D_FILE_OFFSET_BITS=64)
add_compile_options(-Wno-array-bounds)
add_compile_options(-Wno-unused-parameter)
add_compile_options(-Wno-unused-result)
add_compile_options(-Wno-pedantic)
add_compile_options(-Wno-narrowing)

# use all the C files in src/ directory
FILE(GLOB FGL_SOURCES src/*.c)
# set include directory path to include/ directory
include_directories(INTERFACE
	PUBLIC_HEADER  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	PRIVATE_HEADER $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/external/fontstash/src>
	PRIVATE_HEADER $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/external/stb>
)

# check for existence of glfw3 platform framework
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
link_libraries(glfw3 m OpenGL GLU)

# build shared version of the library
if(BUILD_SHARED_LIBS)
add_library( ${PROJECT_NAME}_shared SHARED ${FGL_SOURCES})
set_target_properties(${PROJECT_NAME}_shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}_shared)
endif()

# build static version if examples are enabled
if(BUILD_STATIC_LIBS)
add_library( ${PROJECT_NAME}_static STATIC ${FGL_SOURCES})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}_static)
endif()

install(FILES ${PROJECT_SOURCE_DIR}/include/fgl.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT headers)
install(FILES ${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR} COMPONENT docs)

add_library(${PROJECT_NAME} INTERFACE)
if(BUILD_SHARED_LIBS)
	target_link_libraries(${PROJECT_NAME} INTERFACE ${PROJECT_NAME}_shared)
endif()
if(BUILD_STATIC_LIBS)
	target_link_libraries(${PROJECT_NAME} INTERFACE ${PROJECT_NAME}_static)
endif()

if(BUILD_EXAMPLES AND (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS))
# use all the C files in examples/ directory and link to static version of library
FILE(GLOB FGL_EXAMPLE_SOURCES examples/*.c)
foreach(example IN ITEMS ${FGL_EXAMPLE_SOURCES})
	get_filename_component(FGL_EXAMPLE_NAME ${example} NAME_WLE)
	add_executable(${FGL_EXAMPLE_NAME} ${example})
	set_target_properties(${FGL_EXAMPLE_NAME} PROPERTIES OUTPUT_NAME examples/${FGL_EXAMPLE_NAME})
	target_link_libraries(${FGL_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
	install(TARGETS ${FGL_EXAMPLE_NAME} DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/examples")
	if(INSTALL_EXAMPLE_SOURCES)
		install(FILES ${example} DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/examples")
	endif()
endforeach()

# copy example binaries to binary directory for out of source tree build
FILE   (COPY      examples/res DESTINATION "${PROJECT_BINARY_DIR}/examples")
# install examples system-wide
install(DIRECTORY examples/res DESTINATION "share/${PROJECT_NAME}/examples")
endif()


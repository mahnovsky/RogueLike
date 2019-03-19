cmake_minimum_required(VERSION 2.8)

project(RogueLikeTests)

set( BASE_PATH "../code" )
set( SOURCE_PATH "../code/src" )
set( THIRD_PARTY_PATH "${BASE_PATH}/third_party" )
add_definitions(-D_DEBUG)

if(UNIX AND NOT APPLE)
    set(TARGET_BUILD_PLATFORM "linux")
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed -ldl" )
endif()

if (WIN32)
    set(TARGET_BUILD_PLATFORM "win32")
endif (WIN32)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")

file(GLOB BASIC_SRC ${SOURCE_PATH}/basic/*.cpp ${})
file(GLOB PLATFORM_SRC ${SOURCE_PATH}/platform/${TARGET_BUILD_PLATFORM}/*.cpp ${PLATFORM_SRC})
file(GLOB JSON_SRC ${THIRD_PARTY_PATH}/cJSON/cJSON.c)

set(SRC_FILES ${BASIC_SRC} ${PLATFORM_SRC} ${JSON_SRC} "test_main.cpp")

include_directories("../code/include")
include_directories(${THIRD_PARTY_PATH})

set(SRC_FILES ${SRC_FILES} "hash_map_test.cpp")

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g" )

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# Now simply link against gtest or gtest_main as needed. Eg
add_executable( ${PROJECT_NAME} ${SRC_FILES} )
target_link_libraries(${PROJECT_NAME} gtest_main)
add_test(NAME example_test COMMAND example)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})

find_package(X11 REQUIRED)
target_link_libraries(${PROJECT_NAME} X11)

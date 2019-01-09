cmake_minimum_required(VERSION 2.8)

project(RogueLike)

set( BASE_PATH "./code" )
set( SOURCE_PATH "./code/src" )

if(UNIX AND NOT APPLE)
    set(TARGET_BUILD_PLATFORM "linux")
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl" )
    set( PLATFORM_SRC ${BASE_PATH}/linux_startup.cpp )
endif()

if (WIN32)
    set(TARGET_BUILD_PLATFORM "win32")
endif (WIN32)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")

file(GLOB ENGINE_SRC ${SOURCE_PATH}/*.cpp)
file(GLOB C_SRC ${SOURCE_PATH}/*.c)
file(GLOB BASIC_SRC ${SOURCE_PATH}/basic/*.cpp ${})
file(GLOB PLATFORM_SRC ${SOURCE_PATH}/platform/${TARGET_BUILD_PLATFORM}/*.cpp ${PLATFORM_SRC})
file(GLOB GAME_SRC ${BASE_PATH}/game.cpp ${BASE_PATH}/game/*.cpp)

set(SRC_FILES ${BASIC_SRC} ${ENGINE_SRC} ${PLATFORM_SRC} ${C_SRC} ${GAME_SRC})

include_directories("code/include")

add_executable( ${PROJECT_NAME} ${SRC_FILES} )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g" )

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})

find_package(X11 REQUIRED)
target_link_libraries(${PROJECT_NAME} X11)

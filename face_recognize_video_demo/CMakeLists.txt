cmake_minimum_required(VERSION 2.8)

project(ali_face_engine_demo)

ADD_DEFINITIONS(-DOPENCV)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    IF (${CMAKE_CL_64} MATCHES "1")
        link_libraries(../../ali_face_engine_sdk/c++/${CMAKE_SYSTEM_NAME}/x64/pthreadVC2)
        link_libraries(../../ali_face_engine_sdk/c++/${CMAKE_SYSTEM_NAME}/x64/AliFaceEngine)
    ELSE ()
        link_libraries(../../ali_face_engine_sdk/c++/${CMAKE_SYSTEM_NAME}/x86/pthreadVC2)
        link_libraries(../../ali_face_engine_sdk/c++/${CMAKE_SYSTEM_NAME}/x86/AliFaceEngine)
    ENDIF ()
ELSE ()
    include_directories(../ali_face_engine_sdk/${CMAKE_SYSTEM_NAME}/include)
    link_directories(../ali_face_engine_sdk/${CMAKE_SYSTEM_NAME})
ENDIF ()

message(../ali_face_engine_sdk/${CMAKE_SYSTEM_NAME}/include)

add_executable(demo demo.cpp)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(demo)
ELSE ()
    target_link_libraries(demo AliFaceEngine pthread dl ${OpenCV_LIBS})
ENDIF ()

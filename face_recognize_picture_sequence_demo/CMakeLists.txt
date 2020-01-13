cmake_minimum_required(VERSION 2.8)

project(ali_face_engine_demo)

IF (ARM_LINUX)
    SET(CMAKE_C_COMPILER "arm-linux-gcc")
    SET(CMAKE_CXX_COMPILER "arm-linux-g++")
    ADD_DEFINITIONS(-DARM_LINUX)
    ADD_DEFINITIONS(-DXcnn_ARM)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfpu=vfpv3-d16 -mthumb -mfpu=neon -mcpu=cortex-a53 -mtune=cortex-a53")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfpu=vfpv3-d16 -mthumb -mfpu=neon -mcpu=cortex-a53 -mtune=cortex-a53")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
ENDIF ()

ADD_DEFINITIONS(-std=c++11)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    IF (${CMAKE_CL_64} MATCHES "1")
        link_libraries(../../ali_face_engine_sdk/c++/${CMAKE_SYSTEM_NAME}/x64/pthreadVC2)
        link_libraries(../../ali_face_engine_sdk/c++/${CMAKE_SYSTEM_NAME}/x64/AliFaceEngine)
    ELSE ()
        link_libraries(../../ali_face_engine_sdk/c++/${CMAKE_SYSTEM_NAME}/x86/pthreadVC2)
        link_libraries(../../ali_face_engine_sdk/c++/${CMAKE_SYSTEM_NAME}/x86/AliFaceEngine)
    ENDIF ()
ELSEIF (ARM_LINUX)
    include_directories(../ali_face_engine_sdk/Arm/include)
    link_directories(../ali_face_engine_sdk/Arm)
ELSE ()
    include_directories(../ali_face_engine_sdk/${CMAKE_SYSTEM_NAME}/include)
    link_directories(../ali_face_engine_sdk/${CMAKE_SYSTEM_NAME})
ENDIF ()

message(../ali_face_engine_sdk/${CMAKE_SYSTEM_NAME}/include)

add_executable(demo demo.cpp)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(demo)
ELSE ()
    target_link_libraries(demo AliFaceEngine pthread dl)
ENDIF ()

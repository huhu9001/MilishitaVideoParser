cmake_minimum_required(VERSION 3.28)
project(MilishitaVideoParser VERSION 2.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(path.cmake OPTIONAL)

find_path(DIR_FFMPEG lib/avcodec.lib)
if (DIR_FFMPEG STREQUAL DIR_FFMPEG-NOTFOUND)
    message(SEND_ERROR "DIR_FFMPEG: ffmpeg not found.")
    message("=>https://github.com/FFmpeg/FFmpeg")
endif()

add_library(avcodec SHARED IMPORTED)
add_library(avformat SHARED IMPORTED)
add_library(swscale SHARED IMPORTED)
add_library(avutil SHARED IMPORTED)
set_property(TARGET avcodec PROPERTY IMPORTED_IMPLIB ${DIR_FFMPEG}/lib/avcodec.lib)
set_property(TARGET avformat PROPERTY IMPORTED_IMPLIB ${DIR_FFMPEG}/lib/avformat.lib)
set_property(TARGET swscale PROPERTY IMPORTED_IMPLIB ${DIR_FFMPEG}/lib/swscale.lib)
set_property(TARGET avutil PROPERTY IMPORTED_IMPLIB ${DIR_FFMPEG}/lib/avutil.lib)
link_libraries(avcodec avformat swscale avutil)
include_directories(${DIR_FFMPEG}/include)

add_executable(MilishitaVideoParser
    main.cpp
    noteParser.cpp
    noteParser_CircleSeeker.cpp
    noteParser_StripeSeeker.cpp
    noteParser_TimeNotes.cpp)
target_compile_definitions(MilishitaVideoParser PUBLIC DIFFICULTY_NAME=DMM)
add_executable(MilishitaVideoParser_2m
    main.cpp
    noteParser.cpp
    noteParser_CircleSeeker.cpp
    noteParser_StripeSeeker.cpp
    noteParser_TimeNotes.cpp)
target_compile_definitions(MilishitaVideoParser_2m PUBLIC DIFFICULTY_NAME=D2M)
add_executable(MilishitaVideoParser_2p
    main.cpp
    noteParser.cpp
    noteParser_CircleSeeker.cpp
    noteParser_StripeSeeker.cpp
    noteParser_TimeNotes.cpp)
target_compile_definitions(MilishitaVideoParser_2p PUBLIC DIFFICULTY_NAME=D2S)
add_executable(MilishitaVideoParser_4m
    main.cpp
    noteParser.cpp
    noteParser_CircleSeeker.cpp
    noteParser_StripeSeeker.cpp
    noteParser_TimeNotes.cpp)
target_compile_definitions(MilishitaVideoParser_4m PUBLIC DIFFICULTY_NAME=D4M)
cmake_minimum_required(VERSION 3.28)
project(MilishitaVideoParser VERSION 2.0)

set(DIR_FFMPEG ../ffmpeg/ffmpeg-20200515-b18fd2b-win64-dev)
add_library(avcodec SHARED IMPORTED)
add_library(avformat SHARED IMPORTED)
add_library(swscale SHARED IMPORTED)
add_library(avutil SHARED IMPORTED)
set_property(TARGET avcodec PROPERTY IMPORTED_IMPLIB ${DIR_FFMPEG}/lib/avcodec.lib)
set_property(TARGET avformat PROPERTY IMPORTED_IMPLIB ${DIR_FFMPEG}/lib/avformat.lib)
set_property(TARGET swscale PROPERTY IMPORTED_IMPLIB ${DIR_FFMPEG}/lib/swscale.lib)
set_property(TARGET avutil PROPERTY IMPORTED_IMPLIB ${DIR_FFMPEG}/lib/avutil.lib)
link_libraries(avcodec avformat swscale avutil)
include_directories(SYSTEM ${DIR_FFMPEG}/include)

add_executable(MilishitaVideoParser
	MilishitaVideoParser/main.cpp
	MilishitaVideoParser/circle.cpp
	MilishitaVideoParser/color.cpp
	MilishitaVideoParser/noteParser.cpp
	MilishitaVideoParser/noteParser_CircleSeeker.cpp
	MilishitaVideoParser/noteParser_StripeSeeker.cpp
	MilishitaVideoParser/noteParser_TimeNotes.cpp
	MilishitaVideoParser/noteParser_mm.cpp
)
add_executable(MilishitaVideoParser_2m
	MilishitaVideoParser/main.cpp
	MilishitaVideoParser/circle.cpp
	MilishitaVideoParser/color.cpp
	MilishitaVideoParser/noteParser.cpp
	MilishitaVideoParser/noteParser_CircleSeeker.cpp
	MilishitaVideoParser/noteParser_StripeSeeker.cpp
	MilishitaVideoParser/noteParser_TimeNotes.cpp
	MilishitaVideoParser_2m/noteParser_2m.cpp
)
add_executable(MilishitaVideoParser_2p
	MilishitaVideoParser/main.cpp
	MilishitaVideoParser/circle.cpp
	MilishitaVideoParser/color.cpp
	MilishitaVideoParser/noteParser.cpp
	MilishitaVideoParser/noteParser_CircleSeeker.cpp
	MilishitaVideoParser/noteParser_StripeSeeker.cpp
	MilishitaVideoParser/noteParser_TimeNotes.cpp
	MilishitaVideoParser_2p/noteParser_2p.cpp
)
add_executable(MilishitaVideoParser_4m
	MilishitaVideoParser/main.cpp
	MilishitaVideoParser/circle.cpp
	MilishitaVideoParser/color.cpp
	MilishitaVideoParser/noteParser.cpp
	MilishitaVideoParser/noteParser_CircleSeeker.cpp
	MilishitaVideoParser/noteParser_StripeSeeker.cpp
	MilishitaVideoParser/noteParser_TimeNotes.cpp
	MilishitaVideoParser_4m/noteParser_4m.cpp
)
cmake_minimum_required(VERSION 2.8)
project( shrink )
find_package( OpenCV REQUIRED )
add_executable( shrink shrink.cpp )
target_link_libraries( shrink ${OpenCV_LIBS} )


cmake_minimum_required(VERSION 2.8)
project( work )
find_package( OpenCV REQUIRED )
add_executable( work work.cpp )
target_link_libraries( work ${OpenCV_LIBS} )


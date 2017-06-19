cmake_minimum_required(VERSION 2.8)
project( amplify )
find_package( OpenCV REQUIRED )
add_executable( amplify amplify.cpp )
target_link_libraries( amplify ${OpenCV_LIBS} )


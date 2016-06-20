cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project("My Project")

set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_package(freenect2 REQUIRED)

include_directories("/usr/include/libusb-1.0/")

INCLUDE_DIRECTORIES(
  ${freenect2_INCLUDE_DIR}
)

add_executable(dCreator src/DatasetCreatorDrag.cpp)
add_executable(imageViewer src/retrieveMatrix.cpp)
add_executable(listFiles src/listFiles.cpp)
add_executable(slideWindow src/slideWindow.cpp)
add_executable(featExtractor src/featExtractor.cpp)

target_link_libraries(dCreator  ${OpenCV_LIBS}
                               ${CMAKE_THREAD_LIBS_INIT}
                               ${freenect2_LIBRARIES})

target_link_libraries(imageViewer  ${OpenCV_LIBS}
                               ${CMAKE_THREAD_LIBS_INIT}
                               ${freenect2_LIBRARIES})	

target_link_libraries(listFiles  ${OpenCV_LIBS}
                               ${CMAKE_THREAD_LIBS_INIT}
                               ${freenect2_LIBRARIES})
target_link_libraries(slideWindow  ${OpenCV_LIBS}
                               ${CMAKE_THREAD_LIBS_INIT}
                               ${freenect2_LIBRARIES})
target_link_libraries(featExtractor  ${OpenCV_LIBS}
                               ${CMAKE_THREAD_LIBS_INIT}
                               ${freenect2_LIBRARIES})


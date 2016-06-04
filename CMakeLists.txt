cmake_minimum_required(VERSION 3.5)
project(ImageBlurDetection)

find_package(Boost 1.60.0 COMPONENTS system filesystem REQUIRED)

if(Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})

endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp NetworkService/NetworkService.cpp NetworkService/NetworkService.h JSONParser/JSONParser.cpp JSONParser/JSONParser.h ImageWriter/ImageWriter.cpp ImageWriter/ImageWriter.h)
add_executable(ImageBlurDetection ${SOURCE_FILES})
if(Boost_FOUND)
    # linux下编译需要加-lpthread
    target_link_libraries(ImageBlurDetection -lpthread -lboost_regex -lboost_date_time ${Boost_LIBRARIES})

endif()

FIND_PACKAGE(OpenCV REQUIRED)
if(OpenCV_FOUND)
    # linux下编译需要加-lpthread
    target_link_libraries(ImageBlurDetection -lpthread ${OpenCV_LIBS})
endif()
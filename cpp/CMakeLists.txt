cmake_minimum_required(VERSION 2.8.8)

project(cmfbindings)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

set(cmfbindings_SRCS
    streaming/BufferPool.cpp
    streaming/ConstBuffer.cpp
    streaming/MessageBuilder.cpp
    streaming/MessageParser.cpp

    tests/TestCMF.cpp
)

add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(.)

add_executable(cmfbindings ${cmfbindings_SRCS})
set(cmfbindings_LIBS ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${CMAKE_DL_LIBS})
target_link_libraries(cmfbindings ${cmfbindings_LIBS})

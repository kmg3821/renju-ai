cmake_minimum_required(VERSION 3.16)
project(renju C CXX)

set(CMAKE_CXX_STANDARD 17)

set(TENSORFLOW_SOURCE_DIR "" CACHE PATH
  "Directory that contains the TensorFlow project" )
if(NOT TENSORFLOW_SOURCE_DIR)
  get_filename_component(TENSORFLOW_SOURCE_DIR
    "${CMAKE_CURRENT_LIST_DIR}/tensorflow" ABSOLUTE)
endif()

set(MONGOOSE_SOURCE_DIR "" CACHE PATH
  "Directory that contains the Mongoose project" )
if(NOT MONGOOSE_SOURCE_DIR)
  get_filename_component(MONGOOSE_SOURCE_DIR
    "${CMAKE_CURRENT_LIST_DIR}/mongoose" ABSOLUTE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(
  "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite" EXCLUDE_FROM_ALL)

add_executable(renju main.cpp search.cpp rule.cpp 
  ${MONGOOSE_SOURCE_DIR}/mongoose.c
)

target_include_directories(renju PRIVATE
  ${MONGOOSE_SOURCE_DIR}
  ${MONGOOSE_SOURCE_DIR}/src
)

target_link_libraries(renju tensorflow-lite)


if(ENABLE_TEST)
add_definitions(-DENABLE_TEST)
endif()

# set output binary dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Output)

# add source directory macro
add_definitions(-DCCPELIB_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")






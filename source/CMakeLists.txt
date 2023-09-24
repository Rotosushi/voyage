cmake_minimum_required(VERSION 3.20)

set(VOYAGE_SOURCE_FILES

)

add_executable(voyage 
    ${VOYAGE_SOURCE_FILES}
    ${VOYAGE_SOURCE_DIR}/main.cpp
)
target_include_directories(voyage PUBLIC ${VOYAGE_INCLUDE_DIR})
target_compile_options(voyage PUBLIC ${CXX_OPTIONS})
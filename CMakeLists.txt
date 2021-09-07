project(bamfilter)
cmake_minimum_required(VERSION 3.5.2)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(${PROJECT_NAME} main.cpp include include/CIGAR.hpp include/CIGAR.cpp include/filtering.hpp include/filtering.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
conan_target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS} -static-libgcc -static-libstdc++)

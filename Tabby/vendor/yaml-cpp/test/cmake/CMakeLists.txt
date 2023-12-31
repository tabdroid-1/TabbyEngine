cmake_minimum_required(VERSION 3.5)
project(yaml-cpp-consumer LANGUAGES CXX)

find_package(yaml-cpp CONFIG REQUIRED)
get_target_property(LIBRARY_TYPE yaml-cpp::yaml-cpp TYPE)

if(LIBRARY_TYPE STREQUAL "SHARED_LIBRARY")
  if(NOT YAML_CPP_SHARED_LIBS_BUILT)
    message(FATAL_ERROR "Library type (${LIBRARY_TYPE}) contradicts config: ${YAML_CPP_SHARED_LIBS_BUILT}")
  endif()
else()
  if(YAML_CPP_SHARED_LIBS_BUILT)
    message(FATAL_ERROR "Library type (${LIBRARY_TYPE}) contradicts config: ${YAML_CPP_SHARED_LIBS_BUILT}")
  endif()
endif()

add_executable(main main.cpp)
if (NOT DEFINED CMAKE_CXX_STANDARD)
  set_target_properties(main PROPERTIES CXX_STANDARD 11)
endif()
target_link_libraries(main PRIVATE ${YAML_CPP_LIBRARIES})

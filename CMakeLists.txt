cmake_minimum_required(VERSION 3.10)
project(iot_monitor CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(iot_monitor main.cpp)
# optional: doc target if Doxygen available
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generando documentación con Doxygen"
  )
endif()

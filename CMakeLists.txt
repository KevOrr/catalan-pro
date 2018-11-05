cmake_minimum_required (VERSION 2.6)
project (CatalanPro)

include_directories(include)
file(GLOB SOURCES "src/*.c")

set(PROJECT_LINK_LIBS libgmp.so)

add_executable(catalanpro ${SOURCES})
target_link_libraries(catalanpro ${PROJECT_LINK_LIBS})

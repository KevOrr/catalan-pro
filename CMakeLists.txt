cmake_minimum_required (VERSION 2.6)
project (CatalanPro)

add_definitions(-std=c++11)

include(CheckIncludeFiles)
include(CheckLibraryExists)

# CHECK HEADERS
CHECK_INCLUDE_FILES(gmpxx.h HAVE_GMP_H)
if (NOT ${HAVE_GMP_H})
  message(FATAL_ERROR "Could not find gmpxx.h")
endif()

CHECK_INCLUDE_FILES(pthread.h HAVE_PTHREAD_H)
if (NOT ${HAVE_PTHREAD_H})
  message(FATAL_ERROR "Could not find pthread.h")
endif()

# CHECK LIBRARIES
CHECK_LIBRARY_EXISTS(gmp mpz_set "" HAVE_GMP)
if (NOT ${HAVE_GMP})
  message(FATAL_ERROR "Could not link against libgmp")
endif()

CHECK_LIBRARY_EXISTS(pthread pthread_create "" HAVE_PTHREAD)
if (NOT ${HAVE_PTHREAD})
  message(FATAL_ERROR "Could not link against libpthread")
endif()

include_directories(include)
file(GLOB SOURCES "src/*.c" "src/*.cpp")

set(PROJECT_LINK_LIBS libgmp.so libgmpxx.so libpthread.so)

add_executable(catalanpro ${SOURCES})
target_link_libraries(catalanpro ${PROJECT_LINK_LIBS})

cmake_minimum_required(VERSION 3.1)

project(libtwolame VERSION 0.5 LANGUAGES C)

option(BUILD_SHARED_LIBS "Build dynamic library" ON)

add_library(libtwolame
  ath.c
  availbits.c
  bitbuffer.c
  crc.c
  dab.c
  encode.c
  energy.c
  fft.c
  get_set.c
  mem.c
  psycho_0.c
  psycho_1.c
  psycho_2.c
  psycho_3.c
  psycho_4.c
  psycho_n1.c
  subband.c
  twolame.c
  util.c
)
target_include_directories(libtwolame PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(libtwolame PROPERTIES
  SOVERSION ${CMAKE_PROJECT_VERSION}
  VERSION ${CMAKE_PROJECT_VERSION}
  OUTPUT_NAME twolame
)

target_compile_definitions(libtwolame PRIVATE CMAKE)
target_compile_definitions(libtwolame PRIVATE PACKAGE_VERSION="${CMAKE_PROJECT_VERSION}")
target_compile_definitions(libtwolame PRIVATE PACKAGE_BUGREPORT="support")
target_compile_definitions(libtwolame PRIVATE PACKAGE_URL="https://twolame.org/")

include(CheckIncludeFile)
check_include_file(stdlib.h HAVE_STDLIB_H)
if(HAVE_STDLIB_H)
  target_compile_definitions(libtwolame PRIVATE HAVE_STDLIB_H)
endif()

check_include_file(string.h HAVE_STRING_H)
if(HAVE_STRING_H)
  target_compile_definitions(libtwolame PRIVATE HAVE_STRING_H)
endif()

check_include_file(malloc.h HAVE_MALLOC_H)
if(HAVE_MALLOC_H)
  target_compile_definitions(libtwolame PRIVATE HAVE_MALLOC_H)
endif()

if(WIN32)
  if(BUILD_SHARED_LIBS)
    target_compile_definitions(libtwolame PRIVATE LIBTWOLAME_BUILD DLL_EXPORT)
  else()
    target_compile_definitions(libtwolame PUBLIC LIBTWOLAME_STATIC)
  endif()
endif()

if(NOT WIN32)
  target_link_libraries(libtwolame PRIVATE m)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Library
install(TARGETS libtwolame
  EXPORT libtwolameTargets
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(FILES twolame.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# pkgconfig
configure_file(${CMAKE_SOURCE_DIR}/twolame.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/twolame.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/twolame.pc DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

# CMake config
set(TWOLAME_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/libtwolame")
install(
  EXPORT libtwolameTargets
  FILE libtwolameTargets.cmake
  NAMESPACE libtwolame::
  DESTINATION "${TWOLAME_INSTALL_CMAKEDIR}"
)
configure_package_config_file(libtwolameConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/libtwolameConfig.cmake"
  INSTALL_DESTINATION "${TWOLAME_INSTALL_CMAKEDIR}"
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/libtwolameConfigVersion.cmake"
  VERSION "${CMAKE_PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libtwolameConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/libtwolameConfigVersion.cmake"
  DESTINATION "${TWOLAME_INSTALL_CMAKEDIR}"
)

cmake_minimum_required(VERSION 3.1)
project(TwoLAME-frontend LANGUAGES C)

add_executable(TwoLAME-frontend frontend.c)
target_link_libraries(TwoLAME-frontend PRIVATE libtwolame SndFile::SndFile)
set_target_properties(TwoLAME-frontend PROPERTIES OUTPUT_NAME twolame)

include(GNUInstallDirs)
install(TARGETS TwoLAME-frontend DESTINATION "${CMAKE_INSTALL_BINDIR}")

cmake_minimum_required(VERSION 3.1)
project(TwoLAME-simplefrontend LANGUAGES C)

add_executable(TwoLAME-simplefrontend simplefrontend.c audio_wave.c)
target_link_libraries(TwoLAME-simplefrontend PRIVATE libtwolame)
set_target_properties(TwoLAME-simplefrontend PROPERTIES OUTPUT_NAME stwolame)

include(GNUInstallDirs)
install(TARGETS TwoLAME-simplefrontend DESTINATION "${CMAKE_INSTALL_BINDIR}")

cmake_minimum_required(VERSION 3.28)
project(PhantomMaskVGPTest LANGUAGES CXX ASM)

include(SymbianApp)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  assets/PhantomMask_SH.bin assets/MaskMain.pvr assets/RoomStill.pvr)

symbian_add_executable(phantommask
  app.cc
  application.cc
  assets.S)
target_include_directories(phantommask PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(phantommask PRIVATE
  Symbian::GlesDisplay
  Symbian::WindowServer
  Symbian::Gdi
  Symbian::Math
  Symbian::Hal)
symbian_publish_executable(phantommask UID3 0xe0000836)

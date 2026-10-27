cmake_minimum_required(VERSION 3.28)
project(PhantomMaskVGPTest LANGUAGES CXX ASM)

include(SymbianApp)

symbian_add_executable(phantom_vgp_test app.cc application.cc)
target_include_directories(phantom_vgp_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(phantom_vgp_test PRIVATE
  Symbian::GlesDisplay
  Symbian::GLES1
  Symbian::EGL
  Symbian::WindowServer
  Symbian::Gdi
  Symbian::Math
  Symbian::Hal)
symbian_publish_executable(phantom_vgp_test UID3 0xe0000836)

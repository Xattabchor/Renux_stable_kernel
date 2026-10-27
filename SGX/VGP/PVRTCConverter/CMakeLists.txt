cmake_minimum_required(VERSION 3.28)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(PVRTCConverter LANGUAGES C CXX ASM)
endif()

include(SymbianApp)

set(PVRTC_REFERENCE_DIR "${CMAKE_CURRENT_LIST_DIR}/third_party/pvrtc")

symbian_add_executable(pvrtc_converter
  app.cc
  converter.cc
  application.cc
  "${PVRTC_REFERENCE_DIR}/PVRTDecompress.cpp")
target_include_directories(pvrtc_converter PRIVATE "${PVRTC_REFERENCE_DIR}")
target_link_libraries(pvrtc_converter PRIVATE
  Symbian::PortablePng
  Symbian::WindowServer
  Symbian::Gdi
  Symbian::FileServer)
symbian_publish_executable(pvrtc_converter UID3 0xe0000840)

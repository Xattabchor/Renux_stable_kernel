cmake_minimum_required(VERSION 3.28)
project(GlesExtensionProbe LANGUAGES CXX ASM)

include(SymbianApp)

symbian_add_executable(glesextprobe
  app.cc
  application.cc)
target_link_libraries(glesextprobe PRIVATE
  Symbian::GlesDisplay
  Symbian::WindowServer
  Symbian::Gdi
  Symbian::Math
  Symbian::Hal)
symbian_publish_executable(glesextprobe UID3 0xe0000838)

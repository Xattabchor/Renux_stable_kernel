cmake_minimum_required(VERSION 3.28)
project(PolyBumpN95 LANGUAGES CXX ASM)

include(SymbianApp)

symbian_add_executable(polybumpn95
  app.cc
  application.cc)
target_compile_features(polybumpn95 PRIVATE cxx_std_17)
target_link_libraries(polybumpn95 PRIVATE
  Symbian::GlesDisplay
  Symbian::WindowServer
  Symbian::Gdi
  Symbian::Math
  Symbian::Hal)
symbian_publish_executable(polybumpn95 UID3 0xe0000842)

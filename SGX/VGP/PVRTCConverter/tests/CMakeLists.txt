cmake_minimum_required(VERSION 3.16)
project(PVRTCConverterHostTests LANGUAGES CXX)

find_package(PNG REQUIRED)

set(PVRTC_REFERENCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/pvrtc")

add_executable(pvrtc_converter_tests
  "${CMAKE_CURRENT_LIST_DIR}/../converter.cc"
  "${CMAKE_CURRENT_LIST_DIR}/../tests.cc"
  "${PVRTC_REFERENCE_DIR}/PVRTDecompress.cpp")
target_include_directories(pvrtc_converter_tests PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/.."
  "${PVRTC_REFERENCE_DIR}")
target_link_libraries(pvrtc_converter_tests PRIVATE PNG::PNG)
target_compile_features(pvrtc_converter_tests PRIVATE cxx_std_17)
enable_testing()
add_test(NAME pvrtc_converter_roundtrip COMMAND pvrtc_converter_tests)

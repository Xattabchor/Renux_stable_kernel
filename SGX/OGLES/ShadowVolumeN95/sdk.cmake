# The SDK path is relative to the project directory to keep local builds portable.
set(symbian_project_directory "${CMAKE_CURRENT_LIST_DIR}")
file(READ "${symbian_project_directory}/sdk-location.json" symbian_sdk_selection)
string(JSON symbian_sdk_prefix GET "${symbian_sdk_selection}" sdk)
get_filename_component(symbian_sdk_prefix "${symbian_sdk_prefix}" ABSOLUTE
  BASE_DIR "${symbian_project_directory}")
set(SYMBIAN_SDK_PREFIX "${symbian_sdk_prefix}")
include("${SYMBIAN_SDK_PREFIX}/cmake/symbian-arm.cmake")

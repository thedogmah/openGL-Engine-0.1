#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LinearMath" for configuration "Release"
set_property(TARGET LinearMath APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LinearMath PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/LinearMath.lib"
  )

list(APPEND _cmake_import_check_targets LinearMath )
list(APPEND _cmake_import_check_files_for_LinearMath "${_IMPORT_PREFIX}/lib/LinearMath.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "flexspi_nor_release")
  file(REMOVE_RECURSE
  "clean_files-NOTFOUND"
  "riop_EIP_M33LEADER_cm33.bin"
  )
endif()

# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/../M7FOLLOWER")
  file(MAKE_DIRECTORY "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/../M7FOLLOWER")
endif()
file(MAKE_DIRECTORY
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/riop_EIP_M7FOLLOWER"
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/riop_EIP_M7FOLLOWER-prefix"
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/riop_EIP_M7FOLLOWER-prefix/tmp"
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/riop_EIP_M7FOLLOWER-prefix/src/riop_EIP_M7FOLLOWER-stamp"
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/riop_EIP_M7FOLLOWER-prefix/src"
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/riop_EIP_M7FOLLOWER-prefix/src/riop_EIP_M7FOLLOWER-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/riop_EIP_M7FOLLOWER-prefix/src/riop_EIP_M7FOLLOWER-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/riop_EIP_M7FOLLOWER-prefix/src/riop_EIP_M7FOLLOWER-stamp${cfgdir}") # cfgdir has leading slash
endif()

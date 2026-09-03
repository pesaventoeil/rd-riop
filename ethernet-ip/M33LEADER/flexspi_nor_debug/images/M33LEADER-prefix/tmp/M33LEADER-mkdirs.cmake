# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER")
  file(MAKE_DIRECTORY "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER")
endif()
file(MAKE_DIRECTORY
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/M33LEADER"
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/images/M33LEADER-prefix"
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/images/M33LEADER-prefix/tmp"
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/images/M33LEADER-prefix/src/M33LEADER-stamp"
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/images/M33LEADER-prefix/src"
  "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/images/M33LEADER-prefix/src/M33LEADER-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/images/M33LEADER-prefix/src/M33LEADER-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/dpesavento.EIL/Documents/GitHub/daniel-sandbox/rd-riop-EIP-demo/ethernet-ip/M33LEADER/flexspi_nor_debug/images/M33LEADER-prefix/src/M33LEADER-stamp${cfgdir}") # cfgdir has leading slash
endif()

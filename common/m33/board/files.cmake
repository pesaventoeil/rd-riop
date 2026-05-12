#
# Copyright 2025 NXP
#
# SPDX-License-Identifier: Apache-2.0
#

# board soures
mcux_add_source(
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    SOURCES
    board.c
    board.h
    pin_mux.c
    pin_mux.h
    clock_config.c
    clock_config.h
)

mcux_add_include(
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    INCLUDES
    .
)


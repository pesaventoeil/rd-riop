#
# Copyright 2025-2026 NXP
#
# SPDX-License-Identifier: Apache-2.0
#

if(NOT RIOP_SKIP_BOARD_FILES)

# board soures
mcux_add_source(
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    SOURCES
    board/board.c
    board/board.h
    board/pin_mux.c
    board/pin_mux.h
    board/clock_config.c
    board/clock_config.h
)

mcux_add_include(
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    INCLUDES
    board
)

mcux_add_configuration(
    TARGETS debug release
    AS " -DRIOP=1 "
    CC " -DRIOP=1 "
)

endif() # NOT RIOP_SKIP_BOARD_FILES

# riop sources
mcux_add_source(
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    SOURCES
    source/afe_task/afe_task.c
    source/afe_task/afe_task.h
    source/api_icc.c
    source/api_icc.h
    source/api_afe.c
    source/api_afe.h
    source/api_riop.c
    source/api_riop.h
    source/api_riop_common.h
    source/api_siggen.c
    source/api_siggen.h
    source/digital_io.c
    source/gl.h
    source/gpio_task/gpio_task.c
    source/gpio_task/gpio_task.h
    source/icc_task/icc_task.c
    source/icc_task/icc_task.h
    source/riop_feature_config.h
    source/init.c
    source/SIGGEN_task/SIGGEN_task.c
    source/SIGGEN_task/SIGGEN_task.h
    NAFE_hw/edma_permission.c
    NAFE_hw/edma_permission.h
    NAFE_hw/nafe13388.c
    NAFE_hw/nafe13388.h
    NAFE_hw/nafe_hal.c
    NAFE_hw/nafe_hal.h
)

mcux_add_include(
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    INCLUDES
    source
    source/afe_task
    source/gpio_task
    source/icc_task
    source/SIGGEN_task
    NAFE_hw
)


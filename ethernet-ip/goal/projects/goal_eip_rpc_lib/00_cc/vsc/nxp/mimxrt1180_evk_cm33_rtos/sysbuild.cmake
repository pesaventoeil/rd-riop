
# Copyright 2025 NXP
#
# SPDX-License-Identifier: Apache-2.0



ExternalZephyrProject_Add(
    APPLICATION RT1180_CM7
    SOURCE_DIR ${APP_DIR}/../../../../../../projects/goal_eip_rpc/opener/10_led_button/vsc/nxp/mimxrt1180_evk_cm7_rtos
    board ${SB_CONFIG_secondary_board}
    core_id ${SB_CONFIG_secondary_core_id}
    config ${SB_CONFIG_secondary_config}
    toolchain ${SB_CONFIG_secondary_toolchain}
)

add_dependencies(${DEFAULT_IMAGE} RT1180_CM7)
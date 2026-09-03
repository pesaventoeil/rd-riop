
# Copyright 2025 NXP
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.22.0)

include(
    ${SdkRootDirPath}/cmake/extension/mcux.cmake
)

project(rt1180_cm7 LANGUAGES C ASM)


include(
    ${SdkRootDirPath}/CMakeLists.txt
)

mcux_add_source(
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    SOURCES
    ../../../../../../../appl/goal_eip/10_led_button/goal_app_eip.h
    ../../../../../../../appl/goal_eip/10_led_button/goal_appl.c
    ../../../../../../../appl/goal_eip/10_led_button/goal_config.h
    ../../../../../../../ext/psnprintf/psnprintf.c
    ../../../../../../../ext/psnprintf/psnprintf.h
    ../../../../../../../ext/uthash/utlist.h
    ../../../../../../../goal/cm/goal_cm_cm.c
    ../../../../../../../goal/cm/goal_cm_cm.h
    ../../../../../../../goal/cm/goal_eth_cm.c
    ../../../../../../../goal/cm/goal_eth_cm.h
    ../../../../../../../goal/cm/goal_lm_cm.c
    ../../../../../../../goal/cm/goal_lm_cm.h
    ../../../../../../../goal/cm/goal_queue_cm.c
    ../../../../../../../goal/cm/goal_queue_cm.h
    ../../../../../../../goal/goal_alloc.c
    ../../../../../../../goal/goal_alloc.h
    ../../../../../../../goal/goal_bm.c
    ../../../../../../../goal/goal_bm.h
    ../../../../../../../goal/goal_bus.h
    ../../../../../../../goal/goal_cm.c
    ../../../../../../../goal/goal_cm.h
    ../../../../../../../goal/goal_cm_id.h
    ../../../../../../../goal/goal_cm_t.h
    ../../../../../../../goal/goal_config_def.h
    ../../../../../../../goal/goal_id.h
    ../../../../../../../goal/goal_includes.h
    ../../../../../../../goal/goal_inst.c
    ../../../../../../../goal/goal_inst.h
    ../../../../../../../goal/goal_lm.c
    ../../../../../../../goal/goal_lm.h
    ../../../../../../../goal/goal_lock.c
    ../../../../../../../goal/goal_lock.h
    ../../../../../../../goal/goal_log.c
    ../../../../../../../goal/goal_log.h
    ../../../../../../../goal/goal_main.c
    ../../../../../../../goal/goal_main.h
    ../../../../../../../goal/goal_mbox.c
    ../../../../../../../goal/goal_mbox.h
    ../../../../../../../goal/goal_queue.c
    ../../../../../../../goal/goal_queue.h
    ../../../../../../../goal/goal_rb.c
    ../../../../../../../goal/goal_rb.h
    ../../../../../../../goal/goal_reg.c
    ../../../../../../../goal/goal_reg.h
    ../../../../../../../goal/goal_stat.c
    ../../../../../../../goal/goal_stat.h
    ../../../../../../../goal/goal_target_api.h
    ../../../../../../../goal/goal_task.c
    ../../../../../../../goal/goal_task.h
    ../../../../../../../goal/goal_timer.c
    ../../../../../../../goal/goal_timer.h
    ../../../../../../../goal/goal_types.h
    ../../../../../../../goal/goal_util.c
    ../../../../../../../goal/goal_util.h
    ../../../../../../../goal_media/cm/goal_mi_mctc_cm.c
    ../../../../../../../goal_media/cm/goal_mi_mctc_cm.h
    ../../../../../../../goal_media/goal_ma_heap.c
    ../../../../../../../goal_media/goal_ma_heap.h
    ../../../../../../../goal_media/goal_mi_dm.c
    ../../../../../../../goal_media/goal_mi_dm.h
    ../../../../../../../goal_media/goal_mi_mctc.c
    ../../../../../../../goal_media/goal_mi_mctc.h
    ../../../../../../../goal_media/goal_mi_mctc_ram.c
    ../../../../../../../goal_media/goal_mi_mctc_ram.h
    ../../../../../../../plat/arch/common/goal_arch_common.h
    ../../../../../../../plat/arch/common/goal_target_default.h
    ../../../../../../../plat/arch/freertos/goal_target.c
    ../../../../../../../plat/arch/freertos/goal_target_ext.h
    ../../../../../../../plat/arch/rt1180/goal_target.h
    ../../../../../../../plat/arch/rt1180/goal_target_common.c
    ../../../../../../../plat/arch/rt1180/goal_target_common.h
    ../../../../../../../plat/arch/rt1180/goal_target_types.h
    ../../../../../../../plat/board/nxp/evkmimxrt1180/goal_target_board.c
    ../../../../../../../plat/board/nxp/evkmimxrt1180/goal_target_board.h
    ../../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board.c
    ../../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board.h
    ../../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board/clock_config.c
    ../../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board/clock_config.h
    ../../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board/pin_mux.c
    ../../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board/pin_mux.h
    ../../../../../../../plat/drv/mem/heap/freertos/mem_heap_freertos.c
    ../../../../../../../plat/drv/mem/heap/freertos/mem_heap_freertos.h
    ../../../../../../../protos/dd/goal_dd.h
    ../../../../../../../protos/dd/rpc/goal_dd_rpc.h
    ../../../../../../../protos/dd/rpc/goal_dd_rpc_ac.c
    ../../../../../../../protos/dd/rpc/goal_dd_rpc_ac.h
    ../../../../../../../protos/goal_mctc/plat/goal/goal_mctc_plat.h
    ../../../../../../../protos/goal_mctc/src/goal_mctc.c
    ../../../../../../../protos/goal_mctc/src/goal_mctc.h
    ../../../../../../../protos/goal_mctc/src/goal_mctc_rb.c
    ../../../../../../../protos/goal_mctc/src/goal_mctc_rb.h
    ../../../../../../../protos/lm_emit/goal_lm_emit.c
    ../../../../../../../protos/lm_emit/goal_lm_emit.h
    ../../../../../../../protos/lm_emit/goal_lm_emit_raw.c
    ../../../../../../../protos/lm_emit/goal_lm_emit_raw.h
    ../../../../../../../protos/opener/src/ports/goal/wrapper/ac/goal_ciptypes.h
    ../../../../../../../protos/opener/src/ports/goal/wrapper/ac/goal_eip.h
    ../../../../../../../protos/opener/src/ports/goal/wrapper/ac/goal_eip_rpc.h
    ../../../../../../../protos/opener/src/ports/goal/wrapper/ac/goal_eip_rpc_ac.c
)

mcux_add_include(
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    INCLUDES
    ../../../../../../../.
    ../../../../../../../appl/goal_eip/10_led_button
    ../../../../../../../ext/psnprintf
    ../../../../../../../ext/uthash
    ../../../../../../../goal
    ../../../../../../../plat
    ../../../../../../../plat/arch/freertos
    ../../../../../../../plat/arch/rt1180
    ../../../../../../../plat/board/nxp/evkmimxrt1180
    ../../../../../../../plat/board/nxp/evkmimxrt1180/vendor
    ../../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board
    ../../../../../../../plat/drv/nvs/sflash/fsl/rt1180
    ../../../../../../../plat/drv/nvs/sflash/fsl/rt1180/vendor
    ../../../../../../../protos/dd
    ../../../../../../../protos/dd/rpc
    ../../../../../../../protos/goal_mctc/plat/goal
    ../../../../../../../protos/goal_mctc/src
    ../../../../../../../protos/opener/src/ports/goal/wrapper/ac
)

mcux_add_configuration(
    TARGETS debug
    # naming no TARGETS will add the elements to all build TARGETs
    # Assembler-Compiler flags
    AS " \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -D__STARTUP_INITIALIZE_QADATA \
    -D__STARTUP_INITIALIZE_RAMFUNCTION \
    -D__STDC_FORMAT_MACROS \
    -D__USE_SHMEM \
    -DCPU_MIMXRT1189CVM8C_cm7 \
    -DGOAL=1 \
    -DGOAL_CONFIG_EIP=1 \
    -DGOAL_CONFIG_FEAT_UTHASH=1 \
    -DGOAL_CONFIG_GEN_CTC=1 \
    -DGOAL_CONFIG_GEN_CTC_AC=1 \
    -DGOAL_CONFIG_GEN_RPC=1 \
    -DGOAL_CONFIG_LM_EMIT=1 \
    -DGOAL_CONFIG_MCTC=1 \
    -DGOAL_CONFIG_MCTC_AC=1 \
    -DGOAL_CONFIG_MCTC_RAM=1 \
    -DGOAL_CONFIG_MCTC_RPC_DATA_LENGTH=2 \
    -DGOAL_CONFIG_MEDIA_MI_DM=1 \
    -DGOAL_CONFIG_MM_EXT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_TICK_FROM_HOOK=1 \
    -DGOAL_CONFIG_TASK=1 \
    -DGOAL_MI_DM_CFG_LOCAL=1 \
    -DMCUX_META_BUILD \
    -DMCUXPRESSO_SDK \
    -DMIMXRT1189_cm7_SERIES \
    -DNDEBUG \
    -DPRINTF_ADVANCED_ENABLE \
    -DSDK_OS_FREE_RTOS \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DUSE_RTOS \
    -DXIP_BOOT_HEADER_ENABLE=0 \
    -DXIP_EXTERNAL_FLASH=1 \
    -fdata-sections \
    -ffreestanding \
    -ffunction-sections \
    -fno-builtin \
    -fno-common \
    -mapcs \
    -mcpu=cortex-m33 \
    -mcpu=cortex-m7 \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    -MMD \
    -MP \
    -mthumb \
    -O2 \
    -Wno-unused-parameter \
    -DDEBUG \
    -g \
    -O0 \
    "
    # C-Compiler flags
    CC " \
    -D__STARTUP_CLEAR_BSS \
    -D__STDC_FORMAT_MACROS \
    -D__USE_SHMEM \
    -DCPU_MIMXRT1189CVM8C_cm7 \
    -DGOAL=1 \
    -DGOAL_CONFIG_EIP=1 \
    -DGOAL_CONFIG_FEAT_UTHASH=1 \
    -DGOAL_CONFIG_GEN_CTC=1 \
    -DGOAL_CONFIG_GEN_CTC_AC=1 \
    -DGOAL_CONFIG_GEN_RPC=1 \
    -DGOAL_CONFIG_LM_EMIT=1 \
    -DGOAL_CONFIG_MCTC=1 \
    -DGOAL_CONFIG_MCTC_AC=1 \
    -DGOAL_CONFIG_MCTC_RAM=1 \
    -DGOAL_CONFIG_MCTC_RPC_DATA_LENGTH=2 \
    -DGOAL_CONFIG_MEDIA_MI_DM=1 \
    -DGOAL_CONFIG_MM_EXT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_TICK_FROM_HOOK=1 \
    -DGOAL_CONFIG_TASK=1 \
    -DGOAL_MI_DM_CFG_LOCAL=1 \
    -DMCUX_META_BUILD \
    -DMCUXPRESSO_SDK \
    -DMIMXRT1189_cm7_SERIES \
    -DNDEBUG \
    -DPRINTF_ADVANCED_ENABLE \
    -DSDK_OS_FREE_RTOS \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DUSE_RTOS \
    -DXIP_BOOT_HEADER_ENABLE=0 \
    -DXIP_EXTERNAL_FLASH=1 \
    -fdata-sections \
    -ffreestanding \
    -ffunction-sections \
    -fno-builtin \
    -fno-common \
    -mapcs \
    -mcpu=cortex-m7 \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    -MMD \
    -MP \
    -mthumb \
    -O2 \
    -std=c99 \
    -DDEBUG \
    -g \
    -O0 \
    "
)

mcux_add_configuration(
    TARGETS release
    # naming no TARGETS will add the elements to all build TARGETs
    # Assembler-Compiler flags
    AS " \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -D__STARTUP_INITIALIZE_QADATA \
    -D__STARTUP_INITIALIZE_RAMFUNCTION \
    -D__STDC_FORMAT_MACROS \
    -D__USE_SHMEM \
    -DCPU_MIMXRT1189CVM8C_cm7 \
    -DGOAL=1 \
    -DGOAL_CONFIG_EIP=1 \
    -DGOAL_CONFIG_FEAT_UTHASH=1 \
    -DGOAL_CONFIG_GEN_CTC=1 \
    -DGOAL_CONFIG_GEN_CTC_AC=1 \
    -DGOAL_CONFIG_GEN_RPC=1 \
    -DGOAL_CONFIG_LM_EMIT=1 \
    -DGOAL_CONFIG_MCTC=1 \
    -DGOAL_CONFIG_MCTC_AC=1 \
    -DGOAL_CONFIG_MCTC_RAM=1 \
    -DGOAL_CONFIG_MCTC_RPC_DATA_LENGTH=2 \
    -DGOAL_CONFIG_MEDIA_MI_DM=1 \
    -DGOAL_CONFIG_MM_EXT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_TICK_FROM_HOOK=1 \
    -DGOAL_CONFIG_TASK=1 \
    -DGOAL_MI_DM_CFG_LOCAL=1 \
    -DMCUX_META_BUILD \
    -DMCUXPRESSO_SDK \
    -DMIMXRT1189_cm7_SERIES \
    -DNDEBUG \
    -DPRINTF_ADVANCED_ENABLE \
    -DSDK_OS_FREE_RTOS \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DUSE_RTOS \
    -DXIP_BOOT_HEADER_ENABLE=0 \
    -DXIP_EXTERNAL_FLASH=1 \
    -fdata-sections \
    -ffreestanding \
    -ffunction-sections \
    -fno-builtin \
    -fno-common \
    -mapcs \
    -mcpu=cortex-m33 \
    -mcpu=cortex-m7 \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    -MMD \
    -MP \
    -mthumb \
    -O2 \
    -Wno-unused-parameter \
    "
    # C-Compiler flags
    CC " \
    -D__STARTUP_CLEAR_BSS \
    -D__STDC_FORMAT_MACROS \
    -D__USE_SHMEM \
    -DCPU_MIMXRT1189CVM8C_cm7 \
    -DGOAL=1 \
    -DGOAL_CONFIG_EIP=1 \
    -DGOAL_CONFIG_FEAT_UTHASH=1 \
    -DGOAL_CONFIG_GEN_CTC=1 \
    -DGOAL_CONFIG_GEN_CTC_AC=1 \
    -DGOAL_CONFIG_GEN_RPC=1 \
    -DGOAL_CONFIG_LM_EMIT=1 \
    -DGOAL_CONFIG_MCTC=1 \
    -DGOAL_CONFIG_MCTC_AC=1 \
    -DGOAL_CONFIG_MCTC_RAM=1 \
    -DGOAL_CONFIG_MCTC_RPC_DATA_LENGTH=2 \
    -DGOAL_CONFIG_MEDIA_MI_DM=1 \
    -DGOAL_CONFIG_MM_EXT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_TICK_FROM_HOOK=1 \
    -DGOAL_CONFIG_TASK=1 \
    -DGOAL_MI_DM_CFG_LOCAL=1 \
    -DMCUX_META_BUILD \
    -DMCUXPRESSO_SDK \
    -DMIMXRT1189_cm7_SERIES \
    -DNDEBUG \
    -DPRINTF_ADVANCED_ENABLE \
    -DSDK_OS_FREE_RTOS \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DUSE_RTOS \
    -DXIP_BOOT_HEADER_ENABLE=0 \
    -DXIP_EXTERNAL_FLASH=1 \
    -fdata-sections \
    -ffreestanding \
    -ffunction-sections \
    -fno-builtin \
    -fno-common \
    -mapcs \
    -mcpu=cortex-m7 \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    -MMD \
    -MP \
    -mthumb \
    -O2 \
    -std=c99 \
    "
)
# Linker flags for shared memory
mcux_add_linker_symbol(
    TARGETS debug release
    SYMBOLS " \
    __use_shmem__=1 \
    __multicore__=1 \
    "
)

mcux_convert_binary(
    TOOLCHAINS armgcc
    BINARY ${APPLICATION_BINARY_DIR}/${CONFIG_TOOLCHAIN}/core1_image.bin
)

# replace linker script for debug configuration
mcux_remove_armgcc_linker_script(
    TARGETS debug release
    BASE_PATH ${SdkRootDirPath}
    LINKER ${device_root}/RT/RT1180/MIMXRT1189/gcc/MIMXRT1189xxxxx_cm7_ram.ld
)

mcux_add_armgcc_linker_script(
    TARGETS debug release
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    LINKER armgcc/MIMXRT1189xxxxx_cm7_ram.ld
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
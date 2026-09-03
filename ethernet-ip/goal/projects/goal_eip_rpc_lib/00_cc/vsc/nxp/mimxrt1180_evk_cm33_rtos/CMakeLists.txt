
# Copyright 2025 NXP
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.22.0)

include(
    ${SdkRootDirPath}/cmake/extension/mcux.cmake
)

project(rt1180_cm33 LANGUAGES C ASM)


include(
    ${SdkRootDirPath}/CMakeLists.txt
)

mcux_add_source(
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    SOURCES
    ../../../../../../appl/goal_eip/00_rpc_cc/dlr_config.h
    ../../../../../../appl/goal_eip/00_rpc_cc/eip_system_conf.h
    ../../../../../../appl/goal_eip/00_rpc_cc/eip_user_conf.h
    ../../../../../../appl/goal_eip/00_rpc_cc/goal_appl.c
    ../../../../../../appl/goal_eip/00_rpc_cc/goal_config.h
    ../../../../../../ext/lwip/src/core/def.c
    ../../../../../../ext/lwip/src/core/dns.c
    ../../../../../../ext/lwip/src/core/inet_chksum.c
    ../../../../../../ext/lwip/src/core/init.c
    ../../../../../../ext/lwip/src/core/ip.c
    ../../../../../../ext/lwip/src/core/ipv4/etharp.c
    ../../../../../../ext/lwip/src/core/ipv4/icmp.c
    ../../../../../../ext/lwip/src/core/ipv4/igmp.c
    ../../../../../../ext/lwip/src/core/ipv4/ip4.c
    ../../../../../../ext/lwip/src/core/ipv4/ip4_addr.c
    ../../../../../../ext/lwip/src/core/ipv4/ip4_frag.c
    ../../../../../../ext/lwip/src/core/mem.c
    ../../../../../../ext/lwip/src/core/memp.c
    ../../../../../../ext/lwip/src/core/netif.c
    ../../../../../../ext/lwip/src/core/pbuf.c
    ../../../../../../ext/lwip/src/core/tcp.c
    ../../../../../../ext/lwip/src/core/tcp_in.c
    ../../../../../../ext/lwip/src/core/tcp_out.c
    ../../../../../../ext/lwip/src/core/timeouts.c
    ../../../../../../ext/lwip/src/core/udp.c
    ../../../../../../ext/lwip/src/include/lwip/altcp.h
    ../../../../../../ext/lwip/src/include/lwip/api.h
    ../../../../../../ext/lwip/src/include/lwip/arch.h
    ../../../../../../ext/lwip/src/include/lwip/autoip.h
    ../../../../../../ext/lwip/src/include/lwip/debug.h
    ../../../../../../ext/lwip/src/include/lwip/def.h
    ../../../../../../ext/lwip/src/include/lwip/dhcp.h
    ../../../../../../ext/lwip/src/include/lwip/dns.h
    ../../../../../../ext/lwip/src/include/lwip/err.h
    ../../../../../../ext/lwip/src/include/lwip/etharp.h
    ../../../../../../ext/lwip/src/include/lwip/icmp.h
    ../../../../../../ext/lwip/src/include/lwip/icmp6.h
    ../../../../../../ext/lwip/src/include/lwip/igmp.h
    ../../../../../../ext/lwip/src/include/lwip/inet_chksum.h
    ../../../../../../ext/lwip/src/include/lwip/init.h
    ../../../../../../ext/lwip/src/include/lwip/ip.h
    ../../../../../../ext/lwip/src/include/lwip/ip4.h
    ../../../../../../ext/lwip/src/include/lwip/ip4_addr.h
    ../../../../../../ext/lwip/src/include/lwip/ip4_frag.h
    ../../../../../../ext/lwip/src/include/lwip/ip6.h
    ../../../../../../ext/lwip/src/include/lwip/ip6_addr.h
    ../../../../../../ext/lwip/src/include/lwip/ip6_frag.h
    ../../../../../../ext/lwip/src/include/lwip/ip_addr.h
    ../../../../../../ext/lwip/src/include/lwip/mem.h
    ../../../../../../ext/lwip/src/include/lwip/memp.h
    ../../../../../../ext/lwip/src/include/lwip/mld6.h
    ../../../../../../ext/lwip/src/include/lwip/nd6.h
    ../../../../../../ext/lwip/src/include/lwip/netbuf.h
    ../../../../../../ext/lwip/src/include/lwip/netdb.h
    ../../../../../../ext/lwip/src/include/lwip/netif.h
    ../../../../../../ext/lwip/src/include/lwip/netifapi.h
    ../../../../../../ext/lwip/src/include/lwip/opt.h
    ../../../../../../ext/lwip/src/include/lwip/pbuf.h
    ../../../../../../ext/lwip/src/include/lwip/priv/api_msg.h
    ../../../../../../ext/lwip/src/include/lwip/priv/memp_priv.h
    ../../../../../../ext/lwip/src/include/lwip/priv/memp_std.h
    ../../../../../../ext/lwip/src/include/lwip/priv/nd6_priv.h
    ../../../../../../ext/lwip/src/include/lwip/priv/sockets_priv.h
    ../../../../../../ext/lwip/src/include/lwip/priv/tcp_priv.h
    ../../../../../../ext/lwip/src/include/lwip/priv/tcpip_priv.h
    ../../../../../../ext/lwip/src/include/lwip/prot/dhcp.h
    ../../../../../../ext/lwip/src/include/lwip/prot/dns.h
    ../../../../../../ext/lwip/src/include/lwip/prot/etharp.h
    ../../../../../../ext/lwip/src/include/lwip/prot/ethernet.h
    ../../../../../../ext/lwip/src/include/lwip/prot/icmp.h
    ../../../../../../ext/lwip/src/include/lwip/prot/icmp6.h
    ../../../../../../ext/lwip/src/include/lwip/prot/igmp.h
    ../../../../../../ext/lwip/src/include/lwip/prot/ip.h
    ../../../../../../ext/lwip/src/include/lwip/prot/ip4.h
    ../../../../../../ext/lwip/src/include/lwip/prot/tcp.h
    ../../../../../../ext/lwip/src/include/lwip/prot/udp.h
    ../../../../../../ext/lwip/src/include/lwip/raw.h
    ../../../../../../ext/lwip/src/include/lwip/snmp.h
    ../../../../../../ext/lwip/src/include/lwip/sockets.h
    ../../../../../../ext/lwip/src/include/lwip/stats.h
    ../../../../../../ext/lwip/src/include/lwip/sys.h
    ../../../../../../ext/lwip/src/include/lwip/tcp.h
    ../../../../../../ext/lwip/src/include/lwip/tcpbase.h
    ../../../../../../ext/lwip/src/include/lwip/timeouts.h
    ../../../../../../ext/lwip/src/include/lwip/udp.h
    ../../../../../../ext/lwip/src/include/netif/etharp.h
    ../../../../../../ext/lwip/src/include/netif/ethernet.h
    ../../../../../../ext/lwip/src/include/netif/ppp/ppp_impl.h
    ../../../../../../ext/lwip/src/include/netif/ppp/ppp_opts.h
    ../../../../../../ext/lwip/src/netif/ethernet.c
    ../../../../../../ext/psnprintf/psnprintf.h
    ../../../../../../ext/uthash/utlist.h
    ../../../../../../goal/cm/goal_cm_cm.h
    ../../../../../../goal/cm/goal_eth_cm.h
    ../../../../../../goal/cm/goal_lm_cm.h
    ../../../../../../goal/cm/goal_net_cm.h
    ../../../../../../goal/cm/goal_queue_cm.h
    ../../../../../../goal/goal_alloc.h
    ../../../../../../goal/goal_bm.h
    ../../../../../../goal/goal_bus.h
    ../../../../../../goal/goal_cm.h
    ../../../../../../goal/goal_cm_id.h
    ../../../../../../goal/goal_cm_t.h
    ../../../../../../goal/goal_config_def.h
    ../../../../../../goal/goal_eth_names.h
    ../../../../../../goal/goal_id.h
    ../../../../../../goal/goal_includes.h
    ../../../../../../goal/goal_inst.h
    ../../../../../../goal/goal_lm.h
    ../../../../../../goal/goal_lock.h
    ../../../../../../goal/goal_log.h
    ../../../../../../goal/goal_main.h
    ../../../../../../goal/goal_mbox.h
    ../../../../../../goal/goal_net_chan.h
    ../../../../../../goal/goal_net_dhcp.h
    ../../../../../../goal/goal_queue.h
    ../../../../../../goal/goal_rb.h
    ../../../../../../goal/goal_reg.h
    ../../../../../../goal/goal_stat.h
    ../../../../../../goal/goal_target_api.h
    ../../../../../../goal/goal_task.h
    ../../../../../../goal/goal_timer.h
    ../../../../../../goal/goal_types.h
    ../../../../../../goal/goal_util.h
    ../../../../../../goal_media/goal_ma_eth.h
    ../../../../../../goal_media/goal_ma_heap.h
    ../../../../../../goal_media/goal_ma_net.h
    ../../../../../../goal_media/goal_mi_eth.h
    ../../../../../../goal_media/goal_mi_net.h
    ../../../../../../plat/arch/common/goal_arch_common.h
    ../../../../../../plat/arch/common/goal_target_default.h
    ../../../../../../plat/arch/freertos/goal_target.c
    ../../../../../../plat/arch/freertos/goal_target_ext.h
    ../../../../../../plat/arch/rt1180/goal_target.h
    ../../../../../../plat/arch/rt1180/goal_target_common.c
    ../../../../../../plat/arch/rt1180/goal_target_common.h
    ../../../../../../plat/arch/rt1180/goal_target_types.h
    ../../../../../../plat/board/nxp/evkmimxrt1180/goal_target_board.c
    ../../../../../../plat/board/nxp/evkmimxrt1180/goal_target_board.h
    ../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board.c
    ../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board.h
    ../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board/clock_config.c
    ../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board/clock_config.h
    ../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board/pin_mux.c
    ../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board/pin_mux.h
    ../../../../../../plat/drv/bus/mdio/netc/mdio.c
    ../../../../../../plat/drv/bus/mdio/netc/mdio.h
    ../../../../../../plat/drv/dlr/nxp_netc/dlr_netc.c
    ../../../../../../plat/drv/dlr/nxp_netc/dlr_netc.h
    ../../../../../../plat/drv/eth/nxp/netc/goal_target_eth.c
    ../../../../../../plat/drv/eth/nxp/netc/goal_target_eth.h
    ../../../../../../plat/drv/mem/heap/freertos/mem_heap_freertos.c
    ../../../../../../plat/drv/mem/heap/freertos/mem_heap_freertos.h
    ../../../../../../plat/drv/net/lwip/goal_target_net.c
    ../../../../../../plat/drv/net/lwip/vendor/arch/cc.h
    ../../../../../../plat/drv/net/lwip/vendor/lwipopts.h
    ../../../../../../plat/drv/nvs/sflash/fsl/rt1180/nvs_sflash_fsl.c
    ../../../../../../plat/drv/nvs/sflash/fsl/rt1180/vendor/flexspi_nor_flash_ops.c
    ../../../../../../plat/drv/phy/generic/phy_generic.c
    ../../../../../../plat/drv/phy/generic/phy_generic.h
    ../../../../../../plat/drv/phy/generic/phy_generic_int.h
    ../../../../../../plat/drv/phy/realtek_rtl8201/phy_rtl8201.c
    ../../../../../../plat/drv/phy/realtek_rtl8201/phy_rtl8201.h
    ../../../../../../plat/drv/phy/realtek_rtl8211/phy_rtl8211.c
    ../../../../../../plat/drv/phy/realtek_rtl8211/phy_rtl8211.h
    ../../../../../../plat/drv/ptp/goal_target_ptp.h
    ../../../../../../plat/drv/ptp/nxp/netc/goal_target_ptp.c
    ../../../../../../plat/drv/switch/nxp_netc/nxp_netc.c
    ../../../../../../plat/drv/switch/nxp_netc/nxp_netc_bsp.c
    ../../../../../../protos/acd/goal_acd.h
    ../../../../../../protos/dd/cm/goal_dd_cm.h
    ../../../../../../protos/dd/goal_dd.h
    ../../../../../../protos/dd/goal_dd_config.h
    ../../../../../../protos/dd/goal_dd_crypt.h
    ../../../../../../protos/dd/goal_dd_getconf.h
    ../../../../../../protos/dd/goal_dd_getlist.h
    ../../../../../../protos/dd/goal_dd_hello.h
    ../../../../../../protos/dd/goal_dd_packet.h
    ../../../../../../protos/dd/goal_dd_setconf.h
    ../../../../../../protos/dd/goal_dd_setip.h
    ../../../../../../protos/dd/goal_dd_wink.h
    ../../../../../../protos/dd/rpc/goal_dd_rpc.h
    ../../../../../../protos/dd/rpc/goal_dd_rpc_cc.h
    ../../../../../../protos/dlr/dlr/goal_target_dlr.h
    ../../../../../../protos/goal_lldp/lldp/goal_lldp.h
    ../../../../../../protos/opener/src/cip/cipassembly.h
    ../../../../../../protos/opener/src/cip/cipclass3connection.h
    ../../../../../../protos/opener/src/cip/cipcommon.h
    ../../../../../../protos/opener/src/cip/cipconnectionmanager.h
    ../../../../../../protos/opener/src/cip/cipdlr.h
    ../../../../../../protos/opener/src/cip/ciperror.h
    ../../../../../../protos/opener/src/cip/cipethernetlink.h
    ../../../../../../protos/opener/src/cip/cipidentity.h
    ../../../../../../protos/opener/src/cip/cipioconnection.h
    ../../../../../../protos/opener/src/cip/ciplldp.h
    ../../../../../../protos/opener/src/cip/cipmessagerouter.h
    ../../../../../../protos/opener/src/cip/cipqos.h
    ../../../../../../protos/opener/src/cip/ciptcpipinterface.h
    ../../../../../../protos/opener/src/eip_types.h
    ../../../../../../protos/opener/src/enet_encap/encap.h
    ../../../../../../protos/opener/src/enet_encap/endianconv.h
    ../../../../../../protos/opener/src/opener_api.h
    ../../../../../../protos/opener/src/opener_includes.h
    ../../../../../../protos/opener/src/ports/goal/cm/goal_eip_cm.h
    ../../../../../../protos/opener/src/ports/goal/opener_led.h
    ../../../../../../protos/opener/src/ports/goal/opener_target.h
    ../../../../../../protos/opener/src/ports/goal/opener_target_types.h
    ../../../../../../protos/opener/src/ports/goal/wrapper/cc/goal_eip_rpc.h
    ../../../../../../protos/opener/src/ports/goal/wrapper/goal_eip.h
)

mcux_add_include(
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    INCLUDES
    ../../../../../../.
    ../../../../../../appl/goal_eip/00_rpc_cc
    ../../../../../../ext/lwip/src/include
    ../../../../../../ext/psnprintf
    ../../../../../../ext/uthash
    ../../../../../../goal
    ../../../../../../plat
    ../../../../../../plat/arch/freertos
    ../../../../../../plat/arch/rt1180
    ../../../../../../plat/board/nxp/evkmimxrt1180
    ../../../../../../plat/board/nxp/evkmimxrt1180/vendor
    ../../../../../../plat/board/nxp/evkmimxrt1180/vendor/board
    ../../../../../../plat/drv/bus/mdio/netc
    ../../../../../../plat/drv/eth/nxp/netc
    ../../../../../../plat/drv/net/lwip
    ../../../../../../plat/drv/net/lwip/vendor
    ../../../../../../plat/drv/nvs/sflash/fsl/rt1180
    ../../../../../../plat/drv/nvs/sflash/fsl/rt1180/vendor
    ../../../../../../plat/drv/phy/realtek_rtl8201
    ../../../../../../plat/drv/phy/realtek_rtl8211
    ../../../../../../plat/drv/ptp
    ../../../../../../protos/acd
    ../../../../../../protos/dd
    ../../../../../../protos/dd/rpc
    ../../../../../../protos/dlr/dlr
    ../../../../../../protos/goal_lldp/lldp
    ../../../../../../protos/goal_mctc/plat/goal
    ../../../../../../protos/opener/src
    ../../../../../../protos/opener/src/cip
    ../../../../../../protos/opener/src/enet_encap
    ../../../../../../protos/opener/src/ports/goal
    ../../../../../../protos/opener/src/ports/goal/wrapper
    ../../../../../../protos/opener/src/ports/goal/wrapper/cc
)

mcux_add_configuration(
    TARGETS debug flexspi_nor_debug
    # naming no TARGETS will add the elements to all build TARGETs
    # Assembler-Compiler flags
    AS " \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -D__STARTUP_INITIALIZE_QADATA \
    -D__STARTUP_INITIALIZE_RAMFUNCTION \
    -D__STDC_FORMAT_MACROS \
    -D__USE_SHMEM \
    -DCPU_MIMXRT1189CVM8C_cm33 \
    -DDLR_CONFIG_BEACON_RINGNODE=1 \
    -DGOAL=1 \
    -DGOAL_CONFIG_ACD=1 \
    -DGOAL_CONFIG_DD=1 \
    -DGOAL_CONFIG_DHCP=1 \
    -DGOAL_CONFIG_EIP=1 \
    -DGOAL_CONFIG_ETH_STATS=1 \
    -DGOAL_CONFIG_ETHERNET=1 \
    -DGOAL_CONFIG_FEAT_UTHASH=1 \
    -DGOAL_CONFIG_GEN_CTC=1 \
    -DGOAL_CONFIG_GEN_CTC_CC=1 \
    -DGOAL_CONFIG_GEN_RPC=1 \
    -DGOAL_CONFIG_LIBRARY_APPL=1 \
    -DGOAL_CONFIG_LM_EMIT=1 \
    -DGOAL_CONFIG_MCTC=1 \
    -DGOAL_CONFIG_MCTC_CC=1 \
    -DGOAL_CONFIG_MCTC_RAM=1 \
    -DGOAL_CONFIG_MCTC_RPC_DATA_LENGTH=2 \
    -DGOAL_CONFIG_MEDIA_MA_NVS=1 \
    -DGOAL_CONFIG_MEDIA_MI_DM=1 \
    -DGOAL_CONFIG_MEDIA_MI_ETH=1 \
    -DGOAL_CONFIG_MEDIA_MI_NVS=1 \
    -DGOAL_CONFIG_MM_EXT=1 \
    -DGOAL_CONFIG_STATS=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_TICK_FROM_HOOK=1 \
    -DGOAL_CONFIG_TASK=1 \
    -DGOAL_CONFIG_TCPIP_STACK=1 \
    -DGOAL_CONFIG_TCPIP_TCP=1 \
    -DGOAL_DLR=1 \
    -DGOAL_FEAT_LWIP_INIT=1 \
    -DGOAL_MI_DM_CFG_LOCAL=1 \
    -DMCUX_META_BUILD \
    -DMCUXPRESSO_SDK \
    -DMIMXRT1189_cm33_SERIES \
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
    -DCPU_MIMXRT1189CVM8C_cm33 \
    -DDLR_CONFIG_BEACON_RINGNODE=1 \
    -DGOAL=1 \
    -DGOAL_CONFIG_ACD=1 \
    -DGOAL_CONFIG_DD=1 \
    -DGOAL_CONFIG_DHCP=1 \
    -DGOAL_CONFIG_EIP=1 \
    -DGOAL_CONFIG_ETH_STATS=1 \
    -DGOAL_CONFIG_ETHERNET=1 \
    -DGOAL_CONFIG_FEAT_UTHASH=1 \
    -DGOAL_CONFIG_GEN_CTC=1 \
    -DGOAL_CONFIG_GEN_CTC_CC=1 \
    -DGOAL_CONFIG_GEN_RPC=1 \
    -DGOAL_CONFIG_LIBRARY_APPL=1 \
    -DGOAL_CONFIG_LM_EMIT=1 \
    -DGOAL_CONFIG_MCTC=1 \
    -DGOAL_CONFIG_MCTC_CC=1 \
    -DGOAL_CONFIG_MCTC_RAM=1 \
    -DGOAL_CONFIG_MCTC_RPC_DATA_LENGTH=2 \
    -DGOAL_CONFIG_MEDIA_MA_NVS=1 \
    -DGOAL_CONFIG_MEDIA_MI_DM=1 \
    -DGOAL_CONFIG_MEDIA_MI_ETH=1 \
    -DGOAL_CONFIG_MEDIA_MI_NVS=1 \
    -DGOAL_CONFIG_MM_EXT=1 \
    -DGOAL_CONFIG_STATS=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_TICK_FROM_HOOK=1 \
    -DGOAL_CONFIG_TASK=1 \
    -DGOAL_CONFIG_TCPIP_STACK=1 \
    -DGOAL_CONFIG_TCPIP_TCP=1 \
    -DGOAL_DLR=1 \
    -DGOAL_FEAT_LWIP_INIT=1 \
    -DGOAL_MI_DM_CFG_LOCAL=1 \
    -DMCUX_META_BUILD \
    -DMCUXPRESSO_SDK \
    -DMIMXRT1189_cm33_SERIES \
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
    TARGETS release flexspi_nor_release
    # naming no TARGETS will add the elements to all build TARGETs
    # Assembler-Compiler flags
    AS " \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -D__STARTUP_INITIALIZE_QADATA \
    -D__STARTUP_INITIALIZE_RAMFUNCTION \
    -D__STDC_FORMAT_MACROS \
    -D__USE_SHMEM \
    -DCPU_MIMXRT1189CVM8C_cm33 \
    -DDLR_CONFIG_BEACON_RINGNODE=1 \
    -DGOAL=1 \
    -DGOAL_CONFIG_ACD=1 \
    -DGOAL_CONFIG_DD=1 \
    -DGOAL_CONFIG_DHCP=1 \
    -DGOAL_CONFIG_EIP=1 \
    -DGOAL_CONFIG_ETH_STATS=1 \
    -DGOAL_CONFIG_ETHERNET=1 \
    -DGOAL_CONFIG_FEAT_UTHASH=1 \
    -DGOAL_CONFIG_GEN_CTC=1 \
    -DGOAL_CONFIG_GEN_CTC_CC=1 \
    -DGOAL_CONFIG_GEN_RPC=1 \
    -DGOAL_CONFIG_LIBRARY_APPL=1 \
    -DGOAL_CONFIG_LM_EMIT=1 \
    -DGOAL_CONFIG_MCTC=1 \
    -DGOAL_CONFIG_MCTC_CC=1 \
    -DGOAL_CONFIG_MCTC_RAM=1 \
    -DGOAL_CONFIG_MCTC_RPC_DATA_LENGTH=2 \
    -DGOAL_CONFIG_MEDIA_MA_NVS=1 \
    -DGOAL_CONFIG_MEDIA_MI_DM=1 \
    -DGOAL_CONFIG_MEDIA_MI_ETH=1 \
    -DGOAL_CONFIG_MEDIA_MI_NVS=1 \
    -DGOAL_CONFIG_MM_EXT=1 \
    -DGOAL_CONFIG_STATS=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_TICK_FROM_HOOK=1 \
    -DGOAL_CONFIG_TASK=1 \
    -DGOAL_CONFIG_TCPIP_STACK=1 \
    -DGOAL_CONFIG_TCPIP_TCP=1 \
    -DGOAL_DLR=1 \
    -DGOAL_FEAT_LWIP_INIT=1 \
    -DGOAL_MI_DM_CFG_LOCAL=1 \
    -DMCUX_META_BUILD \
    -DMCUXPRESSO_SDK \
    -DMIMXRT1189_cm33_SERIES \
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
    -DCPU_MIMXRT1189CVM8C_cm33 \
    -DDLR_CONFIG_BEACON_RINGNODE=1 \
    -DGOAL=1 \
    -DGOAL_CONFIG_ACD=1 \
    -DGOAL_CONFIG_DD=1 \
    -DGOAL_CONFIG_DHCP=1 \
    -DGOAL_CONFIG_EIP=1 \
    -DGOAL_CONFIG_ETH_STATS=1 \
    -DGOAL_CONFIG_ETHERNET=1 \
    -DGOAL_CONFIG_FEAT_UTHASH=1 \
    -DGOAL_CONFIG_GEN_CTC=1 \
    -DGOAL_CONFIG_GEN_CTC_CC=1 \
    -DGOAL_CONFIG_GEN_RPC=1 \
    -DGOAL_CONFIG_LIBRARY_APPL=1 \
    -DGOAL_CONFIG_LM_EMIT=1 \
    -DGOAL_CONFIG_MCTC=1 \
    -DGOAL_CONFIG_MCTC_CC=1 \
    -DGOAL_CONFIG_MCTC_RAM=1 \
    -DGOAL_CONFIG_MCTC_RPC_DATA_LENGTH=2 \
    -DGOAL_CONFIG_MEDIA_MA_NVS=1 \
    -DGOAL_CONFIG_MEDIA_MI_DM=1 \
    -DGOAL_CONFIG_MEDIA_MI_ETH=1 \
    -DGOAL_CONFIG_MEDIA_MI_NVS=1 \
    -DGOAL_CONFIG_MM_EXT=1 \
    -DGOAL_CONFIG_STATS=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT=1 \
    -DGOAL_CONFIG_TARGET_FREERTOS_TIMER_TICK_FROM_HOOK=1 \
    -DGOAL_CONFIG_TASK=1 \
    -DGOAL_CONFIG_TCPIP_STACK=1 \
    -DGOAL_CONFIG_TCPIP_TCP=1 \
    -DGOAL_DLR=1 \
    -DGOAL_FEAT_LWIP_INIT=1 \
    -DGOAL_MI_DM_CFG_LOCAL=1 \
    -DMCUX_META_BUILD \
    -DMCUXPRESSO_SDK \
    -DMIMXRT1189_cm33_SERIES \
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
    TARGETS debug release flexspi_nor_debug flexspi_nor_release
    SYMBOLS " \
    __use_shmem__=1 \
    __multicore__=1 \
    "
)

# replac linker script for debug configuration
mcux_remove_armgcc_linker_script(
    TARGETS debug release
    BASE_PATH ${SdkRootDirPath}
    LINKER ${device_root}/RT/RT1180/MIMXRT1189/gcc/MIMXRT1189xxxxx_cm33_ram.ld
)

mcux_add_armgcc_linker_script(
    TARGETS debug release
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    LINKER armgcc/MIMXRT1189xxxxx_cm33_ram.ld
)

# replac linker script for flexspi_nor_* configuration
mcux_remove_armgcc_linker_script(
    TARGETS flexspi_nor_debug flexspi_nor_release
    BASE_PATH ${SdkRootDirPath}
    LINKER ${device_root}/RT/RT1180/MIMXRT1189/gcc/MIMXRT1189xxxxx_cm33_flexspi_nor.ld
)

mcux_add_armgcc_linker_script(
    TARGETS flexspi_nor_debug flexspi_nor_release
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    LINKER armgcc/MIMXRT1189xxxxx_cm33_flexspi_nor.ld
)
# adding GOAL library
mcux_add_library(
    BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
    LIBS ../../../../../..//projects/goal_eip_rpc_lib/00_lib_cc/gcc/build/mimxrt1180_evk_cm33_rtos/libgoal_mimxrt1180_evk_cm33_rtos.a
)

mcux_add_include(
    BASE_PATH ${APPLICATION_BINARY_DIR}
    TOOLCHAINS armgcc
    INCLUDES ../RT1180_CM7/${CONFIG_TOOLCHAIN}/
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
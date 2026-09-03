/**
 * @file
 *
 * lwIP Options Configuration
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include <goal_includes.h>
#include <goal_target_board.h>


/****************************************************************************/
/* Configuration */
/****************************************************************************/
#ifndef GOAL_FEAT_LWIP_ASSERT
#  define GOAL_FEAT_LWIP_ASSERT 0
#endif

#define IP_SOF_BROADCAST 1
#define IP_SOF_BROADCAST_RECV 1

/*
   -----------------------------------------------
   ---------- Platform specific locking ----------
   -----------------------------------------------
*/

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#if GOAL_CONFIG_TCPIP_BSD_API == 1
#  define NO_SYS                        0
#else
#  define NO_SYS                        1
#endif /* GOAL_CONFIG_TCPIP_BSD_API */

/*
   ------------------------------------
   ---------- Memory options ----------
   ------------------------------------
*/
/**
 * LWIP_MPU_COMPATIBLE: enables special memory management mechanism
 * which makes lwip able to work on MPU (Memory Protection Unit) system
 * by not passing stack-pointers to other threads
 * (this decreases performance)
 */

/**
 * MEM_LIBC_MALLOC==1: Use malloc/free/realloc provided by your C-library
 * instead of the lwip internal allocator. Can save code size if you
 * already use it.
 */
#ifndef GOAL_CONFIG_LWIP_MEM_LIBC_MALLOC
# define GOAL_CONFIG_LWIP_MEM_LIBC_MALLOC 0
#endif
#if GOAL_CONFIG_LWIP_MEM_LIBC_MALLOC == 1
#  define MEM_LIBC_MALLOC               1
#  include <stdlib.h>
#endif

/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#define MEM_ALIGNMENT                   GOAL_TARGET_MEM_ALIGN_CPU

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 */
#if (GOAL_CONFIG_JUMBO_FRAMES == 1)
#  define MEM_SIZE      32 * 1024               /**< increased memory size */
#else
#  define MEM_SIZE      16000                   /**< lwIP default memory size */
#endif



/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/
/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high.
 */
#define MEMP_NUM_PBUF                   30

/**
 * MEMP_NUM_RAW_PCB: Number of raw connection PCBs
 * (requires the LWIP_RAW option)
 */
#define MEMP_NUM_RAW_PCB                4

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#define MEMP_NUM_UDP_PCB                1

/**
 * MEMP_NUM_TCP_PCB: the number of simultaneously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB                1

/**
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB_LISTEN         1

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_SEG                16

/**
 * MEMP_NUM_ARP_QUEUE: the number of simultaneously queued outgoing
 * packets (pbufs) that are waiting for an ARP request (to resolve
 * their destination address) to finish.
 * (requires the ARP_QUEUEING option)
 */
#ifdef GOAL_CONFIG_LWIP_MEMP_NUM_ARP_QUEUE
#  define MEMP_NUM_ARP_QUEUE            GOAL_CONFIG_LWIP_MEMP_NUM_ARP_QUEUE
#else
#  define MEMP_NUM_ARP_QUEUE            2
#endif

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simultaneously active timeouts.
 */
#define MEMP_NUM_SYS_TIMEOUT            10

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#if GOAL_CONFIG_TCPIP_BSD_API == 1
#  define MEMP_NUM_NETBUF               4
#else
#  define MEMP_NUM_NETBUF               0
#endif /* GOAL_CONFIG_TCPIP_BSD_API */

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#if GOAL_CONFIG_TCPIP_BSD_API == 1
#  define MEMP_NUM_NETCONN              5
#else
#  define MEMP_NUM_NETCONN              0
#endif /* GOAL_CONFIG_TCPIP_BSD_API */

/**
 * MEMP_NUM_TCPIP_MSG_API: the number of struct tcpip_msg, which are used
 * for callback/timeout API communication.
 * (only needed if you use tcpip.c)
 */
#if GOAL_CONFIG_TCPIP_BSD_API == 1
#  define MEMP_NUM_TCPIP_MSG_API        2
#else
#  define MEMP_NUM_TCPIP_MSG_API        0
#endif /* GOAL_CONFIG_TCPIP_BSD_API */

/**
 * MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used
 * for incoming packets.
 * (only needed if you use tcpip.c)
 */
#if GOAL_CONFIG_TCPIP_BSD_API == 1
#  define MEMP_NUM_TCPIP_MSG_INPKT      2
#else
#  define MEMP_NUM_TCPIP_MSG_INPKT      0
#endif /* GOAL_CONFIG_TCPIP_BSD_API */

/**
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool.
 */
#define PBUF_POOL_SIZE                  32


#if GOAL_CONFIG_TCPIP_BSD_API == 1
#  define DEFAULT_UDP_RECVMBOX_SIZE     2
#  define DEFAULT_TCP_RECVMBOX_SIZE     DEFAULT_UDP_RECVMBOX_SIZE
#  define DEFAULT_ACCEPTMBOX_SIZE       DEFAULT_UDP_RECVMBOX_SIZE
#  if GOAL_CONFIG_TCPIP_BSD_API_USE_LWIP_ERRNO == 1
#    define LWIP_PROVIDE_ERRNO
#  endif
#endif /* GOAL_CONFIG_TCPIP_BSD_API */


/*
   ---------------------------------
   ---------- ARP options ----------
   ---------------------------------
*/
/**
 * LWIP_ARP==1: Enable ARP functionality.
 */
#define LWIP_ARP                        1

/*
   --------------------------------
   ---------- IP options ----------
   --------------------------------
*/
/**
 * IP_FORWARD==1: Enables the ability to forward IP packets across network
 * interfaces. If you are going to run lwIP on a device with only one network
 * interface, define this to 0.
 */
#define IP_FORWARD                      0

/**
 * IP_OPTIONS: Defines the behavior for IP options.
 *      IP_OPTIONS==0_ALLOWED: All packets with IP options are dropped.
 *      IP_OPTIONS==1_ALLOWED: IP options are allowed (but not parsed).
 */
#define IP_OPTIONS_ALLOWED              1

/**
 * IP_REASSEMBLY==1: Reassemble incoming fragmented IP packets. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via IP_FRAG.
 */
#define IP_REASSEMBLY                   0

/**
 * IP_FRAG==1: Fragment outgoing IP packets if their size exceeds MTU. Note
 * that this option does not affect incoming packet sizes, which can be
 * controlled via IP_REASSEMBLY.
 */
#define IP_FRAG                         0

/**
 * IP_REASS_MAXAGE: Maximum time (in multiples of IP_TMR_INTERVAL - so seconds, normally)
 * a fragmented IP packet waits for all fragments to arrive. If not all fragments arrived
 * in this time, the whole packet is discarded.
 */
#define IP_REASS_MAXAGE                 3

/**
 * IP_REASS_MAX_PBUFS: Total maximum amount of pbufs waiting to be reassembled.
 * Since the received pbufs are enqueued, be sure to configure
 * PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS so that the stack is still able to receive
 * packets even if the maximum amount of fragments is enqueued for reassembly!
 */
#define IP_REASS_MAX_PBUFS              0

/**
 * IP_FRAG_USES_STATIC_BUF==1: Use a static MTU-sized buffer for IP
 * fragmentation. Otherwise pbufs are allocated and reference the original
 * packet data to be fragmented.
 */
#define IP_FRAG_USES_STATIC_BUF         0

/**
 * IP_DEFAULT_TTL: Default value for Time-To-Live used by transport layers.
 */
#define IP_DEFAULT_TTL                  255

/*
   ----------------------------------
   ---------- ICMP options ----------
   ----------------------------------
*/
/**
 * LWIP_ICMP==1: Enable ICMP module inside the IP stack.
 * Be careful, disable that make your product non-compliant to RFC1122
 */
#define LWIP_ICMP                       1

/**
 * ICMP_TTL: Default value for Time-To-Live used by ICMP packets.
 */
#define ICMP_TTL                       (IP_DEFAULT_TTL)

/*
   ---------------------------------
   ---------- RAW options ----------
   ---------------------------------
*/
/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#define LWIP_RAW                        0

/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/
/**
 * LWIP_DHCP==1: Enable DHCP module.
 */
#if GOAL_CONFIG_DHCP_EXT == 1
#  define LWIP_DHCP                     1
#else
#  define LWIP_DHCP                     0
#endif

#if GOAL_CONFIG_DHCP == 1
/* enable reception of frames with udp.dstPort == 68, even if iface is down */
#  define LWIP_IP_ACCEPT_UDP_PORT(dst_port) ((dst_port) == PP_NTOHS(19010) || (dst_port) == PP_NTOHS(68))
#else
#  define LWIP_IP_ACCEPT_UDP_PORT(dst_port) ((dst_port) == PP_NTOHS(19010))
#endif

/*
   ------------------------------------
   ---------- AUTOIP options ----------
   ------------------------------------
*/
/**
 * LWIP_AUTOIP==1: Enable AUTOIP module.
 */
#define LWIP_AUTOIP                     0

/*
   ----------------------------------
   ---------- SNMP options ----------
   ----------------------------------
*/
/**
 * LWIP_SNMP==1: Turn on SNMP module. UDP must be available for SNMP
 * transport.
 */
#define LWIP_SNMP                       0
#define SNMP_PRIVATE_MIB                0

/*
   ----------------------------------
   ---------- IGMP options ----------
   ----------------------------------
*/
/**
 * LWIP_IGMP==1: Turn on IGMP module.
 */
#define LWIP_IGMP                       1

 /* LWIP_RAND required for IGMP */
#define LWIP_RAND(...)                  0

/*
   ----------------------------------
   ---------- DNS options -----------
   ----------------------------------
*/
/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#define LWIP_DNS                        1

/* do not use random XID */
#define LWIP_DNS_SECURE (LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT)

/*
   ---------------------------------
   ---------- UDP options ----------
   ---------------------------------
*/
/**
 * LWIP_UDP==1: Turn on UDP.
 */
#define LWIP_UDP                        1

/**
 * LWIP_UDPLITE==1: Turn on UDP-Lite. (Requires LWIP_UDP)
 */
#define LWIP_UDPLITE                    0

/**
 * UDP_TTL: Default Time-To-Live value.
 */
#define UDP_TTL                         (IP_DEFAULT_TTL)

/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
*/
/**
 * LWIP_TCP==1: Turn on TCP.
 */
#define LWIP_TCP    GOAL_CONFIG_TCPIP_TCP       /**< map GOAL TCP config flag to lwIP flag */

/* Note:
 * lwIP allocates a default amount of buffers for TCP segmentation also if TCP is not enabled.
 * By default it allocates at least 536 buffers with a default size of 32 bytes if TCP_MSS
 * is not explicitly defined by user.
 * If TCP is disabled, the buffer number is defined as zero.
 * If TCP is enabled, the default values are used by lwIP.
 */
#if !(GOAL_CONFIG_TCPIP_TCP == 1)
#  if (GOAL_CONFIG_JUMBO_FRAMES == 1)
#    define TCP_MSS  GOAL_TGT_MTU_SIZE - 40     /**< Set maximum MSS according to target MTU for jumbo frames */
#  else
#    define TCP_MSS  0                          /**< Set maximum TCP count to zero to avoid default value */
#  endif
#else
#  if defined GOAL_CONFIG_TCPIP_TCP_MSS
#    define TCP_MSS GOAL_CONFIG_TCPIP_TCP_MSS   /**< use GOAL custom MSS size for TCP_MSS */
#  else
#    if (GOAL_CONFIG_JUMBO_FRAMES == 1)
#      define TCP_MSS  GOAL_TGT_MTU_SIZE - 40   /**< target MTU for jumbo frames minus 40 byte TCP/IP header) */
#    else
#      define TCP_MSS  1460                     /**< use original lwIP value (1500 byte - 40 byte TCP/IP header) */
#    endif
#  endif
#  define TCP_SND_BUF  (4 * TCP_MSS)            /**< Set maximum TCP frame length to avoid default value */

#  define TCP_LISTEN_BACKLOG 1
#endif



/*
   ----------------------------------
   ---------- Pbuf options ----------
   ----------------------------------
*/
/**
 * PBUF_LINK_HLEN: the number of bytes that should be allocated for a
 * link level header. The default is 14, the standard value for
 * Ethernet.
 */
#define PBUF_LINK_HLEN                  14

/*
   ------------------------------------
   ---------- LOOPIF options ----------
   ------------------------------------
*/
/**
 * LWIP_HAVE_LOOPIF==1: Support loop interface (127.0.0.1) and loopif.c
 */
#define LWIP_HAVE_LOOPIF                0


/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/

/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                    0

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#if GOAL_CONFIG_TCPIP_BSD_API == 1
#  define LWIP_SOCKET                   1
#  define LWIP_COMPAT_SOCKETS           2
#else
#  define LWIP_SOCKET                   0
#  define LWIP_COMPAT_SOCKETS           0
#endif /* GOAL_CONFIG_TCPIP_BSD_API */

/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/
/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#if GOAL_CONFIG_IP_STATS == 1
#  define LWIP_STATS                    1
#  define MIB2_STATS                    1
#else
#  define LWIP_STATS                    0
#  define MIB2_STATS                    0
#endif


/* Misc */

#if GOAL_CONFIG_TCPIP_BSD_API_IGNORE_TS_STRUCT == 0
#  define LWIP_TIMEVAL_PRIVATE 1
#else
#  define LWIP_TIMEVAL_PRIVATE 0
#endif


/********************************************************************/

#define ETHARP_SUPPORT_VLAN 1

/* If ETHARP_VLAN_CHECK is defined lwIP will check the VLAN-ID of incoming
 * frames and silently discard frames with non-matchin VLAN-IDs.
 * The value of ETHARP_VLAN_CHECK is the VLAN-ID of the device.
 */
/* #define ETHARP_VLAN_CHECK 0 */


/* allow option SO_REUSEADDR */
#define SO_REUSE 1

#endif /* __LWIPOPTS_H__ */

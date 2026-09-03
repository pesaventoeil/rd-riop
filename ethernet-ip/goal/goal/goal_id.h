/** @file
 *
 * @brief GOAL id list
 *
 * @copyright
 * Copyright 2010-2017.
 * This software is protected Intellectual Property and may only be used
 * according to the license agreement.
 *
 * Copyright 2025 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP
 * and may only be used strictly in accordance with the applicable license
 * terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you
 * have read, and that you agree to comply with and are bound by, such license
 * terms. If you do not agree to be bound by the applicable license terms, then
 * you may not retain, install, activate or otherwise use the software.
 */
#ifndef GOAL_ID_H
#define GOAL_ID_H


/****************************************************************************/
/* Defines */
/****************************************************************************/

#define GOAL_ID_DEFAULT               (0)       /**< GOAL : Default ID */
#define GOAL_ID_BM                    (1)       /**< GOAL : Bitmap Handling */
#define GOAL_ID_CM                    (2)       /**< GOAL : Config Manager */
#define GOAL_ID_CTC                   (3)       /**< GOAL : CTC */
#define GOAL_ID_ETH                   (4)       /**< GOAL : Ethernet */
#define GOAL_ID_LIST                  (5)       /**< GOAL : List Management */
#define GOAL_ID_LOCK                  (6)       /**< GOAL : Lock Management */
#define GOAL_ID_LOG                   (7)       /**< GOAL : Logging */
#define GOAL_ID_MAIN                  (8)       /**< GOAL : Main */
#define GOAL_ID_MEM                   (9)       /**< GOAL : Memory Management */
#define GOAL_ID_MI                    (10)      /**< GOAL : Media Interface */
#define GOAL_ID_MA                    (11)      /**< GOAL : Media Adapter */
#define GOAL_ID_NET                   (12)      /**< GOAL : TCP/IP Networking */
#define GOAL_ID_REG                   (13)      /**< GOAL : Register Handling */
#define GOAL_ID_RPC                   (14)      /**< GOAL : RPC (CTC) */
#define GOAL_ID_TGT                   (15)      /**< GOAL : Target */
#define GOAL_ID_DRV                   (16)      /**< GOAL : Driver */
#define GOAL_ID_TASK                  (17)      /**< GOAL : Task Management */
#define GOAL_ID_TMR                   (18)      /**< GOAL : Timer Management */
#define GOAL_ID_CAN                   (19)      /**< CAN : CAN Driver */
#define GOAL_ID_CANOPEN               (20)      /**< CANopen : CANopen */
#define GOAL_ID_DHCP                  (21)      /**< DHCP : Dynamic Host Configuration Protocol */
#define GOAL_ID_DLR                   (22)      /**< DLR : Device Level Ring */
#define GOAL_ID_EIP                   (23)      /**< EtherNet/IP : EtherNet/IP */
#define GOAL_ID_EPL                   (24)      /**< Ethernet POWERLINK : Ethernet POWERLINK */
#define GOAL_ID_HTTP                  (25)      /**< HTTP : HTTP Server */
#define GOAL_ID_PACP                  (26)      /**< PACP : Pseudonymous Authentication-Based Conditional Privacy Protocol */
#define GOAL_ID_PNIO                  (27)      /**< PROFINET : PROFINET */
#define GOAL_ID_RADIUS                (28)      /**< RADIUS : Remote Authentication Dial-In User Service */
#define GOAL_ID_DRV_MBOX_PL320        (29)      /**< Driver : ARM PL320 Mailbox Driver */
#define GOAL_ID_MA_MBOX               (30)      /**< GOAL Media Interface : Mailbox Media Adapter */
#define GOAL_ID_MA_FLASH              (31)      /**< GOAL Media Interface : Flash Storage Media Adapter */
#define GOAL_ID_DRV_FLASH_REN_RZN     (32)      /**< GOAL Media Interface : Renesas RZ/N Quad-SPI Flash Driver */
#define GOAL_ID_TGT_RPC               (33)      /**< Target : Target RPC Communication */
#define GOAL_ID_DD                    (34)      /**< GOAL : Device Detection */
#define GOAL_ID_LM                    (35)      /**< GOAL : Logging Manager */
#define GOAL_ID_APPL                  (36)      /**< GOAL : Generic GOAL application */
#define GOAL_ID_BOOT                  (37)      /**< BOOT : Bootloader Interface */
#define GOAL_ID_PTP                   (38)      /**< PTP : Precision Time Protocol */
#define GOAL_ID_GPTP                  (39)      /**< GPTP : generalized Precision Time Protocol */
#define GOAL_ID_NET_IP_SET            (40)      /**< GOAL : GOAL Net IP Set */
#define GOAL_ID_CSAP                  (41)      /**< GOAL : Config Storage and Parser */
#define GOAL_ID_QUEUE                 (42)      /**< GOAL : Queue Buffer management */
#define GOAL_ID_PNIO_ALARM_ACK        (43)      /**< PROFINET : Alarm ACK/NACK-RTA-PDU */
#define GOAL_ID_PNIO_ALARM_DATA       (44)      /**< PROFINET : Alarm DATA-RTA-PDU */
#define GOAL_ID_PNIO_ALARM_ERR        (45)      /**< PROFINET : Alarm ERR-RTA-PDU */
#define GOAL_ID_PNIO_AR               (46)      /**< PROFINET : AR data */
#define GOAL_ID_PNIO_CRT_EP           (47)      /**< PROFINET : CRT Generic Endpoint */
#define GOAL_ID_PNIO_CRT_EP_INPUT     (48)      /**< PROFINET : CRT Input Endpoint */
#define GOAL_ID_PNIO_CRT_EP_OUTPUT    (49)      /**< PROFINET : CRT Output Endpoint */
#define GOAL_ID_PNIO_CRT_EP_CLEANUP   (50)      /**< PROFINET : CRT Endpoint cleanup */
#define GOAL_ID_PNIO_CTX_APP_READY    (51)      /**< PROFINET : Context Application Ready */
#define GOAL_ID_PNIO_DCP_IDENT_RESP   (52)      /**< PROFINET : DCP Identify Response */
#define GOAL_ID_PNIO_DCP_GET_RESP     (53)      /**< PROFINET : DCP Get Response */
#define GOAL_ID_PNIO_DCP_SET_RESP     (54)      /**< PROFINET : DCP Set Response */
#define GOAL_ID_PNIO_LLDP             (55)      /**< PROFINET : LLDP */
#define GOAL_ID_PNIO_RPC_LOCK         (56)      /**< PROFINET : RPC Lock */
#define GOAL_ID_PNIO_RPC_EPM_LOCK     (57)      /**< PROFINET : RPC EPM Lock */
#define GOAL_ID_PNIO_OAL_ETH_RECV     (58)      /**< PROFINET : OAL Ethernet Receive */
#define GOAL_ID_PNIO_OAL_ETH_SEND     (59)      /**< PROFINET : OAL Ethernet Send */
#define GOAL_ID_PNIO_LOG_SYSLOG       (60)      /**< PROFINET : Logging Syslog */
#define GOAL_ID_SPI                   (61)      /**< GOAL : GOAL SPI */
#define GOAL_ID_MEM_POOL              (62)      /**< GOAL : Memory Buffer Pool */
#define GOAL_ID_ETH_RECV              (63)      /**< GOAL : Ethernet Receive */
#define GOAL_ID_ETH_SEND              (64)      /**< GOAL : Ethernet Send */
#define GOAL_ID_LOG_SYSLOG            (65)      /**< GOAL : Logging Syslog */
#define GOAL_ID_CAN_SEND              (66)      /**< CAN : Send Buffer */
#define GOAL_ID_CAN_TXBUF             (67)      /**< CAN : TX Buffer Pool */
#define GOAL_ID_CM_VAR_TABLE          (68)      /**< GOAL : Config Manager Variable Table */
#define GOAL_ID_MODBUS                (69)      /**< MODBUS : Modbus */
#define GOAL_ID_RSTP                  (70)      /**< RSTP : Rapid Spanning Tree Protocol */
#define GOAL_ID_IGMP_SNOOP            (71)      /**< IGMP Snooping : IGMP Snooping */
#define GOAL_ID_CCM                   (72)      /**< CCM : CCM Ethernet Modules */
#define GOAL_ID_INSTANCE              (73)      /**< GOAL : Generic Instance */
#define GOAL_ID_PNIO_CTC              (74)      /**< PROFINET : PROFINET CTC Instance */
#define GOAL_ID_SNMP                  (75)      /**< SNMP : Simple Network Management Protocol */
#define GOAL_ID_MA_NVS                (76)      /**< NVS : Non Volatile Storage Media Adapter */
#define GOAL_ID_MI_NVS                (77)      /**< NVS : Non Volatile Storage Media Interface */
#define GOAL_ID_MA_TLS                (78)      /**< TLS : Tansport Layer Security Media Adapter */
#define GOAL_ID_MA_SECTION            (79)      /**< SECTION : Memory Section MA */
#define GOAL_ID_MA_UART               (80)      /**< UART : UART MA */
#define GOAL_ID_MA_SPI                (81)      /**< SPI : SPI MA */
#define GOAL_ID_MI_CTC_SPI            (82)      /**< CTC : CTC SPI MI */
#define GOAL_ID_MI_CTC                (83)      /**< CTC : CTC MI */
#define GOAL_ID_DRV_SPI               (84)      /**< SPI : SPI Driver */
#define GOAL_ID_MA_DPRAM              (85)      /**< DPRAM : DPRAM MA */
#define GOAL_ID_MA_CHAN_TCP           (86)      /**< NET : Networking TCP channel */
#define GOAL_ID_MA_CHAN_UDP           (87)      /**< NET : Networking UDP channel */
#define GOAL_ID_MI_CTC_DPRAM          (88)      /**< CTC : CTC DPRAM MI */
#define GOAL_ID_DRV_DPRAM             (89)      /**< DPRAM : DPRAM Driver */
#define GOAL_ID_NET_CTC               (90)      /**< NET : Network channel CTC wrapper */
#define GOAL_ID_HTTP_CTC              (91)      /**< HTTP : HTTP CTC Instance */
#define GOAL_ID_MA_NET                (92)      /**< NET : Networking MA */
#define GOAL_ID_HTTPS                 (93)      /**< HTTPS : HTTPS Server */
#define GOAL_ID_MI_MCTC               (94)      /**< MCTC : Micro Core To Core Media Interface */
#define GOAL_ID_PNIO_CTC_AC           (95)      /**< PROFINET : PROFINET CTC AC Instance */
#define GOAL_ID_PNIO_CTC_CC           (96)      /**< PROFINET : PROFINET CTC CC Instance */
#define GOAL_ID_DRV_IIC               (97)      /**< IIC : IIC Driver */
#define GOAL_ID_MA_WD                 (98)      /**< WATCHDOG : Watchdog MA */
#define GOAL_ID_MI_WD                 (99)      /**< WATCHDOG : Watchdog MI */
#define GOAL_ID_DRV_WD                (100)     /**< WATCHDOG : Watchdog Driver */
#define GOAL_ID_GPT                   (101)     /**< GPT : GOAL PNIO Test Tool */
#define GOAL_ID_STAT                  (102)     /**< GOAL : Statistics */
#define GOAL_ID_RB                    (103)     /**< GOAL : Ring Buffer */
#define GOAL_ID_UTIL                  (104)     /**< GOAL : Utilities */
#define GOAL_ID_GLOBAL                (105)     /**< GOAL : Global */
#define GOAL_ID_MI_CMFS               (106)     /**< CMFS : CMFS Media Interface */
#define GOAL_ID_TGT_BOARD             (107)     /**< Target : Target Board */
#define GOAL_ID_DRV_MDIO              (108)     /**< MDIO : MDIO Driver */
#define GOAL_ID_DRV_ETH               (109)     /**< ETH : ETH Driver */
#define GOAL_ID_DRV_NET               (110)     /**< NET : NET Driver */
#define GOAL_ID_DRV_NVS               (111)     /**< NVS : NVS Driver */
#define GOAL_ID_DRV_PHY               (112)     /**< PHY : PHY Driver */
#define GOAL_ID_DRV_SWITCH            (113)     /**< SWITCH : SWITCH Driver */
#define GOAL_ID_MA_LED                (114)     /**< LED : LED MA */
#define GOAL_ID_DRV_LED               (115)     /**< LED : LED Driver */
#define GOAL_ID_MON                   (116)     /**< GOAL : Monitor */
#define GOAL_ID_TLS                   (117)     /**< TLS : Transport Layer Security */
#define GOAL_ID_MI_DM                 (118)     /**< MI DM : DM MI */
#define GOAL_ID_MI_DM_INPUT           (119)     /**< MI DM : DM MI Input Data */
#define GOAL_ID_MI_DM_OUTPUT          (120)     /**< MI DM : DM MI Output Data */
#define GOAL_ID_ETH_MAC_ADDR          (121)     /**< ETH : MAC Address Updater */
#define GOAL_ID_ECAT                  (122)     /**< ETHERCAT : EtherCAT */
#define GOAL_ID_PNIO_CTX              (123)     /**< PROFINET : Context Module */
#define GOAL_ID_GPT_PORT              (124)     /**< GPT : GOAL PNIO Test Tool Ethernet Port */
#define GOAL_ID_QBV_QUEUE             (125)     /**< QBV : QBV Queue Management */
#define GOAL_ID_CCL_IE_TSN            (126)     /**< CCL_IE_TSN : CC-Link IE TSN */
#define GOAL_ID_SLMP                  (127)     /**< SLMP : SLMP Stack */
#define GOAL_ID_ECAT_CTC              (128)     /**< ETHERCAT CTC : EtherCAT CTC Wrapper */
#define GOAL_ID_DRV_UART              (129)     /**< UART : Driver */
#define GOAL_ID_DRV_EVENT             (130)     /**< EVENT : Driver */
#define GOAL_ID_MA_EVENT              (131)     /**< EVENT : Event MA */
#define GOAL_ID_MA_ETH                (132)     /**< ETH : ETH MA */
#define GOAL_ID_ACD                   (133)     /**< ACD : Address Conflict Detection */
#define GOAL_ID_DRV_ID                (134)     /**< ID : Driver */
#define GOAL_ID_MA_ID                 (135)     /**< ID : ID MA */
#define GOAL_ID_CCLIEFB               (136)     /**< CCLIEFB : CC-Link IE Field Basic */
#define GOAL_ID_MRP                   (137)     /**< MRP : Media Redundancy Protocol */
#define GOAL_ID_MI_CRYPT              (138)     /**< CRYPT : Cryptography Media Interface */
#define GOAL_ID_MI_TLS                (139)     /**< TLS : Tansport Layer Security Media Interface */
#define GOAL_ID_DRV_TLS               (140)     /**< TLS : Tansport Layer Security Driver */
#define GOAL_ID_MBOX                  (141)     /**< MBOX : GOAL Mailbox */
#define GOAL_ID_OPCUA                 (142)     /**< OPCUA : OPC UA */
#define GOAL_ID_BST                   (143)     /**< BST : Binary Search Tree */
#define GOAL_ID_PNIO_FIREWALL         (144)     /**< PNIO : Firewall */
#define GOAL_ID_LLDP                  (145)     /**< LLDP : GOAL LLDP */
#define GOAL_ID_PA                    (146)     /**< PNIO : Process Automation Profile */
#define GOAL_ID_OPCUA_CTC             (147)     /**< OPCUA CTC : OPCUA CTC Wrapper */
#define GOAL_ID_MA_TEMPERATURE        (148)     /**< TEMPERATURE : TEMPERATURE MA */
#define GOAL_ID_FW                    (149)     /**< FW : Firewall */
#define GOAL_ID_DRV_BP                (150)     /**< DRV BP : Driver for Binary Protection */

#define GOAL_MODULES_ARRAY { \
GOAL_ID_DEFAULT               \
, \
GOAL_ID_BM                    \
, \
GOAL_ID_CM                    \
, \
GOAL_ID_CTC                   \
, \
GOAL_ID_ETH                   \
, \
GOAL_ID_LIST                  \
, \
GOAL_ID_LOCK                  \
, \
GOAL_ID_LOG                   \
, \
GOAL_ID_MAIN                  \
, \
GOAL_ID_MEM                   \
, \
GOAL_ID_MI                    \
, \
GOAL_ID_MA                    \
, \
GOAL_ID_NET                   \
, \
GOAL_ID_REG                   \
, \
GOAL_ID_RPC                   \
, \
GOAL_ID_TGT                   \
, \
GOAL_ID_DRV                   \
, \
GOAL_ID_TASK                  \
, \
GOAL_ID_TMR                   \
, \
GOAL_ID_CAN                   \
, \
GOAL_ID_CANOPEN               \
, \
GOAL_ID_DHCP                  \
, \
GOAL_ID_DLR                   \
, \
GOAL_ID_EIP                   \
, \
GOAL_ID_EPL                   \
, \
GOAL_ID_HTTP                  \
, \
GOAL_ID_PACP                  \
, \
GOAL_ID_PNIO                  \
, \
GOAL_ID_RADIUS                \
, \
GOAL_ID_DRV_MBOX_PL320        \
, \
GOAL_ID_MA_MBOX               \
, \
GOAL_ID_MA_FLASH              \
, \
GOAL_ID_DRV_FLASH_REN_RZN     \
, \
GOAL_ID_TGT_RPC               \
, \
GOAL_ID_DD                    \
, \
GOAL_ID_LM                    \
, \
GOAL_ID_APPL                  \
, \
GOAL_ID_BOOT                  \
, \
GOAL_ID_PTP                   \
, \
GOAL_ID_GPTP                  \
, \
GOAL_ID_NET_IP_SET            \
, \
GOAL_ID_CSAP                  \
, \
GOAL_ID_QUEUE                 \
, \
GOAL_ID_PNIO_ALARM_ACK        \
, \
GOAL_ID_PNIO_ALARM_DATA       \
, \
GOAL_ID_PNIO_ALARM_ERR        \
, \
GOAL_ID_PNIO_AR               \
, \
GOAL_ID_PNIO_CRT_EP           \
, \
GOAL_ID_PNIO_CRT_EP_INPUT     \
, \
GOAL_ID_PNIO_CRT_EP_OUTPUT    \
, \
GOAL_ID_PNIO_CRT_EP_CLEANUP   \
, \
GOAL_ID_PNIO_CTX_APP_READY    \
, \
GOAL_ID_PNIO_DCP_IDENT_RESP   \
, \
GOAL_ID_PNIO_DCP_GET_RESP     \
, \
GOAL_ID_PNIO_DCP_SET_RESP     \
, \
GOAL_ID_PNIO_LLDP             \
, \
GOAL_ID_PNIO_RPC_LOCK         \
, \
GOAL_ID_PNIO_RPC_EPM_LOCK     \
, \
GOAL_ID_PNIO_OAL_ETH_RECV     \
, \
GOAL_ID_PNIO_OAL_ETH_SEND     \
, \
GOAL_ID_PNIO_LOG_SYSLOG       \
, \
GOAL_ID_SPI                   \
, \
GOAL_ID_MEM_POOL              \
, \
GOAL_ID_ETH_RECV              \
, \
GOAL_ID_ETH_SEND              \
, \
GOAL_ID_LOG_SYSLOG            \
, \
GOAL_ID_CAN_SEND              \
, \
GOAL_ID_CAN_TXBUF             \
, \
GOAL_ID_CM_VAR_TABLE          \
, \
GOAL_ID_MODBUS                \
, \
GOAL_ID_RSTP                  \
, \
GOAL_ID_IGMP_SNOOP            \
, \
GOAL_ID_CCM                   \
, \
GOAL_ID_INSTANCE              \
, \
GOAL_ID_PNIO_CTC              \
, \
GOAL_ID_SNMP                  \
, \
GOAL_ID_MA_NVS                \
, \
GOAL_ID_MI_NVS                \
, \
GOAL_ID_MA_TLS                \
, \
GOAL_ID_MA_SECTION            \
, \
GOAL_ID_MA_UART               \
, \
GOAL_ID_MA_SPI                \
, \
GOAL_ID_MI_CTC_SPI            \
, \
GOAL_ID_MI_CTC                \
, \
GOAL_ID_DRV_SPI               \
, \
GOAL_ID_MA_DPRAM              \
, \
GOAL_ID_MA_CHAN_TCP           \
, \
GOAL_ID_MA_CHAN_UDP           \
, \
GOAL_ID_MI_CTC_DPRAM          \
, \
GOAL_ID_DRV_DPRAM             \
, \
GOAL_ID_NET_CTC               \
, \
GOAL_ID_HTTP_CTC              \
, \
GOAL_ID_MA_NET                \
, \
GOAL_ID_HTTPS                 \
, \
GOAL_ID_MI_MCTC               \
, \
GOAL_ID_PNIO_CTC_AC           \
, \
GOAL_ID_PNIO_CTC_CC           \
, \
GOAL_ID_DRV_IIC               \
, \
GOAL_ID_MA_WD                 \
, \
GOAL_ID_MI_WD                 \
, \
GOAL_ID_DRV_WD                \
, \
GOAL_ID_GPT                   \
, \
GOAL_ID_STAT                  \
, \
GOAL_ID_RB                    \
, \
GOAL_ID_UTIL                  \
, \
GOAL_ID_GLOBAL                \
, \
GOAL_ID_MI_CMFS               \
, \
GOAL_ID_TGT_BOARD             \
, \
GOAL_ID_DRV_MDIO              \
, \
GOAL_ID_DRV_ETH               \
, \
GOAL_ID_DRV_NET               \
, \
GOAL_ID_DRV_NVS               \
, \
GOAL_ID_DRV_PHY               \
, \
GOAL_ID_DRV_SWITCH            \
, \
GOAL_ID_MA_LED                \
, \
GOAL_ID_DRV_LED               \
, \
GOAL_ID_MON                   \
, \
GOAL_ID_TLS                   \
, \
GOAL_ID_MI_DM                 \
, \
GOAL_ID_MI_DM_INPUT           \
, \
GOAL_ID_MI_DM_OUTPUT          \
, \
GOAL_ID_ETH_MAC_ADDR          \
, \
GOAL_ID_ECAT                  \
, \
GOAL_ID_PNIO_CTX              \
, \
GOAL_ID_GPT_PORT              \
, \
GOAL_ID_QBV_QUEUE             \
, \
GOAL_ID_CCL_IE_TSN            \
, \
GOAL_ID_SLMP                  \
, \
GOAL_ID_ECAT_CTC              \
, \
GOAL_ID_DRV_UART              \
, \
GOAL_ID_DRV_EVENT             \
, \
GOAL_ID_MA_EVENT              \
, \
GOAL_ID_MA_ETH                \
, \
GOAL_ID_ACD                   \
, \
GOAL_ID_DRV_ID                \
, \
GOAL_ID_MA_ID                 \
, \
GOAL_ID_CCLIEFB               \
, \
GOAL_ID_MRP                   \
, \
GOAL_ID_MI_CRYPT              \
, \
GOAL_ID_MI_TLS                \
, \
GOAL_ID_DRV_TLS               \
, \
GOAL_ID_MBOX                  \
, \
GOAL_ID_OPCUA                 \
, \
GOAL_ID_BST                   \
, \
GOAL_ID_PNIO_FIREWALL         \
, \
GOAL_ID_LLDP                  \
, \
GOAL_ID_PA                    \
, \
GOAL_ID_OPCUA_CTC             \
, \
GOAL_ID_MA_TEMPERATURE        \
,\
GOAL_ID_FW                    \
,\
GOAL_ID_DRV_BP                \
,\
    }

#endif /* GOAL_ID_H */


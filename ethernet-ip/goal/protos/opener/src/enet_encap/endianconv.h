/** @file
 *
 * @brief Endian conversion
 *
 * This module implements functions for reading and writing data with a special
 * endianess from/to a buffer.
 *
 * @copyright
 * This software consists of the OpENer by Rockwell Automation, Inc. and
 * extensive fixes, changes and improvements by port GmbH.
 *
 * Copyright (c) 2019, port GmbH for all fixes, changes and improvements in
 * this software, added to the original, unchanged OpENer
 *
 * Copyright (c) 2009, Rockwell Automation, Inc. for the original, unchanged
 * OpENer source code
 *
 * All rights reserved.
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

#ifndef ENDIANCONV_H
#define ENDIANCONV_H


/****************************************************************************/
/** Convert 16 bit little endian value from buffer to host variable
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 2.
 *
 * @return 16bit value
 */
static inline EIP_UINT16 ltohs(
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */
    EIP_UINT16 data;                            /* host variable */

    pBuf = *ppBuf;
    data = (EIP_UINT16) pBuf[0] |
           (EIP_UINT16) (pBuf[1] << 8);

    *ppBuf += 2;

    return data;
}


/****************************************************************************/
/** Convert 32 bit little endian value from buffer to host variable
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 4.
 *
 * @return 32bit value
 */
static inline EIP_UINT32 ltohl(
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */
    EIP_UINT32 data;                            /* host variable */

    pBuf = *ppBuf;

    data = (EIP_UINT32) pBuf[0] |
           (EIP_UINT32) (pBuf[1] << 8) |
           (EIP_UINT32) (pBuf[2] << 16) |
           (EIP_UINT32) (pBuf[3] << 24);

    *ppBuf += 4;

    return data;
}


/****************************************************************************/
/** Convert 16 bit variable to little endian value in buffer
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 2.
 */
static inline void htols(
    EIP_UINT16 data,                            /**< host variable */
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */

    pBuf = *ppBuf;

    pBuf[0] = (unsigned char) data;
    pBuf[1] = (unsigned char) (data >> 8);

    *ppBuf += 2;
}


/****************************************************************************/
/** Convert 32 bit variable to little endian value in buffer
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 4.
 */
static inline void htoll(
    EIP_UINT32 data,                            /**< host variable */
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */

    pBuf = *ppBuf;

    pBuf[0] = (unsigned char) data;
    pBuf[1] = (unsigned char) (data >> 8);
    pBuf[2] = (unsigned char) (data >> 16);
    pBuf[3] = (unsigned char) (data >> 24);

    *ppBuf += 4;
}


/****************************************************************************/
/** Convert 16 bit big endian value from buffer to host variable
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 2.
 *
 * @return 16bit value
 */
static inline EIP_UINT16 btohs(
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */
    EIP_UINT16 data;                            /* host variable */

    pBuf = *ppBuf;

    data = (EIP_UINT16) pBuf[1] |
           (EIP_UINT16) (pBuf[0] << 8);

    *ppBuf += 2;

    return data;
}


/****************************************************************************/
/** Convert 32 bit big endian value from buffer to host variable
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 4.
 *
 * @return 32bit value
 */
static inline EIP_UINT32 btohl(
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */
    EIP_UINT32 data;                            /* host variable */

    pBuf = *ppBuf;

    data = (EIP_UINT32) pBuf[3] |
           (EIP_UINT32) (pBuf[2] << 8) |
           (EIP_UINT32) (pBuf[1] << 16) |
           (EIP_UINT32) (pBuf[0] << 24);

    *ppBuf += 4;

    return data;
}


/****************************************************************************/
/** Convert 16 bit variable to big endian value in buffer
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 2.
 */
static inline void htobs(
    EIP_UINT16 data,                            /**< host variable */
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */

    pBuf = *ppBuf;

    pBuf[1] = (unsigned char) data;
    pBuf[0] = (unsigned char) (data >> 8);

    *ppBuf += 2;
}


/****************************************************************************/
/** Convert 32 bit variable to big endian value in buffer
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 4.
 */
static inline void htobl(
    EIP_UINT32 data,                            /**< host variable */
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */

    pBuf = *ppBuf;

    pBuf[3] = (unsigned char) data;
    pBuf[2] = (unsigned char) (data >> 8);
    pBuf[1] = (unsigned char) (data >> 16);
    pBuf[0] = (unsigned char) (data >> 24);

    *ppBuf += 4;
}


/****************************************************************************/
/** Convert 64 bit little endian value from buffer to host variable
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 8.
 *
 * @return 64bit value
 */
static inline EIP_UINT64 ltoh64(
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */
    EIP_UINT64 data;                            /* host variable */

    pBuf = *ppBuf;

    data = ((((EIP_UINT64) pBuf[7]) << 56) & 0xFF00000000000000LL)
            + ((((EIP_UINT64) pBuf[6]) << 48) & 0x00FF000000000000LL)
            + ((((EIP_UINT64) pBuf[5]) << 40) & 0x0000FF0000000000LL)
            + ((((EIP_UINT64) pBuf[4]) << 32) & 0x000000FF00000000LL)
            + ((((EIP_UINT64) pBuf[3]) << 24) & 0x00000000FF000000)
            + ((((EIP_UINT64) pBuf[2]) << 16) & 0x0000000000FF0000)
            + ((((EIP_UINT64) pBuf[1]) << 8) & 0x000000000000FF00)
            + (((EIP_UINT64) pBuf[0]) & 0x00000000000000FF);

    (*ppBuf) += 8;

    return data;
}


/****************************************************************************/
/** Convert 64 bit variable to little endian value in buffer
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 8.
 */
static inline void htol64(
    EIP_UINT64 data,                            /**< host variable */
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */

    pBuf = *ppBuf;

    pBuf[7] = (EIP_UINT8) (data >> 56) & 0xFF;
    pBuf[6] = (EIP_UINT8) (data >> 48) & 0xFF;
    pBuf[5] = (EIP_UINT8) (data >> 40) & 0xFF;
    pBuf[4] = (EIP_UINT8) (data >> 32) & 0xFF;
    pBuf[3] = (EIP_UINT8) (data >> 24) & 0xFF;
    pBuf[2] = (EIP_UINT8) (data >> 16) & 0xFF;
    pBuf[1] = (EIP_UINT8) (data >> 8) & 0xFF;
    pBuf[0] = (EIP_UINT8) (data) & 0xFF;

    (*ppBuf) += 8;
}


/****************************************************************************/
/** Convert 64 bit big endian value from buffer to host variable
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 8.
 *
 * @return 64bit value
 */
static inline EIP_UINT64 btoh64(
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */
    EIP_UINT64 data;                            /* host variable */

    pBuf = *ppBuf;

    data = ((((EIP_UINT64) pBuf[0]) << 56) & 0xFF00000000000000LL)
            + ((((EIP_UINT64) pBuf[1]) << 48) & 0x00FF000000000000LL)
            + ((((EIP_UINT64) pBuf[2]) << 40) & 0x0000FF0000000000LL)
            + ((((EIP_UINT64) pBuf[3]) << 32) & 0x000000FF00000000LL)
            + ((((EIP_UINT64) pBuf[4]) << 24) & 0x00000000FF000000)
            + ((((EIP_UINT64) pBuf[5]) << 16) & 0x0000000000FF0000)
            + ((((EIP_UINT64) pBuf[6]) << 8) & 0x000000000000FF00)
            + (((EIP_UINT64) pBuf[7]) & 0x00000000000000FF);

    (*ppBuf) += 8;

    return data;
}


/****************************************************************************/
/** Convert 64 bit variable to big endian value in buffer
 *
 * @ingroup ENCAP
 *
 * The buffer pointer will be incremented by 8.
 */
static inline void htob64(
    EIP_UINT64 data,                            /**< host variable */
    EIP_UINT8 **ppBuf                           /**< data buffer reference */
)
{
    EIP_UINT8 *pBuf;                            /* data buffer */

    pBuf = *ppBuf;

    pBuf[0] = (EIP_UINT8) (data >> 56) & 0xFF;
    pBuf[1] = (EIP_UINT8) (data >> 48) & 0xFF;
    pBuf[2] = (EIP_UINT8) (data >> 40) & 0xFF;
    pBuf[3] = (EIP_UINT8) (data >> 32) & 0xFF;
    pBuf[4] = (EIP_UINT8) (data >> 24) & 0xFF;
    pBuf[5] = (EIP_UINT8) (data >> 16) & 0xFF;
    pBuf[6] = (EIP_UINT8) (data >> 8) & 0xFF;
    pBuf[7] = (EIP_UINT8) (data) & 0xFF;

    (*ppBuf) += 8;
}


#endif /* ENDIANCONV_H */

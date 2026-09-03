/** @file
 *
 * @brief Http webserver Example
 *
 * This application runs a simple HTTP server that hosts a demo HTML page.
 * It can be accessed via web browser by browsing the IP address of this device.
 *
 * @copyright
 * Copyright 2020 port GmbH Halle/Saale.
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

#include "goal_includes.h"
#include <goal_http.h>
#include "goal_appl.h"


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define APPL_HTTP_CHAN_CNT                     2
#define APPL_TCP_PORT                       8081

#define PAGE_INDEX_CONTENT "\
<html><head><meta charset = \"utf-8\"><title>GOAL HTTP Example</title></head> \r\n\
<body><h2>GOAL HTTP Example - Index</h2> \r\n\
    Basic authentication successful\r\n\
</body></html> "

#define PAGE_1_CONTENT "\
<html><head><meta charset = \"utf-8\"><title>GOAL HTTP Example</title></head> \r\n\
<body><h2>GOAL HTTP Example - Page 1</h2> \r\n\
    Basic authentication successful\r\n\
</body></html> "

#define PAGE_2_CONTENT "\
<html><head><meta charset = \"utf-8\"><title>GOAL HTTP Example</title></head> \r\n\
<body><h2>GOAL HTTP Example - Page 2</h2> \r\n\
    Basic authentication successful\r\n\
</body></html> "

#define PAGE_3_CONTENT "\
<html><head><meta charset = \"utf-8\"><title>GOAL HTTP Example</title></head> \r\n\
<body><h2>GOAL HTTP Example - Page 3</h2> \r\n\
    Basic authentication successful\r\n\
</body></html> "


/****************************************************************************/
/* Global variables */
/****************************************************************************/
static GOAL_HTTP_T *httpInstance;               /**< instance pointer */
static GOAL_HTTP_HDL_T hdlResIdx = 0;           /**< resource handle */
static GOAL_HTTP_HDL_T hdlRes1 = 0;             /**< resource handle */
static GOAL_HTTP_HDL_T hdlRes2 = 0;             /**< resource handle */
static GOAL_HTTP_HDL_T hdlRes3 = 0;             /**< resource handle */
static const uint8_t pageIndex[] = PAGE_INDEX_CONTENT; /**< index webpage */
static const uint8_t page1[] = PAGE_1_CONTENT;  /**< page 1 */
static const uint8_t page2[] = PAGE_2_CONTENT;  /**< page 2 */
static const uint8_t page3[] = PAGE_3_CONTENT;  /**< page 3 */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/

static GOAL_STATUS_T goal_http_cb(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< callback info */
);


static GOAL_STATUS_T goal_http_res_reg(
    GOAL_HTTP_T *pInst                          /**< instance pointer */
);


/****************************************************************************/
/**
 * This is the http application callback.
 *
 * @retval GOAL_OK on success
 * @retval GOAL error code otherwise
 */
static GOAL_STATUS_T goal_http_cb(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< callback info */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    /* check resource handle */
    if ((hdlResIdx == pCbInfo->hdlRes)
    || (hdlRes1 == pCbInfo->hdlRes)
    || (hdlRes2 == pCbInfo->hdlRes)
    || (hdlRes3 == pCbInfo->hdlRes)) {
    } else {
        /* error */
        goal_logErr("Requested resource does not match");
        res = GOAL_ERROR;
    }

    if (GOAL_RES_OK(res)) {
        if (hdlResIdx == pCbInfo->hdlRes) {
            /* check requested method */
            if (GOAL_HTTP_FW_GET == pCbInfo->reqType) {
                /* set information for send function */
                GOAL_HTTP_GET_RETURN_HTML(pCbInfo, pageIndex, GOAL_STRLEN((const char *) pageIndex));
            }
        } else if (hdlRes1 == pCbInfo->hdlRes) {
            /* check requested method */
            if (GOAL_HTTP_FW_GET == pCbInfo->reqType) {
                /* set information for send function */
                GOAL_HTTP_GET_RETURN_HTML(pCbInfo, page1, GOAL_STRLEN((const char *) page1));
            }
        }  else if (hdlRes2 == pCbInfo->hdlRes) {
            /* check requested method */
            if (GOAL_HTTP_FW_GET == pCbInfo->reqType) {
                /* set information for send function */
                GOAL_HTTP_GET_RETURN_HTML(pCbInfo, page2, GOAL_STRLEN((const char *) page2));
            }
        }  else if (hdlRes3 == pCbInfo->hdlRes) {
            /* check requested method */
            if (GOAL_HTTP_FW_GET == pCbInfo->reqType) {
                /* set information for send function */
                GOAL_HTTP_GET_RETURN_HTML(pCbInfo, page3, GOAL_STRLEN((const char *) page3));
            }
        }
    }

    if (GOAL_RES_OK(res)) {
        if ((GOAL_HTTP_FW_REQ_DONE_OK == pCbInfo->reqType) || (GOAL_HTTP_FW_REQ_DONE_ERR == pCbInfo->reqType)) {
            /* send info message */
            goal_logInfo("Transmission done");
            GOAL_HTTP_RETURN_OK_204(pCbInfo);
        }
    }

    return res;
}


/****************************************************************************/
/**
 * This function registers resources at the http module.
 *
 * @retval GOAL_OK on success
 * @retval GOAL error code otherwise
 */
static GOAL_STATUS_T goal_http_res_reg(
    GOAL_HTTP_T *pInst                          /**< instance pointer */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    /* register resource */
    res = goal_httpResReg(pInst,
                           (uint8_t *) "/index.html",
                           GOAL_HTTP_METHOD_ALLW_GET | GOAL_HTTP_AUTH_USERLEVEL0,
                           goal_http_cb,
                           NULL,
                           &hdlResIdx);

    /* register resource */
    res = goal_httpResReg(pInst,
                           (uint8_t *) "/page1.html",
                           GOAL_HTTP_METHOD_ALLW_GET | GOAL_HTTP_AUTH_USERLEVEL1,
                           goal_http_cb,
                           NULL,
                           &hdlRes1);

    /* register resource */
    res = goal_httpResReg(pInst,
                           (uint8_t *) "/page2.html",
                           GOAL_HTTP_METHOD_ALLW_GET | GOAL_HTTP_AUTH_USERLEVEL2,
                           goal_http_cb,
                           NULL,
                           &hdlRes2);

    /* register resource */
    res = goal_httpResReg(pInst,
                           (uint8_t *) "/page3.html",
                           GOAL_HTTP_METHOD_ALLW_GET | GOAL_HTTP_AUTH_USERLEVEL3,
                           goal_http_cb,
                           NULL,
                           &hdlRes3);

    return res;
}


/****************************************************************************/
/** Application Init
 *
 * This function must initialize the GOAL and all used protocol stacks.
 * Furthermore application specific resources must be initialized.
 */
GOAL_STATUS_T appl_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* initialize HTTP module */
    res = goal_httpInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Initialization of http module failed");
        return res;
    }

    return res;
}


/****************************************************************************/
/** Application Setup
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */

    /* get new instance */
    ret = goal_httpNew(&httpInstance, APPL_TCP_PORT, APPL_HTTP_CHAN_CNT);

    if (GOAL_RES_OK(ret)) {
        /* register resources */
        ret = goal_http_res_reg(httpInstance);
    }

    if (GOAL_RES_OK(ret)) {
        /* set user name and password */
        ret = goal_httpAuthBasSetUserInfo(httpInstance, GOAL_HTTP_AUTH_USERLEVEL0, "index", "level0");
    }

    if (GOAL_RES_OK(ret)) {
        /* set user name and password */
        ret = goal_httpAuthBasSetUserInfo(httpInstance, GOAL_HTTP_AUTH_USERLEVEL1, "page1", "level1");
    }

    if (GOAL_RES_OK(ret)) {
        /* set user name and password */
        ret = goal_httpAuthBasSetUserInfo(httpInstance, GOAL_HTTP_AUTH_USERLEVEL2, "page2", "level2");
    }

    if (GOAL_RES_OK(ret)) {
        /* set user name and password */
        ret = goal_httpAuthBasSetUserInfo(httpInstance, GOAL_HTTP_AUTH_USERLEVEL3, "page3", "level3");
    }

    return ret;
}

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
#include <protos/http/goal_http.h>
#include "goal_appl.h"


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define APPL_HTTP_CHAN_CNT                     2
#define APPL_TCP_PORT                       8081

#define PAGE_INDEX_CONTENT "\
<html><head><meta charset = \"utf-8\"><title>GOAL HTTP Example</title></head> \r\n\
<body><h2>GOAL HTTP Example</h2> \r\n\
    You can post a small amount of 32 byte data to this URL \r\n\
    <form method = \"post\" enctype = \"multipart/form-data\"> \r\n\
    <input type = \"file\" name = \"file\"><br> \r\n\
    <input type = \"submit\" value = \"POST\"> \r\n\
    </form> \r\n\
</body></html> "


/****************************************************************************/
/* Global variables */
/****************************************************************************/
static GOAL_HTTP_T *httpInstance;               /**< instance pointer */
static GOAL_HTTP_HDL_T hdlRes = 0;              /**< resource handle */
static const uint8_t pageIndex[] = PAGE_INDEX_CONTENT; /**< index webpage */


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
    static uint8_t data[32];                    /* test data */
    static uint8_t lenData = 0;                 /* test data length */
    unsigned int lenCopy = 0;                   /* length for copying data */

    /* check resource handle */
    if (hdlRes != pCbInfo->hdlRes) {
        /* error */
        goal_logErr("Requested resource does not match");
        res = GOAL_ERROR;
    }

    if (GOAL_RES_OK(res)) {
        /* check requested method */
        switch (pCbInfo->reqType)
        {
        case GOAL_HTTP_FW_GET:
            if (0 == GOAL_MEMCMP(pCbInfo->cs.pUrl, "/index.html?data", GOAL_STRLEN("/index.html?data"))) {
                /* return test data */
                GOAL_HTTP_GET_RETURN_RAW(pCbInfo, data, lenData);
            } else {
                /* set information for send function */
                GOAL_HTTP_GET_RETURN_HTML(pCbInfo, pageIndex, GOAL_STRLEN((const char *) pageIndex));
            }
            break;

        case GOAL_HTTP_FW_POST_START:
            goal_logInfo("Start of POST request reached application");
            /* reset test data */
            GOAL_MEMSET(data, 0, sizeof(data));
            lenData = 0;
            /* return html code "204 no content" */
            GOAL_HTTP_RETURN_OK_204(pCbInfo);
            break;

        case GOAL_HTTP_FW_POST_DATA:
            goal_logInfo("Data chunk of POST request reached application");
            /* calculate length for copying data */
            if (lenData < sizeof(data)) {
                if (pCbInfo->cs.lenData < (sizeof(data) - lenData)) {
                    lenCopy = pCbInfo->cs.lenData;
                } else {
                    lenCopy = sizeof(data) - lenData;
                }
                /* update recent data length */
                lenData += lenCopy;
            }
            /* copy testdata */
            GOAL_MEMCPY(data, pCbInfo->cs.pData, lenCopy);
            /* return html code "204 no content" */
            GOAL_HTTP_RETURN_OK_204(pCbInfo);
            break;

        case GOAL_HTTP_FW_POST_END:
            goal_logInfo("End POST request reached application");
            /* return html code "204 no content" */
            GOAL_HTTP_RETURN_OK_204(pCbInfo);
            break;

        case GOAL_HTTP_FW_REQ_DONE_OK:
        case GOAL_HTTP_FW_REQ_DONE_ERR:
            /* send info message */
            goal_logInfo("Transmission done");
            GOAL_HTTP_RETURN_OK_204(pCbInfo);
            break;

        default:
            break;
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
                           GOAL_HTTP_METHOD_ALLW_GET | GOAL_HTTP_METHOD_ALLW_POST,
                           goal_http_cb,
                           NULL,
                           &hdlRes);

    return res;
}


/****************************************************************************/
/** Application
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

    /* get new http instance */
    ret = goal_httpNew(&httpInstance, APPL_TCP_PORT, APPL_HTTP_CHAN_CNT);

    if (GOAL_RES_OK(ret)) {
        /* register resources */
        ret = goal_http_res_reg(httpInstance);
    }

    return ret;
}


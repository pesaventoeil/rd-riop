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
<body><h2>GOAL HTTP Example</h2> \r\n\
    You can reach the following URLs: <br>\r\n\
    /admin/ <br>\r\n\
    /firmware/cc/ <br>\r\n\
    /admin/login/ <br>\r\n\
    /index.html <br>\r\n\
    /firmware/ <br>\r\n\
    /firmware/ac/ <br>\r\n\
    /firmware/ac/upload <br>\r\n\
    /firmware/cc/upload <br>\r\n\
    /firmware/cc/commit <br>\r\n\
    /firmware/ac/commit <br>\r\n\
</body></html> "

#define PAGE_CONT_START "\
<html><head><meta charset = \"utf-8\"><title>GOAL HTTP Example</title></head> \r\n\
<body><h2>GOAL HTTP Example</h2> \r\n"

#define PAGE_CONT_END "\
\r\n</body></html>"

#define GOAL_APPL_RES_CNT 10


/****************************************************************************/
/* Local data types */
/****************************************************************************/

typedef struct {
    char url[32];                               /**< url string */
    GOAL_HTTP_HDL_T hdlRes;                     /**< resource handle */
} GOAL_APPL_RES_T;


/****************************************************************************/
/* Global variables */
/****************************************************************************/
static GOAL_HTTP_T *httpInstance;               /**< instance pointer */
static const uint8_t pageIndex[] = PAGE_INDEX_CONTENT; /**< index webpage */
static uint8_t genericPage[256];                /**< generic webpage string */

static GOAL_APPL_RES_T resources[GOAL_APPL_RES_CNT] = {
        { "/admin/", 0 },
        { "/firmware/cc/", 0 },
        { "/admin/login/", 0 },
        { "/index.html", 0 },
        { "/firmware/", 0 },
        { "/firmware/ac/", 0 },
        { "/firmware/ac/upload", 0 },
        { "/firmware/cc/upload", 0 },
        { "/firmware/cc/commit", 0 },
        { "/firmware/ac/commit", 0 }
};


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
    uint32_t cnt = 0;                           /* loop counter */
    uint32_t off = 0;                           /* offset */

    if (GOAL_RES_OK(res)) {
        /* check requested method */
        if (GOAL_HTTP_FW_GET == pCbInfo->reqType) {
            goal_logInfo("Resource list before sorting");
            for (cnt = 0; cnt < GOAL_APPL_RES_CNT; cnt++) {
                /* print list before registration */
                goal_logInfo("%s", resources[cnt].url);
                /* check resource handle */
                if (pCbInfo->hdlRes == resources[cnt].hdlRes) {
                    if (0 == GOAL_MEMCMP(resources[cnt].url, "/index.html", GOAL_STRLEN("/index.html"))) {
                        /* set information for send function */
                        GOAL_HTTP_GET_RETURN_HTML(pCbInfo, pageIndex, GOAL_STRLEN((const char *) pageIndex));
                    } else {
                        /* generate page */
                        GOAL_MEMSET(genericPage, 0, sizeof(genericPage));
                        off = 0;
                        GOAL_MEMCPY(&genericPage[off], PAGE_CONT_START, GOAL_STRLEN(PAGE_CONT_START));
                        off += GOAL_STRLEN(PAGE_CONT_START);
                        GOAL_MEMCPY(&genericPage[off], resources[cnt].url, GOAL_STRLEN(resources[cnt].url));
                        off += GOAL_STRLEN(resources[cnt].url);
                        GOAL_MEMCPY(&genericPage[off], PAGE_CONT_END, GOAL_STRLEN(PAGE_CONT_END));
                        /* set information for send function */
                        GOAL_HTTP_GET_RETURN_HTML(pCbInfo, genericPage, GOAL_STRLEN((const char *) genericPage));
                    }
                }
            }
        } else if ((GOAL_HTTP_FW_REQ_DONE_OK == pCbInfo->reqType) || (GOAL_HTTP_FW_REQ_DONE_ERR == pCbInfo->reqType)) {
            /* send info message */
            goal_logInfo("Transmission done");
            GOAL_HTTP_RETURN_OK_204(pCbInfo);
        }
#if !(GOAL_CONFIG_GEN_CTC_AC == 1)
        /* print sorted list */
        goal_logInfo("Resource list after sorting");
        goal_httpResMgrPrintListSort(httpInstance);
#endif
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
    uint32_t cnt = 0;                           /* loop counter */

    for (cnt = 0; cnt < GOAL_APPL_RES_CNT; cnt++) {
        /* print list before registration */
        goal_logInfo("%s", resources[cnt].url);
        /* register resources */
        res = goal_httpResReg(pInst,
                               (uint8_t *) &(resources[cnt].url[0]),
                               GOAL_HTTP_METHOD_ALLW_GET,
                               goal_http_cb,
                               NULL,
                               &(resources[cnt].hdlRes));
    }
#if !(GOAL_CONFIG_GEN_CTC_AC == 1)
    goal_httpResMgrPrintListSort(pInst);
#endif

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

    goal_logSetLevel(GOAL_DBG_INFO);

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

    return ret;
}

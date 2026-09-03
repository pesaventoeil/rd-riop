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
    We have an opening bracket here:[[<br>\r\n\
    and a closing bracket here:]]<br>\r\n\
    we have a CM var tag here: [CM:25, 0] and here for IP address [CM:12, 0]<br>\r\n\
    and an application replacement here:[VAR:appl1] and here [VAR:appl2]<br>\r\n\
</body></html> "


/****************************************************************************/
/* Global variables */
/****************************************************************************/
static GOAL_HTTP_T *httpInstance;               /**< instance pointer */
static GOAL_HTTP_HDL_T hdlRes = 0;              /**< resource handle */
static const uint8_t pageIndex[] = PAGE_INDEX_CONTENT; /**< index webpage */
static char templateString1[] = "Value1";       /**< template replacement */
static char templateString2[] = "Value2";       /**< template replacement */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/

static GOAL_STATUS_T goal_http_cb_data(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< callback info */
);

static GOAL_STATUS_T goal_http_cb_templ(
    GOAL_HTTP_APPLCB_TEMPL_T *pCbInfo           /**< pointer to callback information */
);

static GOAL_STATUS_T goal_http_res_reg(
    GOAL_HTTP_T *pInst                          /**< instance pointer */
);


/****************************************************************************/
/**
 * This is the http application data callback.
 *
 * @retval GOAL_OK on success
 * @retval GOAL error code otherwise
 */
static GOAL_STATUS_T goal_http_cb_data(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< callback info */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

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
            /* set information for send function */
            GOAL_HTTP_GET_RETURN_HTML(pCbInfo, pageIndex, GOAL_STRLEN((const char *) pageIndex));
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
 * This is the http application template handler callback.
 *
 * @retval GOAL_OK on success
 * @retval GOAL error code otherwise
 */
static GOAL_STATUS_T goal_http_cb_templ(
    GOAL_HTTP_APPLCB_TEMPL_T *pCbInfo           /**< pointer to callback information */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */

    if (0 == GOAL_MEMCMP(pCbInfo->in.name, "appl1", GOAL_STRLEN("appl1"))) {
        /* set return string pointer */
        if (GOAL_STRLEN(templateString1) <= pCbInfo->in.retLenMax) {
            GOAL_MEMCPY(pCbInfo->out.strReturn, templateString1, GOAL_STRLEN(templateString1));
        } else {
            GOAL_MEMCPY(pCbInfo->out.strReturn, templateString1, pCbInfo->in.retLenMax);
        }
    } else if (0 == GOAL_MEMCMP(pCbInfo->in.name, "appl2", GOAL_STRLEN("appl2"))) {
        /* set return string pointer */
        if (GOAL_STRLEN(templateString2) <= pCbInfo->in.retLenMax) {
            GOAL_MEMCPY(pCbInfo->out.strReturn, templateString2, GOAL_STRLEN(templateString2));
        } else {
            GOAL_MEMCPY(pCbInfo->out.strReturn, templateString2, pCbInfo->in.retLenMax);
        }
    }
    return ret;
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
                          GOAL_HTTP_METHOD_ALLW_GET,
                          goal_http_cb_data,
                          goal_http_cb_templ,
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


    return GOAL_OK;
}

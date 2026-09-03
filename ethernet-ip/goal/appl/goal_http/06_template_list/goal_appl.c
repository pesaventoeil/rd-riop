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
    We have a list of items starting here: <ul>[FOREACH:modules]\r\n\
        <li>[VAR:modName] \r\n\
        <ul>[FOREACH:submodules]\r\n\
            <li>[VAR:submodName]</li> \r\n\
        [/FOREACH] </ul> </li> \r\n\
    [/FOREACH] </ul> <br>\r\n\
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

static GOAL_STATUS_T goal_http_cb_data(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< callback info */
);

static GOAL_STATUS_T goal_http_cb_templ(
    GOAL_HTTP_APPLCB_TEMPL_T *pCbInfo           /**< pointer to callback information */
);

static GOAL_STATUS_T goal_http_res_reg(
    GOAL_HTTP_T *pInst                          /**< instance pointer */
);

static GOAL_STATUS_T goal_http_template_reg(
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

    if (pCbInfo->in.pPath) {
        if (0 == GOAL_MEMCMP(pCbInfo->in.name, "modName", GOAL_STRLEN("modName"))) {
            /* list index 0 = module list */
            switch ((pCbInfo->in.pPath)->path[0].index)
            {
            case 0:
                GOAL_MEMCPY(pCbInfo->out.strReturn, "Module 1", GOAL_STRLEN("Module 1"));
                break;
            case 1:
                GOAL_MEMCPY(pCbInfo->out.strReturn, "Module 2", GOAL_STRLEN("Module 2"));
                break;
            case 2:
                GOAL_MEMCPY(pCbInfo->out.strReturn, "Module 2", GOAL_STRLEN("Module 2"));
                break;
            case 3:
                GOAL_MEMCPY(pCbInfo->out.strReturn, "Module 3", GOAL_STRLEN("Module 3"));
                break;
            default:
                break;
            }
        } else  if (0 == GOAL_MEMCMP(pCbInfo->in.name, "submodName", GOAL_STRLEN("submodName"))) {
            /* list index 1 = submodule list */
            switch ((pCbInfo->in.pPath)->path[1].index)
            {
            case 0:
                if (2 == (pCbInfo->in.pPath)->path[0].index) {
                    GOAL_MEMCPY(pCbInfo->out.strReturn, "Submodule 3", GOAL_STRLEN("Submodule 3"));
                } else {
                    GOAL_MEMCPY(pCbInfo->out.strReturn, "Submodule 1", GOAL_STRLEN("Submodule 1"));
                }
                break;
            case 1:
                if (2 == (pCbInfo->in.pPath)->path[0].index) {
                    GOAL_MEMCPY(pCbInfo->out.strReturn, "Submodule 4", GOAL_STRLEN("Submodule 4"));
                } else {
                    GOAL_MEMCPY(pCbInfo->out.strReturn, "Submodule 2", GOAL_STRLEN("Submodule 2"));
                }
                break;
            default:
                break;
            }
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
/**
 * This function registers template lists at the http module.
 *
 * @retval GOAL_OK on success
 * @retval GOAL error code otherwise
 */
static GOAL_STATUS_T goal_http_template_reg(
    GOAL_HTTP_T *pInst                          /**< instance pointer */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    GOAL_HTTP_TEMPLATE_LIST_INIT_T initList;    /* template list */

    /* add list 1 */
    GOAL_MEMSET(&initList, 0, sizeof(initList));
    initList.listId = 1;
    initList.cntMemb = 4;
    GOAL_MEMCPY(initList.listName, "modules", GOAL_STRLEN("modules"));
    res = goal_httpTmpMgrNewList(pInst, &initList);

    /* add list 2 */
    if (GOAL_RES_OK (res)) {
        GOAL_MEMSET(&initList, 0, sizeof(initList));
        initList.listId = 2;
        initList.cntMemb = 2;
        GOAL_MEMCPY(initList.listName, "submodules", GOAL_STRLEN("submodules"));
        res = goal_httpTmpMgrNewList(pInst, &initList);
    }
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
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    /* new instance */
    res = goal_httpNew(&httpInstance, APPL_TCP_PORT, APPL_HTTP_CHAN_CNT);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Adding instance of http module failed");
    }

    /* register resources */
    res = goal_http_res_reg(httpInstance);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Resource registration at http module failed");
        return res;
    }

    /* register template lists */
    res = goal_http_template_reg(httpInstance);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Template list registration at http module failed");
    }

    return res;
}

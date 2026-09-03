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

#define PAGE_INDEX_TABLE "\
<html><head><meta charset = \"utf-8\"><title>GOAL HTTP Example</title> \r\n\
    <link rel=\"stylesheet\" href=\"stylesheet.css\"> \r\n\
</head> \r\n\
<body><h2>GOAL HTTP Example</h2> \r\n\
    We create a table:  \r\n\
    <p><table border> <tr> <th> Sensor </th> <th> Temp </th> <th> Flow 1 </th> <th> Flow 2 </th> <th> Flow 3 </th> </tr> \r\n\
        [FOREACH:column] \r\n\
        <tr> [FOREACH:row] <td> [VAR:value] </td> [/FOREACH] </tr> \r\n\
        [/FOREACH]\r\n\
    </table> </p> \r\n\
</body></html> "

#define DATA_STYLESHEET "\
table, td, th { border: 1px solid gray } "


/****************************************************************************/
/* Global variables */
/****************************************************************************/
static GOAL_HTTP_T *httpInstance;               /**< instance pointer */
static GOAL_HTTP_HDL_T hdlHtml = 0;             /**< resource handle */
static GOAL_HTTP_HDL_T hdlCss = 0;              /**< resource handle */
static const uint8_t pageIndex[] = PAGE_INDEX_TABLE; /**< index webpage */
static const uint8_t stylesheet[] = DATA_STYLESHEET; /**< style sheet */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/

static GOAL_STATUS_T goal_http_cb_data(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< callback info */
);

static GOAL_STATUS_T goal_http_cb_templ(
    GOAL_HTTP_APPLCB_TEMPL_T *pCbInfo           /**< pointer to callback information */
);

static GOAL_STATUS_T goal_applGetSensorValue(
    char *strReturn,                            /**< [out] return string */
    uint32_t lenMax,                            /**< maximum string length */
    uint32_t row,                               /**< row index */
    uint32_t index                              /**< index of value */
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

    /* check requested method */
    switch (pCbInfo->reqType)
    {
     case GOAL_HTTP_FW_GET:
        if (hdlHtml == pCbInfo->hdlRes) {
            /* set information for webserver sending routine */
            GOAL_HTTP_GET_RETURN_HTML(pCbInfo, pageIndex, GOAL_STRLEN((const char *) pageIndex));
        } else if (hdlCss == pCbInfo->hdlRes) {
            /* set information for webserver sending routine */
            GOAL_HTTP_GET_RETURN_CSS(pCbInfo, stylesheet, GOAL_STRLEN((const char *) stylesheet));
        } else {
            /* error */
            goal_logErr("Requested resource does not match");
            res = GOAL_ERROR;
        }
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

    if ((pCbInfo->in.pPath) && (2 == pCbInfo->in.pPath->pathLen)) {
        if (0 == GOAL_MEMCMP(pCbInfo->in.name, "value", GOAL_STRLEN("value"))) {
            /* list index 0 = submodule list */

            ret = goal_applGetSensorValue(pCbInfo->out.strReturn, pCbInfo->in.retLenMax, (pCbInfo->in.pPath)->path[0].index, (pCbInfo->in.pPath)->path[1].index);
        }
    }
    return ret;
}


/****************************************************************************/
/**
 * This function fills the table rows with values.
 *
 * @retval GOAL_OK on success
 * @retval GOAL error code otherwise
 */
static GOAL_STATUS_T goal_applGetSensorValue(
    char *strReturn,                            /**< [out] return string */
    uint32_t lenMax,                            /**< maximum string length */
    uint32_t row,                               /**< row index */
    uint32_t index                              /**< index of value */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    UNUSEDARG(lenMax);

    switch (index)
    {
    case 0:
        switch (row) {
            default:
            case 0:
                GOAL_MEMCPY(strReturn, "Sensor 1", GOAL_STRLEN("Sensor 1"));
                break;
            case 1:
                GOAL_MEMCPY(strReturn, "Sensor 2", GOAL_STRLEN("Sensor 2"));
                break;
            case 2:
                GOAL_MEMCPY(strReturn, "Sensor 3", GOAL_STRLEN("Sensor 3"));
                break;
            case 3:
                GOAL_MEMCPY(strReturn, "Sensor 4", GOAL_STRLEN("Sensor 4"));
                break;
            case 4:
                GOAL_MEMCPY(strReturn, "Sensor 5", GOAL_STRLEN("Sensor 5"));
                break;
        }
        break;

    case 1:
        GOAL_MEMCPY(strReturn, "72", GOAL_STRLEN("Value"));
        break;

    case 2:
        GOAL_MEMCPY(strReturn, "100%", GOAL_STRLEN("Value"));
        break;

    case 3:
        GOAL_MEMCPY(strReturn, "27%", GOAL_STRLEN("Value"));
        break;

    case 4:
        GOAL_MEMCPY(strReturn, "75%", GOAL_STRLEN("Value"));
        break;

    default:
        break;
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

    /* register html page */
    res = goal_httpResReg(pInst,
                          (uint8_t *) "/index.html",
                          GOAL_HTTP_METHOD_ALLW_GET,
                          goal_http_cb_data,
                          goal_http_cb_templ,
                          &hdlHtml);

    if (GOAL_RES_OK(res)) {
        /* register style sheet */
        res = goal_httpResReg(pInst,
                              (uint8_t *) "/stylesheet.css",
                              GOAL_HTTP_METHOD_ALLW_GET,
                              goal_http_cb_data,
                              NULL,
                              &hdlCss);
    }

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
    initList.cntMemb = 5;
    GOAL_MEMCPY(initList.listName, "column", GOAL_STRLEN("column"));
    res = goal_httpTmpMgrNewList(pInst, &initList);

    if (GOAL_RES_OK (res)) {
        /* add list 2 */
        GOAL_MEMSET(&initList, 0, sizeof(initList));
        initList.listId = 2;
        initList.cntMemb = 5;
        GOAL_MEMCPY(initList.listName, "row", GOAL_STRLEN("row"));
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

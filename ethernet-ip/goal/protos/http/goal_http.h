/** @file
 *
 * @brief Goal http interface
 *
 * This is the goal webserver user interface.
 *
 * @copyright
 * Copyright 2010-2017 port GmbH Halle/Saale.
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


#ifndef GOAL_HTTP_H
#define GOAL_HTTP_H

#include <goal_http_int.h>
#include <goal_http_tmpmgr.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/

#define GOAL_HTTP_METHOD_ALLW_GET      ((uint8_t) (1 << 0)) /**< allow GET */
#define GOAL_HTTP_METHOD_ALLW_PUT      ((uint8_t) (1 << 1)) /**< allow PUT */
#define GOAL_HTTP_METHOD_ALLW_POST     ((uint8_t) (1 << 2)) /**< allow POST */
#define GOAL_HTTP_METHOD_ALLW_DEL      ((uint8_t) (1 << 3)) /**< allow DELETE */
#define GOAL_HTTP_AUTH_USERLEVEL0      ((uint8_t) (1 << 4)) /**< authentication required */
#define GOAL_HTTP_AUTH_USERLEVEL1      ((uint8_t) (1 << 5)) /**< authentication required */
#define GOAL_HTTP_AUTH_USERLEVEL2      ((uint8_t) (1 << 6)) /**< authentication required */
#define GOAL_HTTP_AUTH_USERLEVEL3      ((uint8_t) (1 << 7)) /**< authentication required */

#define GOAL_HTTP_AUTH (GOAL_HTTP_AUTH_USERLEVEL0 | GOAL_HTTP_AUTH_USERLEVEL1 | \
                        GOAL_HTTP_AUTH_USERLEVEL2 | GOAL_HTTP_AUTH_USERLEVEL3)

#ifndef GOAL_HTTP_URL_MAX_LENGTH
# define GOAL_HTTP_URL_MAX_LENGTH 256                       /**< maximum string length */
#endif


/****************************************************************************/
/* Data types */
/****************************************************************************/

/** application request forwarding types */
typedef enum {
    GOAL_HTTP_FW_GET,                           /**< GET request */
    GOAL_HTTP_FW_POST_START,                    /**< POST request start */
    GOAL_HTTP_FW_POST_DATA,                     /**< POST request data */
    GOAL_HTTP_FW_POST_END,                      /**< POST request end */
    GOAL_HTTP_FW_REQ_DONE_OK,                   /**< last transmission finished successful */
    GOAL_HTTP_FW_REQ_DONE_ERR                   /**< last transmission finished with error */
} GOAL_HTTP_FWCB_T;

/** substructure for list path struct */
typedef struct {
    uint32_t listId;                            /**< list identifier */
    uint32_t index;                             /**< recent list index */
    uint32_t offset;                            /**< list starting point in data buffer */
} GOAL_HTTP_TMPMGR_L_PATHSUB_T;

/** list path struct */
typedef struct {
    uint32_t pathLen;                           /**< recent count of entries */
    GOAL_HTTP_TMPMGR_L_PATHSUB_T path[GOAL_HTTP_TMPMGR_MAX_LEN_PATH]; /**< array of substruct */
} GOAL_HTTP_TMPMGR_L_PATH_T;

/** application data callback information struct */
typedef struct {
    GOAL_HTTP_T         *pInst;                 /**< instance pointer */
    GOAL_HTTP_FWCB_T    reqType;                /**< client request type */
    GOAL_HTTP_HDL_T     hdlRes;                 /**< resource handle */
    GOAL_HTTP_HDL_T     hdlTransm;              /**< transmission handle */
    GOAL_HTTP_STATUS_T  *pRet;                  /**< pointer to return value */
    struct {
      uint8_t           *pData;                 /**< pointer to data */
      uint32_t          lenData;                /**< data length */
      char              *pUrl;                  /**< URL string */
      uint32_t          lenUrl;                 /**< URL string length */
      uint32_t          lenMatch;               /**< parsed URL length */
    } cs;                                       /**< data flow: client > server */
    struct {
      uint32_t          lenBuf;                 /**< buffer length */
      GOAL_HTTP_CONTENT_T *pContType;           /**< content type */
      const uint8_t     **ppData;               /**< address of data pointer */
      uint32_t          *pLenData;              /**< pointer to length var */
    } sc;                                       /**< data flow: server > client */
} GOAL_HTTP_APPLCB_DATA_T;

/** application template callback information struct */
typedef struct {
    GOAL_HTTP_T         *pInst;                 /**< instance pointer */
    GOAL_HTTP_HDL_T     hdlRes;                 /**< resource handle */
    struct {
        char*           name;                   /**< template name */
        uint32_t        retLenMax;              /**< maximum return length */
        GOAL_HTTP_TMPMGR_L_PATH_T *pPath;       /**< pointer to path struct */
    } in;
    struct {
        char*           strReturn;              /**< return string pointer */
    } out;
} GOAL_HTTP_APPLCB_TEMPL_T;

/** template manager list initialization struct */
typedef struct {
    uint32_t listId;                            /**< identifier of list */
    uint32_t cntMemb;                           /**< member count */
    char listName[32];                          /**< list name */
} GOAL_HTTP_TEMPLATE_LIST_INIT_T;

/** application callback type definition: resource access handler */
typedef GOAL_STATUS_T (*GOAL_HTTP_REQCB_T) (GOAL_HTTP_APPLCB_DATA_T *);
/** application callback type definition: template handler */
typedef GOAL_STATUS_T (*GOAL_HTTP_TMPCB_T) (GOAL_HTTP_APPLCB_TEMPL_T *);


/****************************************************************************/
/* Macros */
/****************************************************************************/

#define GOAL_HTTP_GET_RETURN_RAW(x, y, z) \
    if ((NULL == x) || (NULL == y)) { \
        goal_targetHalt(); \
    } else { \
        *(x->sc.pLenData) = (uint32_t) z; \
        *(x->sc.ppData) = (const uint8_t *) y; \
        *(x->sc.pContType) = GOAL_HTTP_CONTENT_APP; \
        *(x->pRet) = GOAL_HTTP_STATUS_OK_11; \
    }

#define GOAL_HTTP_GET_RETURN_HTML(x, y, z) \
    if ((NULL == x) || (NULL == y)) { \
        goal_targetHalt(); \
    } else { \
        *(x->sc.pLenData) = (uint32_t) z; \
        *(x->sc.ppData) = (const uint8_t *) y; \
        *(x->sc.pContType) = GOAL_HTTP_CONTENT_HTML; \
        *(x->pRet) = GOAL_HTTP_STATUS_OK_11; \
    }

#define GOAL_HTTP_GET_RETURN_GIF(x, y, z) \
    if ((NULL == x) || (NULL == y)) { \
        goal_targetHalt(); \
    } else { \
        *(x->sc.pLenData) = (uint32_t) z; \
        *(x->sc.ppData) = (const uint8_t *) y; \
        *(x->sc.pContType) = GOAL_HTTP_CONTENT_GIF; \
        *(x->pRet) = GOAL_HTTP_STATUS_OK_11; \
    }

#define GOAL_HTTP_GET_RETURN_PNG(x, y, z) \
    if ((NULL == x) || (NULL == y)) { \
        goal_targetHalt(); \
    } else { \
        *(x->sc.pLenData) = (uint32_t) z; \
        *(x->sc.ppData) = (const uint8_t *) y; \
        *(x->sc.pContType) = GOAL_HTTP_CONTENT_PNG; \
        *(x->pRet) = GOAL_HTTP_STATUS_OK_11; \
    }

#define GOAL_HTTP_GET_RETURN_JPG(x, y, z) \
    if ((NULL == x) || (NULL == y)) { \
        goal_targetHalt(); \
    } else { \
        *(x->sc.pLenData) = (uint32_t) z; \
        *(x->sc.ppData) = (const uint8_t *) y; \
        *(x->sc.pContType) = GOAL_HTTP_CONTENT_JPG; \
        *(x->pRet) = GOAL_HTTP_STATUS_OK_11; \
    }

#define GOAL_HTTP_GET_RETURN_BMP(x, y, z) \
    if ((NULL == x) || (NULL == y)) { \
        goal_targetHalt(); \
    } else { \
        *(x->sc.pLenData) = (uint32_t) z; \
        *(x->sc.ppData) = (const uint8_t *) y; \
        *(x->sc.pContType) = GOAL_HTTP_CONTENT_BMP; \
        *(x->pRet) = GOAL_HTTP_STATUS_OK_11; \
    }

#define GOAL_HTTP_GET_RETURN_ICO(x, y, z) \
    if ((NULL == x) || (NULL == y)) { \
        goal_targetHalt(); \
    } else { \
        *(x->sc.pLenData) = (uint32_t) z; \
        *(x->sc.ppData) = (const uint8_t *) y; \
        *(x->sc.pContType) = GOAL_HTTP_CONTENT_ICO; \
        *(x->pRet) = GOAL_HTTP_STATUS_OK_11; \
    }

#define GOAL_HTTP_GET_RETURN_JAVASCRIPT(x, y, z) \
    if ((NULL == x) || (NULL == y)) { \
        goal_targetHalt(); \
    } else { \
        *(x->sc.pLenData) = (uint32_t) z; \
        *(x->sc.ppData) = (const uint8_t *) y; \
        *(x->sc.pContType) = GOAL_HTTP_CONTENT_JS; \
        *(x->pRet) = GOAL_HTTP_STATUS_OK_11; \
    }

#define GOAL_HTTP_GET_RETURN_CSS(x, y, z) \
    if ((NULL == x) || (NULL == y)) { \
        goal_targetHalt(); \
    } else { \
        *(x->sc.pLenData) = (uint32_t) z; \
        *(x->sc.ppData) = (const uint8_t *) y; \
        *(x->sc.pContType) = GOAL_HTTP_CONTENT_CSS; \
        *(x->pRet) = GOAL_HTTP_STATUS_OK_11; \
    }

#define GOAL_HTTP_RETURN_OK_204(x) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        if (x->sc.pLenData){*(x->sc.pLenData) = (uint32_t) 0;} \
        if (x->sc.ppData){*(x->sc.ppData) = (uint8_t *) NULL;} \
        if (x->sc.pContType){*(x->sc.pContType) = GOAL_HTTP_CONTENT_HTML;} \
        if (x->pRet){*(x->pRet) = GOAL_HTTP_STATUS_NO_CONT_11;} \
    }

#define GOAL_HTTP_RETURN_ERR_404(x) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        if (x->sc.pLenData){*(x->sc.pLenData) = (uint32_t) 0;} \
        if (x->sc.ppData){*(x->sc.ppData) = (uint8_t *) NULL;} \
        if (x->sc.pContType){*(x->sc.pContType) = GOAL_HTTP_CONTENT_HTML;} \
        if (x->pRet){*(x->pRet) = GOAL_HTTP_STATUS_NOT_FOUND_11;} \
    }

#define GOAL_HTTP_RETURN_ERR_403(x) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        if (x->sc.pLenData){*(x->sc.pLenData) = (uint32_t) 0;} \
        if (x->sc.ppData){*(x->sc.ppData) = (uint8_t *) NULL;} \
        if (x->sc.pContType){*(x->sc.pContType) = GOAL_HTTP_CONTENT_HTML;} \
        if (x->pRet){*(x->pRet) = GOAL_HTTP_STATUS_FORBID_11;} \
    }


/****************************************************************************/
/* Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_httpInit(
    void
);

GOAL_STATUS_T goal_httpNew(
    GOAL_HTTP_T **ppInst,                       /**< pointer to instance pointer */
    uint16_t port,                              /**< port to link instance to */
    uint16_t cntChn                             /**< channel count */
);

GOAL_STATUS_T goal_httpsNew(
    GOAL_HTTP_T **ppInst,                       /**< HTTPS instance */
    uint16_t port,                              /**< port to link instance to */
    uint16_t cntChn                             /**< channel count */
);

GOAL_STATUS_T goal_httpResReg(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint8_t *strUrl,                            /**< URL string */
    uint32_t allowMeth,                         /**< allowed methods for resource */
    GOAL_HTTP_REQCB_T pfnCbData,                /**< application callback */
    GOAL_HTTP_TMPCB_T pfnCbTemplate,            /**< template handler callback */
    GOAL_HTTP_HDL_T *pHdl                       /**< resource handle return */
);

GOAL_STATUS_T goal_httpTmpMgrNewList(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_TEMPLATE_LIST_INIT_T *pInit       /**< pointer to list init data */
);

GOAL_STATUS_T goal_httpAuthBasSetUserInfo(
    uint32_t            userLevel,              /**< user level */
    char                *strUser,               /**< user name string */
    char                *strPw                  /**< password string */
);

void goal_httpResMgrPrintListSort(
    GOAL_HTTP_T *pInst                          /**< instance pointer */
);

#include <goal_http_rpc.h>

#endif /* GOAL_HTTP_H */



/** @file
 *
 *  Http webserver Example
 *
 * This application runs a simple HTTP server that hosts a demo HTML page.
 * It can be accessed via web browser by browsing the IP address of this device.
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


#ifndef GOAL_HTTP_INT_H
#define GOAL_HTTP_INT_H

#include <goal_includes.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) (x = x)
#endif

#define GOAL_HTTP_STANDARD_404 "404 not found"
#define GOAL_HTTP_STANDARD_404_MES "<html><meta charset = \"utf-8\"><body><h2>404: The requested file cannot be found.</h2></body></html>"

#define GOAL_HTTP_STANDARD_INDEX_NAME          "/index.html"
#define GOAL_HTTP_STANDARD_INDEX_NAME_LEN      GOAL_STRLEN(GOAL_HTTP_STANDARD_INDEX_NAME)

#define GOAL_HTTP_STRLEN(x) (ARRAY_ELEMENTS(x)-1)

#define GOAL_HTTP_NAME(id, nr, str) id = nr
#define GOAL_HTTP_EXT_NAME(id, nr, str, cont) id = nr

#define HTTP_POST_MULTIP_BOUND  ((uint8_t) 70)  /**< multipart boundary string len according to RFC2046 */
#define HTTP_CHN_TMPBUF_LEN     (1600)          /**< temp buffer length */
#define HTTP_CHN_TEMPL_BUFLEN   (64)            /**< template replacement buffer length */

#define GOAL_HTTP_HDREND_STR   "\r\n\r\n"

#define GOAL_HTTP_CHNFLAG_PARSE    ((uint32_t)(1<<0))
#define GOAL_HTTP_CHNFLAG_FORWARD  ((uint32_t)(1<<1))
#define GOAL_HTTP_CHNFLAG_SENDRES  ((uint32_t)(1<<2))
#define GOAL_HTTP_CHNFLAG_CLOSCHN  ((uint32_t)(1<<3))
#define GOAL_HTTP_CHNFLAG_CONTINUE ((uint32_t)(1<<4))

#ifndef GOAL_CONFIG_HTTP
#   define GOAL_CONFIG_HTTP 0
#endif

#ifndef GOAL_CONFIG_HTTPS
#   define GOAL_CONFIG_HTTPS 0
#endif

#ifndef GOAL_CONFIG_GEN_CTC
#   define GOAL_CONFIG_GEN_CTC 0
#endif

#ifndef GOAL_CONFIG_GEN_CTC_AC
#   define GOAL_CONFIG_GEN_CTC_AC 0
#endif

#ifndef GOAL_CONFIG_GEN_CTC_CC
#   define GOAL_CONFIG_GEN_CTC_CC 0
#endif


/****************************************************************************/
/* HTTP tables */
/****************************************************************************/

#define GOAL_HTTP_STATUS                                                                            \
                /*  id,                                     nr, status string */                    \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_OK,                    0, "HTTP/1.0 200 OK"),                    \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_CREATED,               1, "HTTP/1.0 201 Created"),               \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_ACCEPTED,              2, "HTTP/1.0 202 Accepted"),              \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_NO_CONT,               3, "HTTP/1.0 204 No Content"),            \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_NOT_FOUND,             4, "HTTP/1.0 404 File not found"),        \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_BAD_REQUEST,           5, "HTTP/1.0 400 Bad Request"),           \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_NOT_IMPL,              6, "HTTP/1.0 501 Not Implemented"),       \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_CONTINUE_11,           7, "HTTP/1.1 100 Continue"),              \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_OK_11,                 8, "HTTP/1.1 200 OK"),                    \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_NO_CONT_11,            9, "HTTP/1.1 204 No Content"),            \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_BAD_REQ_11,           10, "HTTP/1.1 400 Bad Request"),           \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_UNAUTH_11,            11, "HTTP/1.1 401 Unauthorized"),          \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_PAY_11,               12, "HTTP/1.1 402 Payment Required"),      \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_FORBID_11,            13, "HTTP/1.1 403 Forbidden"),             \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_NOT_FOUND_11,         14, "HTTP/1.1 404 Not Found"),             \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_NOT_IMPL_11,          15, "HTTP/1.1 501 Not Implemented"),       \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_CONTENT_LENGTH,       16, "\r\nContent-length: "),               \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_CONN_CLOSE,           17, "\r\nConnection: close"),              \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_CONN_KEEPALIVE,       18, "\r\nConnection: keep-alive"),         \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_SERVER,               19, "\r\nServer: HTTP_SERVER_AGENT"),      \
GOAL_HTTP_NAME(   GOAL_HTTP_STATUS_AUTH_CHALLENGE,       20, "\r\nWWW-Authenticate: Basic")

/** A list of extension-to-HTTP header strings (see outdated RFC 1700 MEDIA TYPES
 * and http://www.iana.org/assignments/media-types for registered content types
 * and subtypes) */


#define GOAL_HTTP_CONTENT                                                                                                \
                /*  id,                               nr,         content-type string */                                  \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_UNK,            0,         "\r\nContent-type: Unknown"),                       \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_HTML,           1,         "\r\nContent-type: text/html"),                     \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_SSI,            2,         "\r\nContent-type: text/html\r\nExpires: Fri, 10 Apr 2008 14:00:00 GMT\r\nPragma: no-cache"), \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_GIF,            3,         "\r\nContent-type: image/gif"),                     \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_PNG,            4,         "\r\nContent-type: image/png"),                     \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_JPG,            5,         "\r\nContent-type: image/jpeg"),                    \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_BMP,            6,         "\r\nContent-type: image/bmp"),                     \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_ICO,            7,         "\r\nContent-type: image/x-icon"),                  \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_APP,            8,         "\r\nContent-type: application/octet-stream"),      \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_JS,             9,         "\r\nContent-type: application/javascript"),        \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_RAM,           10,         "\r\nContent-type: application/javascript"),        \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_CSS,           11,         "\r\nContent-type: text/css"),                      \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_SWF,           12,         "\r\nContent-type: application/x-shockwave-flash"), \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_XML,           13,         "\r\nContent-type: text/xml"),                      \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_PDF,           14,         "\r\nContent-type: application/pdf"),               \
GOAL_HTTP_NAME(   GOAL_HTTP_CONTENT_DEFAULT_TYPE,  15,         "\r\nContent-type: text/plain")                     \


#define GOAL_HTTP_METHOD                                                        \
                /*  id,                         nr,         methode string */   \
GOAL_HTTP_NAME(   GOAL_HTTP_METHOD_NONE,      0,         ""),                   \
GOAL_HTTP_NAME(   GOAL_HTTP_METHOD_GET,       1,         "GET"),                \
GOAL_HTTP_NAME(   GOAL_HTTP_METHOD_POST,      2,         "POST"),               \
GOAL_HTTP_NAME(   GOAL_HTTP_METHOD_HEAD,      3,         "HEAD"),               \
GOAL_HTTP_NAME(   GOAL_HTTP_METHOD_PUT,       4,         "PUT"),                \
GOAL_HTTP_NAME(   GOAL_HTTP_METHOD_PATCH,     5,         "PATCH"),              \
GOAL_HTTP_NAME(   GOAL_HTTP_METHOD_DELETE,    6,         "DELETE"),             \
GOAL_HTTP_NAME(   GOAL_HTTP_METHOD_TRACE,     7,         "TRACE"),              \
GOAL_HTTP_NAME(   GOAL_HTTP_METHOD_OPTIONS,   8,         "OPTIONS"),            \
GOAL_HTTP_NAME(   GOAL_HTTP_METHOD_CONNECT,   9,         "CONNECT")

#define GOAL_HTTP_FILE_EXT                                                                                  \
                    /*  id,                         nr,         extension string,   content-type */         \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_UNK,     0,         ".*",               GOAL_HTTP_CONTENT_UNK),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_HTML,    1,         ".html",            GOAL_HTTP_CONTENT_HTML),   \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_HTM,     2,         ".htm",             GOAL_HTTP_CONTENT_HTML),   \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_SHTML,   3,         ".shtml",           GOAL_HTTP_CONTENT_HTML),   \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_SHTM,    4,         ".shtm",            GOAL_HTTP_CONTENT_HTML),   \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_SSI,     5,         ".ssi",             GOAL_HTTP_CONTENT_SSI),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_GIF,     6,         ".gif",             GOAL_HTTP_CONTENT_GIF),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_PNG,     7,         ".png",             GOAL_HTTP_CONTENT_PNG),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_JPG,     8,         ".jpg",             GOAL_HTTP_CONTENT_JPG),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_BMP,     9,         ".bmp",             GOAL_HTTP_CONTENT_BMP),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_ICO,    10,         ".ico",             GOAL_HTTP_CONTENT_ICO),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_CLASS,  11,         ".class",           GOAL_HTTP_CONTENT_APP),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_CLS,    12,         ".cls",             GOAL_HTTP_CONTENT_APP),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_JS,     13,         ".js",              GOAL_HTTP_CONTENT_JS),     \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_RAM,    14,         ".ram",             GOAL_HTTP_CONTENT_RAM),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_CSS,    15,         ".css",             GOAL_HTTP_CONTENT_CSS),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_SWF,    16,         ".swf",             GOAL_HTTP_CONTENT_SWF),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_XML,    17,         ".xml",             GOAL_HTTP_CONTENT_XML),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_XSL,    18,         ".xsl",             GOAL_HTTP_CONTENT_XML),    \
GOAL_HTTP_EXT_NAME(   GOAL_HTTP_FILE_EXT_PDF,    19,         ".pdf",             GOAL_HTTP_CONTENT_PDF)


/****************************************************************************/
/* Enumerates of HTTP tables */
/****************************************************************************/

typedef enum {GOAL_HTTP_STATUS} GOAL_HTTP_STATUS_T;
typedef enum {GOAL_HTTP_CONTENT} GOAL_HTTP_CONTENT_T;
typedef enum {GOAL_HTTP_METHOD} GOAL_HTTP_METHODS_T;
typedef enum {GOAL_HTTP_FILE_EXT} GOAL_HTTP_FILE_EXT_T;


/****************************************************************************/
/* Macros */
/****************************************************************************/

#define GOAL_HTTP_STATUS_OK(x) \
    (GOAL_BOOL_T) (((x <= GOAL_HTTP_STATUS_NO_CONT_11) &&   \
                    (x >= GOAL_HTTP_STATUS_OK_11)) ? GOAL_TRUE : GOAL_FALSE)

#define GOAL_HTTP_STATUS_ERR(x) \
    (GOAL_BOOL_T) ((x >= GOAL_HTTP_STATUS_BAD_REQ_11) ? GOAL_TRUE : GOAL_FALSE)


/****************************************************************************/
/* Forward Declarations */
/****************************************************************************/
struct GOAL_HTTP_T;                             /**< http instance struct */
struct GOAL_HTTP_CHN_T;                         /**< channel handler struct */


/****************************************************************************/
/* Data types */
/****************************************************************************/

#if GOAL_CONFIG_GEN_CTC_AC == 1
#define GOAL_HTTP_HDL_T uint32_t
#endif

#ifndef GOAL_HTTP_HDL_T
typedef void * GOAL_HTTP_HDL_T;                 /**< http handle */
#endif

#define GOAL_HTTP_GET_HDL(x)    \
    (GOAL_HTTP_HDL_T) ((PtrCast) x & 0xFFFFFFFF)

typedef struct goal_httpTableEntry {
    uint32_t cmd;
    char *strMes;
    uint32_t lenMes;
} goal_httpTableEntry;

typedef struct goal_httpExtTableEntry {
    uint32_t id;
    char *strMes;
    uint32_t lenMes;
    GOAL_HTTP_CONTENT_T content;
} goal_httpExtTableEntry;

typedef struct tHTTPHeader {
  const char *extension;
  const char *content_type;
} tHTTPHeader;

typedef struct GOAL_HTTP_FILE_T {
    char *strName;
    uint32_t lenName;
    GOAL_HTTP_FILE_EXT_T fileExt;
    char *strCont;
    uint32_t lenCont;
    struct GOAL_HTTP_FILE_T *pNext;
} GOAL_HTTP_FILE_T;

/** response type identifier */
typedef enum {
    GOAL_HTTP_RES_IMMEDT = 0,                  /**< response should be sent immediately */
    GOAL_HTTP_RES_LATER                        /**< response should be sent later */
} GOAL_HTTP_RESTYP_T;

/** http channel request types */
typedef enum {
    HTTP_REQ_GET,                              /**< GET request */
    HTTP_REQ_POST,                             /**< POST request */
} GOAL_HTTP_REQCHN_T;

/** http core request structure */
typedef struct {
    GOAL_HTTP_METHODS_T    method;             /**< http request method */
    GOAL_HTTP_FILE_EXT_T   fileExt;            /**< file extension */
    GOAL_HTTP_HDL_T        hdlRes;             /**< resource handle */
} GOAL_HTTP_REQ_T;

/** application return info */
typedef struct {
    GOAL_HTTP_HDL_T         hdlRes;             /**< resource handle */
    const uint8_t           *pData;             /**< data pointer */
    uint32_t                lenData;            /**< data length */
    GOAL_HTTP_STATUS_T      retState;           /**< return state */
    GOAL_HTTP_CONTENT_T     retContent;         /**< return content */
} GOAL_HTTP_APPLRET_T;

/** parser info struct */
typedef struct {
    struct {
        struct GOAL_HTTP_CHN_T *pHttpChan;      /**< HTTP channel pointer */
        uint8_t                 *strMes;        /**< messages to parse */
        uint32_t                lenMes;         /**< length of messages */
        goal_httpTableEntry     *pMethTable;    /**< pointer to http method table */
        uint32_t                lenMethTable;   /**< length of method table */
        goal_httpExtTableEntry  *pFileExtTable; /**< pointer to file extension table */
        uint32_t                lenFileExtTable;/**< length of file extension table */
    } in;

    struct {
        GOAL_HTTP_METHODS_T     method;         /**< http method return */
        GOAL_HTTP_FILE_EXT_T    fileExt;        /**< file extension */
        GOAL_HTTP_HDL_T         hdlRes;         /**< resource handle return */
        void                    *pfnCb;         /**< callback function */
        uint8_t                 *pData;         /**< data pointer */
        uint32_t                lenData;        /**< data length */
        uint32_t                lenParsed;      /**< parsed length */
        char*                   strUrl;         /**< URL string */
        uint32_t                lenUrl;         /**< URL length */
        uint32_t                lenMatch;       /**< URL matching length */
    } out;
} GOAL_HTTP_PARSE_T;

/** http response fragmentation struct */
typedef struct {
    GOAL_BOOL_T     flag;                       /**< fragmentation bool */
    const uint8_t   *pData;                     /**< pointer to data buffer */
    uint32_t        lenData;                    /**< data buffer complete length */
    uint32_t        srcOff;                     /**< source data offset */
} GOAL_HTTP_RESFRAG_T;

/** http request fragmentation struct */
typedef struct {
    GOAL_BOOL_T             flag;               /**< fragmentation bool */
    GOAL_BOOL_T             frstBound;          /**< first boundary found */
    GOAL_BOOL_T             lastBound;          /**< last boundary found */
    void                    *pfnAppCb;          /**< application callback function */
    GOAL_HTTP_HDL_T         hdlRes;             /**< resource handle */
    uint32_t                lenHead;            /**< length of header */
    uint32_t                lenFoot;            /**< length of footer */
    uint32_t                lenContent;         /**< length of complete content */
    uint32_t                lenRemain;          /**< length of remaining data */
    uint8_t boundary[HTTP_POST_MULTIP_BOUND];   /**< boundary string container */
} GOAL_HTTP_REQFRAG_T;

/** http CTC handling fragmentation struct */
typedef struct {
    GOAL_BOOL_T             getFrag;            /**< GET data direction fragmentation */
    GOAL_HTTP_HDL_T         hdlRes;             /**< recent resource handle */
    uint32_t                cntFrag;            /**< recent count of fragments */
    uint32_t                cntFragTot;         /**< total count of fragments */
} GOAL_HTTP_CTC_FRAG_T;

/** channel handler struct */
typedef struct GOAL_HTTP_CHN_T {
    uint32_t                id;                 /**< channel id */
    GOAL_NET_CHAN_T         *pChan;             /**< pointer to channel info */
    GOAL_BUFFER_T           *pRes;              /**< pointer to response buffer */
    GOAL_HTTP_RESTYP_T      resType;            /**< response type */
    GOAL_HTTP_STATUS_T      httpStatus;         /**< http status of channel */
    GOAL_HTTP_RESFRAG_T     resFrag;            /**< response (GET) fragmentation info */
    GOAL_HTTP_REQFRAG_T     reqFrag;            /**< request (POST) fragmentation info */
    GOAL_HTTP_CTC_FRAG_T    ctcFrag;            /**< CTC fragmentation */
    GOAL_BOOL_T             inUse;              /**< channel handler usage bool */
    GOAL_BOOL_T             rdySend;            /**< ready to send flag */
    GOAL_BOOL_T             pend;               /**< send buffer is pending */
    uint32_t                chnState;           /**< channel state */
    uint32_t                lenData;            /**< data length */
    uint8_t                 templateBuffer[HTTP_CHN_TEMPL_BUFLEN]; /**< template replacement buffer */
    uint8_t                 tmpBuf[HTTP_CHN_TMPBUF_LEN]; /**< temporary buffer */
    uint8_t                 cntSendTry;         /**< send retry counter */
    struct GOAL_HTTP_T      *pInst;             /**< owning instance of  */
} GOAL_HTTP_CHN_T;

/** http instance struct */
typedef struct GOAL_HTTP_T {
    struct GOAL_INSTANCE_T  hdr;                /**< instance header */
    GOAL_NET_CHAN_T        *pTcpListener;       /**< TCP Listener channel */
    uint16_t                localPort;          /**< tcp port */
    uint16_t                cntChn;             /**< channel count */
    GOAL_LOCK_T*            lockChn;            /**< channel lock */
    GOAL_HTTP_CHN_T*        pChanHdl;           /**< pointer to channel handler */
    uint32_t                cntResHdlInUse;     /**< count resource handles in use */
    struct HTTP_RESMGR_LL_RES_T *pResMgrRes;    /**< pointer to resources */
    struct GOAL_HTTP_TMPMGR_LL_T *pTemplateList; /**< pointer to template list */
    struct GOAL_HTTP_TMPMGR_L_PATH_T *pTemplatePath; /**< pointer to template path */
} GOAL_HTTP_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_httpAddToBuffer(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    const uint8_t *strMes,                      /**< message for sending */
    uint32_t len                                /**< length of message */
);

GOAL_STATUS_T goal_httpChnSetReqInfo(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_REQFRAG_T *pReqFrag               /**< pointer to request information */
);

GOAL_STATUS_T goal_httpChnGetReqInfo(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_REQFRAG_T *pReqFrag               /**< pointer to fragmentation info */
);

GOAL_BOOL_T goal_httpChnState_get(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    uint32_t flag                               /**< channel flag */
);

GOAL_STATUS_T goal_httpChnState_set(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    uint32_t flag                               /**< channel flag */
);

GOAL_STATUS_T goal_httpChnState_reset(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    uint32_t flag                               /**< channel flag */
);

GOAL_STATUS_T goal_httpChnSetResInfo(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    GOAL_HTTP_RESFRAG_T *pResFrag               /**< pointer to response info */
);

#endif /* GOAL_HTTP_INT_H */

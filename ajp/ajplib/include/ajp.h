/* Copyright 1999-2004 The Apache Software Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AJP_H
#define AJP_H

#include "apr_version.h"
#include "apr.h"

#include "apr_hooks.h"
#include "apr_lib.h"
#include "apr_strings.h"
#include "apr_buckets.h"
#include "apr_md5.h"
#include "apr_network_io.h"
#include "apr_pools.h"
#include "apr_strings.h"
#include "apr_uri.h"
#include "apr_date.h"
#include "apr_fnmatch.h"
#define APR_WANT_STRFUNC
#include "apr_want.h"

#if APR_HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if APR_HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#define AJP13_DEF_HOST "127.0.0.1"
#ifdef NETWARE
#define AJP13_DEF_PORT 9009     /* default to 9009 since 8009 is used by OS */
#else
#define AJP13_DEF_PORT 8009
#endif


#if APR_CHARSET_EBCDIC

#define USE_CHARSET_EBCDIC
#define ajp_xlate_to_ascii(b, l) ap_xlate_proto_to_ascii(b, l)
#define ajp_xlate_from_ascii(b, l) ap_xlate_proto_from_ascii(b, l)

#else                           /* APR_CHARSET_EBCDIC */

#define ajp_xlate_to_ascii(b, l) 
#define ajp_xlate_from_ascii(b, l) 

#endif

#ifdef AJP_USE_HTTPD_WRAP
#include "httpd_wrap.h"
#else
#include "httpd.h"
#include "http_config.h"
#include "http_request.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_main.h"
#include "http_log.h"
#endif


/** AJP Specific error codes
 */
/** Buffer overflow exception */
#define AJP_EOVERFLOW           (APR_OS_START_USERERR + 1) 
/** Destination Buffer is to small */
#define AJP_ETOSMALL            (APR_OS_START_USERERR + 2) 
/** Invalid input parameters */
#define AJP_EINVAL              (APR_OS_START_USERERR + 3) 
/** Bad message signature */
#define AJP_EBAD_SIGNATURE      (APR_OS_START_USERERR + 4) 
/** Incoming message too bg */
#define AJP_ETOBIG              (APR_OS_START_USERERR + 5) 
/** Missing message header */
#define AJP_ENO_HEADER          (APR_OS_START_USERERR + 6) 
/** Bad message header */
#define AJP_EBAD_HEADER         (APR_OS_START_USERERR + 7) 
/** Bad message */
#define AJP_EBAD_MESSAGE        (APR_OS_START_USERERR + 8) 
/** Cant log via AJP14 */
#define AJP_ELOGFAIL            (APR_OS_START_USERERR + 9) 

/** A structure that represents ajp message */ 
typedef struct ajp_msg ajp_msg_t;

/** A structure that represents ajp message */ 
struct ajp_msg
{
    /** The buffer holding a AJP message */ 
    apr_byte_t  *buf;
    /** The length of AJP message header (defaults to AJP_HEADER_LEN) */ 
    apr_size_t  header_len;
    /** The length of AJP message */ 
    apr_size_t  len;
    /** The current read position */ 
    apr_size_t  pos;
    /** Flag indicating the origing of the message */ 
    int         server_side;
};

/**
 * @defgroup AJP defines 
 * @{
 */
/**
 * Signature for the messages sent from Apache to tomcat
 */
#define AJP13_WS_HEADER             0x1234
#define AJP_HEADER_LEN              4
#define AJP_HEADER_SZ_LEN           2
#define AJP_MSG_BUFFER_SZ           (8*1024)
#define AJP13_MAX_SEND_BODY_SZ      (AJP_MSG_BUFFER_SZ - 6)

/** Send a request from web server to container*/
#define CMD_AJP13_FORWARD_REQUEST   (unsigned char)2
/** Write a body chunk from the servlet container to the web server */
#define CMD_AJP13_SEND_BODY_CHUNK   (unsigned char)3
/** Send response headers from the servlet container to the web server. */
#define CMD_AJP13_SEND_HEADERS      (unsigned char)4
/** Marks the end of response. */
#define CMD_AJP13_END_RESPONSE      (unsigned char)5
/** XXX: Have no idea */
#define CMD_AJP13_GET_BODY_CHUNK    (unsigned char)6
/** Webserver ask container to take control (logon phase) */
#define CMD_AJP13_PING              (unsigned char)8
/** Container response to cping request */
#define CMD_AJP13_CPONG             (unsigned char)9
/** Webserver check if container is alive, since container should respond by cpong */
#define CMD_AJP13_CPING             (unsigned char)10

/** @} */

/**
 * Check a new AJP Message by looking at signature and return its size
 *
 * @param msg       AJP Message to check
 * @param len       Pointer to returned len
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_check_header(ajp_msg_t *msg, apr_size_t *len);

/**
 * Reset an AJP Message
 *
 * @param msg       AJP Message to reset
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_reset(ajp_msg_t *msg);

/**
 * Mark the end of an AJP Message
 *
 * @param msg       AJP Message to end
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_end(ajp_msg_t *msg);

/**
 * Add an unsigned 32bits value to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     value to add to AJP Message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_uint32(ajp_msg_t *msg, apr_uint32_t value);

/**
 * Add an unsigned 16bits value to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     value to add to AJP Message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_uint16(ajp_msg_t *msg, apr_uint16_t value);

/**
 * Add an unsigned 8bits value to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     value to add to AJP Message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_uint8(ajp_msg_t *msg, apr_byte_t value);

/**
 *  Add a String in AJP message, and transform the String in ASCII 
 *  if convert is set and we're on an EBCDIC machine    
 *
 * @param msg       AJP Message to get value from
 * @param value     Pointer to String
 * @param convert   When set told to convert String to ASCII
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_cvt_string(ajp_msg_t *msg, const char *value,
                                       int convert);
/**
 *  Add a String in AJP message, and transform 
 *  the String in ASCII if we're on an EBCDIC machine    
 *
 * @param msg       AJP Message to get value from
 * @param value     Pointer to String
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_string(ajp_msg_t *msg, const char *value);

/**
 *  Add a String in AJP message. 
 *
 * @param msg       AJP Message to get value from
 * @param value     Pointer to String
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_ascii_string(ajp_msg_t *msg, const char *value);

/**
 * Add a Byte array to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     Pointer to Byte array
 * @param valuelen  Byte array len
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_bytes(ajp_msg_t *msg, const apr_byte_t *value,
                                  apr_size_t valuelen);

/**
 * Get a 32bits unsigned value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_get_uint32(ajp_msg_t *msg, apr_uint32_t *rvalue);

/**
 * Get a 16bits unsigned value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_get_uint16(ajp_msg_t *msg, apr_uint16_t *rvalue);

/**
 * Peek a 16bits unsigned value from AJP Message, position in message
 * is not updated
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_peek_uint16(ajp_msg_t *msg, apr_uint16_t *rvalue);

/**
 * Get a 8bits unsigned value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_get_byte(ajp_msg_t *msg, apr_byte_t *rvalue);

/**
 * Peek a 8bits unsigned value from AJP Message, position in message
 * is not updated
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_peek_byte(ajp_msg_t *msg, apr_byte_t *rvalue);

/**
 * Get a String value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_get_string(ajp_msg_t *msg, char **rvalue);


/**
 * Get a Byte array from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @param rvalueLen Pointer where Byte array len will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_get_bytes(ajp_msg_t *msg, apr_byte_t **rvalue,
                               apr_size_t *rvalue_len);

/**
 * Create an AJP Message from pool
 *
 * @param pool      memory pool to allocate AJP message from
 * @param rmsg      Pointer to newly created AJP message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_create(apr_pool_t *pool, void **rmsg);

/**
 * Recopy an AJP Message to another
 *
 * @param smsg      source AJP message
 * @param dmsg      destination AJP message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_copy(ajp_msg_t *smsg, ajp_msg_t *dmsg);

/**
 * Serialize in an AJP Message a PING command
 *
 * +-----------------------+
 * | PING CMD (1 byte)     |
 * +-----------------------+
 *
 * @param smsg      AJP message to put serialized message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_serialize_ping(ajp_msg_t *msg);

/** 
 * Serialize in an AJP Message a CPING command
 *
 * +-----------------------+
 * | CPING CMD (1 byte)    |
 * +-----------------------+
 *
 * @param smsg      AJP message to put serialized message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_serialize_cping(ajp_msg_t *msg);

/**
 * Dump up to the first 1024 bytes on an AJP Message
 *
 * @param msg       AJP Message to dump
 * @param err       error string to display
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_dump(ajp_msg_t *msg, char *err);

/** 
 * Send an AJP message to backend
 *
 * @param soct      backend socket
 * @param smsg      AJP message to put serialized message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_ilink_send(apr_socket_t *sock, ajp_msg_t *msg);

/** 
 * Receive an AJP message from backend
 *
 * @param sock      backend socket
 * @param smsg      AJP message to put serialized message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_ilink_receive(apr_socket_t *sock, ajp_msg_t *msg);

/**
 * Build the ajp header message and send it
 * @param sock      backend socket
 * @param r         current request
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_send_header(apr_socket_t *sock, request_rec  *r);

/**
 * Read the ajp message and return the type of the message.
 * @param sock      backend socket
 * @param r         current request
 * @param msg       returned AJP message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_read_header(apr_socket_t *sock,
                             request_rec  *r,
                             ajp_msg_t **msg);

/**
 * Allocate a msg to send data
 * @param r         current request
 * @param ptr       data buffer
 * @param len       the length of allocated data buffer
 * @param data      returned AJP message
 * @return          APR_SUCCESS or error
 */
apr_status_t  ajp_alloc_data_msg(request_rec *r, char **ptr, apr_size_t *len,
                                 ajp_msg_t **data);

/**
 * Send the data message
 * @param sock      backend socket
 * @param r         current request
 * @param data      AJP message to send
 * @param len       AJP message length      
 * @return          APR_SUCCESS or error
 */
apr_status_t  ajp_send_data_msg(apr_socket_t *sock, request_rec  *r,
                                ajp_msg_t *data, apr_size_t len);

/**
 * Parse the message type 
 * @param r         current request
 * @param msg       AJP message
 * @return          AJP message type.
 */
int ajp_parse_type(request_rec  *r, ajp_msg_t *msg);

/**
 * Parse the headers 
 * @param r         current request
 * @param msg       AJP message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_parse_headers(request_rec  *r, ajp_msg_t *msg);

/** parse the header and return data address and length 
 * @param r         current request
 * @param msg       AJP message
 * @param len       returned AJP message length 
 * @param ptr       returned data
 * @return          APR_SUCCESS or error
 */
apr_status_t  ajp_parse_data(request_rec  *r, ajp_msg_t *msg,
                             apr_uint16_t *len, char **ptr);

#endif /* AJP_H */


#ifndef __OS_SOCKET_H__
#define __OS_SOCKET_H__

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************SOCKET**********/
#define OS_SOCKET_EFAIL         -1900
#define OS_SOCKET_ECONNECT      -1901
#define OS_SOCKET_EADDR         -1902
#define OS_SOCKET_ECERT_VERIFY  -1903


#define SOCKET_NUMS         8

/*******************************************
 * socket协议类型
*******************************************/
typedef enum SOCKET_PROTOCOL_TYPE
{
    SOCKET_PRT_TCP = 0,
    SOCKET_PRT_UDP = 1,
    SOCKET_PRT_SSL = 2,
} SOCKET_TYPE;

// SOCKET error
typedef struct
{
    uint32  addrType;       // 0 ipv4 ip 1 ipv6 ip  2 url
    uint32  port;           //
    char    router;         //only support 0
    char    RFU[3];
    char *   addr;          //
} ST_NET_SOCKET;

#define COMM_SSL_SSL3_0             1   /*!< SSL v3.0 */
#define COMM_SSL_TLS1_0             2   /*!< TLS v1.0 */
#define COMM_SSL_TLS1_1             3   /*!< TLS v1.1 */
#define COMM_SSL_TLS1_2             4   /*!< TLS v1.2 */
#define COMM_SSL_TLS1_3             5   /*!< TLS v1.3 */

//SSL/TLS验证类型
#define COMM_VERIFY_AUTO               0   /*!< automatic */
#define COMM_VERIFY_NO              1   /*!< no verify */
#define COMM_VERIFY_ONE                2   /*!< 单向 */
#define COMM_VERIFY_DIDR               3   /*!< 双向 */


typedef enum
{
    SOCKET_S_IDLE = 0,
    SOCKET_S_DISCONNECT= 1,         //
    SOCKET_S_CONNECTING,               //
    SOCKET_S_CONNECTED              //
} SOCKET_STATE_ENUM;



/*******************************************
 * 读写类型
*******************************************/
typedef enum SOCKET_TRANS_TYPE
{
    SOCKET_TRANS_RECV = 0,
    SOCKET_TRANS_SEND = 1,
} SOCKET_TRANS_TYPE;

/*******************************************
 * select的结果类型，是否可读写
*******************************************/
typedef enum SOCKET_FD_ISSET
{
    SOCKET_FD_NO_ISSET = 0,
    SOCKET_FD_ISSET = 1,
} SOCKET_FD_ISSET_E;

typedef struct{
    char type;                        //证书格式类型,
    int ca_len;                        //证书长度
    const unsigned char *ca_buf;        //证书数据
    const char *pwd;            //证书中的common name
}COMM_CERT_INFO;

typedef struct{
    COMM_CERT_INFO *cacert;     //服务器证书数据
    COMM_CERT_INFO *clicert;     //客户端证书数据
    COMM_CERT_INFO *pkey;     //客户端私长度
    const char *srv_cn;             //服务器证书中的common name
    char tls_version;                //为0时自动适配
    char verify_flag;            //验证标志为0时:not support  1为不验证, 2为单向, 3为双向
    unsigned char RFU[10];
}COMM_SSL_CONFIG;

extern int32_t OS_socket_open(SOCKET_TYPE type, void* args, void * config, uint32 timeoutMs);
extern int32_t OS_socket_close(int32_t fd, uint32 force);
extern int32_t OS_socket_send(int32_t fd, void* data, uint32 len, uint32_t flags);
extern int32_t OS_socket_recv(int32_t fd, void* data, uint32 len, uint32 timeoutMs);
extern int32_t OS_socket_status(int32_t fd, uint32* status);
extern int32_t OS_socket_clean(int32_t fd, uint32 flags);




#ifdef __cplusplus
}
#endif

#endif //__OS_SOCKET_H__

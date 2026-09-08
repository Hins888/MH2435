#define DEBUG

#include "drivers.h"
#include "app.h"
#include "api.h"

#include "lwip\sockets.h"
#include "lwip\netif.h"
#include "lwip\dns.h"
#include "lwip\api.h"
#include "lwip\tcp.h"

//#include "cfg80211.h"
//#include "defs.h"
#include "type.h"
#include "os_socket.h"

#include "tcpapp.h"
#include "util_debug.h"
#include "lwip/dhcp.h"
#include "sys_misc.h"

#include "test_data.h"

//全局变量声明
extern int errno; //lwip错误号

/*
 * socket互斥锁，对socket的所有线程外的操作需要上锁
 * 如果不加锁，lwip会很不稳定，原因是lwip不支持多线程同时操作一个socket
 * 需要注意的是像send这样可能会阻塞的函数，如果加了锁将会降低数据
 * 收发的效率，所以收发数据时采用非阻塞方式要好些(MSG_DONTWAIT)
 */
extern mutex_t socket_mutex;

#include "mbedtls/platform.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/net_sockets.h"

#include <string.h>

#include "os_socket.h"
#include "http_parser.h"

#include "test_cert.c"

#define SERVER_PORT "443"
#define SERVER_NAME "www.easynetworking.cn"
#define GET_REQUEST "GET / HTTP/1.1\r\n\r\n"
static char *http_req ="GET / HTTP/1.1\r\n";


static char *http_body;
static unsigned int *http_bodySize;

#if 0
#define NetDev "TCP"
#define NET_CHANNLE   1
#define SERVER_URL_ADDR "93.184.216.34"
#define SERVER_URL_PORT 80

#define NetDev "TLS"
#define NET_CHANNLE   2
#define SERVER_URL_ADDR "49.234.56.78"
//#define SERVER_URL_ADDR "14.215.177.38"
#define SERVER_URL_PORT 443
#else
#define NET_CHANNLE   2
#define SERVER_URL_ADDR "192.168.10.36"
#define SERVER_URL_PORT 7888
#define SERVER_TCPIP_PORT 7777
#endif

int on_message_begin(http_parser* _) {
    (void)_;
    //printf("\n***MESSAGE BEGIN***\n\n");
    return 0;
}

int on_headers_complete(http_parser* _) {
    (void)_;
    //printf("\n***HEADERS COMPLETE***\n\n");
    return 0;
}

int on_message_complete(http_parser* _) {
    (void)_;
    //printf("\n***MESSAGE COMPLETE***\n\n");
    return 0;
}

int on_url(http_parser* _, const char* at, size_t length) {
  (void)_;
  //printf("\n***Url: %.*s\n", (int)length, at);
  return 0;
}

int on_header_field(http_parser* _, const char* at, size_t length) {
    (void)_;
    //printf("\n***Header field: %.*s\n", (int)length, at);
    return 0;
}

int on_header_value(http_parser* _, const char* at, size_t length) {
    (void)_;
    //printf("\n***Header value: %.*s\n", (int)length, at);
    return 0;
}

int on_body(http_parser* _, const char* at, size_t length) {
    (void)_;
    //printf("\n***Body: %.*s \n***\n", (int)length, at);
    if((*http_bodySize) >length) *http_bodySize=length;
    memcpy(http_body, at, *http_bodySize);
    return 0;
}

void usage(const char* name) {

    ((void)name);
/*	printf( "Usage: %s $type $filename\n"
          "  type: -x, where x is one of {r,b,q}\n"
          "  parses file as a Response, reQuest, or Both\n",
          name);*/
}

int http_parse_test(void *addr, int port, char *req, char *respBody, unsigned int *respBodySize)
{
    int ret;
   // enum http_parser_type file_type;
    http_parser parser;
    #define resp_size   (8192)
    ST_NET_SOCKET ser_url;
    int size=0;
    int fd = -1, cnt = 0;
    unsigned int t0;
    size_t nparsed;
    
    char* data = mem_malloc(resp_size);
    if (data == NULL) {
        printf("couldn't read entire file\n");
        return OS_ENOMEM;
    }

    memset(data, 0x00, resp_size);
    memset(&ser_url, 0x00, sizeof(ST_NET_SOCKET));
    ser_url.addr     = (char*)addr;
    ser_url.port     =  port;
    ser_url.addrType = 0; // ipv4 url

#if (NET_CHANNLE == 2)
    COMM_SSL_CONFIG tls_config;
    COMM_CERT_INFO cacert;
    memset(&tls_config, 0x00, sizeof(COMM_SSL_CONFIG));
    memset(&cacert, 0x00, sizeof(COMM_CERT_INFO));

    cacert.ca_buf = ( const unsigned char *)mbedtls_test_example_pem;
    cacert.ca_len = mbedtls_test_example_pem_len;
    tls_config.cacert = &cacert;
    tls_config.clicert = NULL;
    tls_config.pkey = NULL;
    tls_config.tls_version =COMM_SSL_TLS1_2;
    tls_config.verify_flag = 2;
    tls_config.RFU[0] = 1;

    ret = OS_socket_open(SOCKET_PRT_SSL, (void*)&ser_url, &tls_config, 8000);
#else
    ret = OS_socket_open(SOCKET_PRT_TCP, (void*)&ser_url, NULL, 5000);
#endif
    if (ret < 0)
    {
        p_dbg("http url open socket error:%d", ret);
        ret =  OS_SOCKET_ECONNECT;
        goto _errback;
    }

    fd = ret;
    cnt=0;
    size=strlen((char *)req);
    p_dbg("HTTP open  %d Data send len %d\r\n", fd, size);
    t0 = bsp_get_tick();
    while(1)
    {
        ret = OS_socket_send(fd, (unsigned char *)(req+cnt), size-cnt, 1);
        if(ret<0)
        {
            p_dbg("CommSend Error:%d\r\n",ret);
            goto _errback;
        }

        cnt+=ret;
        if(cnt>=size) break;
        if((bsp_get_tick() - t0)>10000)
        {
            p_dbg("CommSend TIMEOUT\r\n");
            ret =  OS_ETIMEOUT;
            goto _errback;
        }
    }

    p_dbg("HTTP Req %d Data send ok %d\r\n", size, cnt);
    cnt = 0;
    ret = OS_socket_recv(fd, (unsigned char *)(data), resp_size, 5000);
    if(ret <= 0)
    {
        p_dbg("HTTP Recv Data error %d\r\n", ret);
        goto _errback;
    }

    cnt += ret;
    size = 0;
    t0 = bsp_get_tick();
    while(cnt < resp_size)
    {
        ret = OS_socket_recv(fd, (unsigned char *)(data + cnt), resp_size-cnt,100);
        if(ret==0)
        {
             if(size++>10)break ;//300ms timeout
        }
        if(ret<0)
        {
            p_dbg("CommRecv Error:%d\r\n",ret);
            ret =  OS_ETIMEOUT;
            goto _errback;
        }
        cnt+=ret;
    }
    if(cnt == 0) { p_dbg("SOCKET Recv Data Error len=0\r\n"); ret = OS_EIO; goto _errback; }

    data[cnt]=0;
    p_dbg("\r\n len=%d  recv:%s \r\n", cnt, data);
    OS_socket_close(fd, 1);
    fd = -1;

    http_body=respBody;
    http_bodySize=respBodySize;
#if 1
    http_parser_settings settings;
    memset(&settings, 0, sizeof(settings));
    settings.on_message_begin = on_message_begin;
    settings.on_url = on_url;
    settings.on_header_field = on_header_field;
    settings.on_header_value = on_header_value;
    settings.on_headers_complete = on_headers_complete;
    settings.on_body = on_body;
    settings.on_message_complete = on_message_complete;
#endif
     p_dbg("\r\n %d Start parse http pack\r\n", __LINE__);
    http_parser_init(&parser, HTTP_RESPONSE);
    nparsed = http_parser_execute(&parser, NULL, data, cnt);
    p_dbg("recv len=%d  parsed len :%d result=%d\r\n", cnt, nparsed, parser.status_code);
    if (nparsed != (size_t)cnt) {
        printf("Error: %s (%s)\n",
                http_errno_description(HTTP_PARSER_ERRNO(&parser)),
                http_errno_name(HTTP_PARSER_ERRNO(&parser)));
        ret = parser.status_code;
        goto _errback;
    }

    bsp_delayms(1000);
    ret =  EXIT_SUCCESS;

_errback:
    p_dbg("http parser end error =%d  parsed len :%d result=%d\r\n", ret, cnt, nparsed);
    if(data) { p_dbg("free memery %p\r\n", data); mem_free(data); }
    if(fd >= 0) OS_socket_close(fd, 1);
    bsp_delayms(1000);
    return parser.status_code;
}

int test_http(void)
{
    int ret;
    char buffer[16*1024];
    unsigned int len;
    memset(buffer, 0x00, sizeof(buffer));
    len = sizeof(buffer);

    p_dbg("Http parse start test %d\r\n", bsp_get_tick());
    ret = http_parse_test(SERVER_URL_ADDR, SERVER_URL_PORT, http_req, buffer, (unsigned int *)&len);
    //ret = http_parse_test(SERVER_URL_ADDR, SERVER_URL_PORT, bd_http_req, buffer, (unsigned int *)&len);

    p_dbg("Http parse end result %d\r\n", ret);
    return 0;
}


#define DEBUG_LEVEL 1


void mbedtls_pal_net_free( void *ctx, int force );
int mbedtls_pal_net_send( void *ctx, const unsigned char *buf, size_t len );
int mbedtls_pal_net_recv_timeout( void *ctx, unsigned char *buf, size_t len, uint32_t timeout);

static void my_debug(void *ctx, int level,
              const char *file, int line,
              const char *str)
{
    printf("%04d: |%d| %s", line, level, str);
    //fflush((FILE *) ctx);
}

mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_ssl_context ssl;
mbedtls_ssl_config conf;
mbedtls_x509_crt cacert;
mbedtls_net_context server_fd;

void mbedtls_demo(void* arg)
{
    int ret, len;
    uint32_t flags;
    unsigned char buf[1024];
    const char *pers = "ssl_client1";
    const char *srv_cn = "MEGAHUNT";

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold( 0 );
#endif

    /*
     * 0. Initialize the RNG and the session data
     */
#if defined(MBEDTLS_NET_C)
    mbedtls_net_init( &server_fd );
#else
    server_fd.fd = -1;
#endif
    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_config_init( &conf );
    mbedtls_x509_crt_init( &cacert );
    mbedtls_ctr_drbg_init( &ctr_drbg );

    mbedtls_printf("\n  . Seeding the random number generator...");

    mbedtls_entropy_init( &entropy );
    
//    psa_status_t status = psa_crypto_init();
//    if (status != 0) {
//        mbedtls_printf("Failed to initialize PSA Crypto implementation: %d\n",
//                        (int) status);
//        goto exit;
//    }
    
    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                                       (const unsigned char *) pers,
                                       strlen( pers ) ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    /*
     * 0. Initialize certificates
     */
    mbedtls_printf("  . Loading the CA root certificate ...");

    ret = mbedtls_x509_crt_parse( &cacert, (const unsigned char *) mbedtls_test_example_pem,
                                  mbedtls_test_example_pem_len );
    if( ret < 0 )
    {
        mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        goto exit;
    }

    mbedtls_printf(" ok (%d skipped)\n", ret);

    /*
     * 1. Start the connection
     */
    mbedtls_printf("  . Connecting to tcp/%s/%s...", SERVER_NAME, SERVER_PORT );
#if defined(MBEDTLS_NET_C)
    if( ( ret = mbedtls_net_connect( &server_fd, SERVER_NAME,
                                     SERVER_PORT, MBEDTLS_NET_PROTO_TCP ) ) != 0 )
    {
        mbedtls_printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret ));
        goto exit;
    }
#else
    ST_NET_SOCKET  ap_info;
    ap_info.addrType=0;
    ap_info.addr = (char *)SERVER_URL_ADDR;
    ap_info.port= SERVER_URL_PORT; //atoi(SERVER_URL_PORT);
    ret = OS_socket_open(SOCKET_PRT_TCP, (void *)&ap_info, NULL, 2000);
    if(ret<0)
    {
        mbedtls_printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
        goto exit;
    }
    server_fd.fd = ret;
#endif
    mbedtls_printf(" ok\n");

    /*
     * 2. Setup stuff
     */
    mbedtls_printf("  . Setting up the SSL/TLS structure...");
    if( ( ret = mbedtls_ssl_config_defaults( &conf,
                MBEDTLS_SSL_IS_CLIENT,
                MBEDTLS_SSL_TRANSPORT_STREAM,
                MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        goto exit;
    }

    mbedtls_printf(" ok\n");

    /* OPTIONAL is not optimal for security,
     * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_OPTIONAL );
    mbedtls_ssl_conf_ca_chain( &conf, &cacert, NULL );
    mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctr_drbg );
    mbedtls_ssl_conf_dbg( &conf, my_debug, NULL );

    if( ( ret = mbedtls_ssl_setup( &ssl, &conf ) ) != 0 )
    {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        goto exit;
    }

    if( ( ret = mbedtls_ssl_set_hostname( &ssl, srv_cn ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
        goto exit;
    }
#if defined(MBEDTLS_NET_C)
    mbedtls_ssl_set_bio( &ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL );
#else
    mbedtls_ssl_set_bio( &ssl, &server_fd, mbedtls_pal_net_send, NULL, mbedtls_pal_net_recv_timeout );
#endif
    /*
     * 4. Handshake
     */
    mbedtls_printf("  . Performing the SSL/TLS handshake...\n");

    while( ( ret = mbedtls_ssl_handshake( &ssl ) ) != 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
            goto exit;
        }
    }

    mbedtls_printf(" ok\n");

    /*
     * 5. Verify the server certificate
     */
    mbedtls_printf("  . Verifying peer X.509 certificate...");

    /* In real life, we probably want to bail out when ret != 0 */
    if( ( flags = mbedtls_ssl_get_verify_result( &ssl ) ) != 0 )
    {
        char vrfy_buf[512];

        mbedtls_printf(" failed = %d\n", flags);

        mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags );

        mbedtls_printf("%s\n", vrfy_buf);
    }
    else
        mbedtls_printf(" ok\n");

    /*
     * 3. Write the GET request
     */
    mbedtls_printf("  > Write to server:");
    len = sprintf( (char *) buf, GET_REQUEST );

    while( ( ret = mbedtls_ssl_write( &ssl, buf, len ) ) <= 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret );
            goto exit;
        }
    }

    len = ret;
    mbedtls_printf(" %d bytes written\n\n%s", len, (char *) buf );

    /*
     * 7. Read the HTTP response
     */
    mbedtls_printf("  < Read from server:");
    do
    {
        len = sizeof( buf ) - 1;
        memset( buf, 0, sizeof( buf ) );
        ret = mbedtls_ssl_read( &ssl, buf, len );

        if( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE )
            continue;

        if( ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY )
            break;

        if( ret < 0 )
        {
            mbedtls_printf("failed\n  ! mbedtls_ssl_read returned %d\n\n", ret);
            break;
        }

        if( ret == 0 )
        {
            mbedtls_printf("\n\nEOF\n\n" );
            break;
        }

        len = ret;
        mbedtls_printf(" %d bytes read:\n\n%s", len, (char *) buf );
    }
    while( 1 );

    mbedtls_ssl_close_notify( &ssl );

exit:

#ifdef MBEDTLS_ERROR_C
    if( ret != 0 )
    {
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100 );
        mbedtls_printf("Last error was: %d - %s\n\n", ret, error_buf );
    }
#endif
    mbedtls_printf("mbedtls test error Last error was: %d \n\n", ret );
#if defined(MBEDTLS_NET_C)
    mbedtls_net_free( &server_fd );
#else
    if(server_fd.fd >= 0) mbedtls_pal_net_free(&server_fd, 1);
#endif
    mbedtls_x509_crt_free( &cacert );
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_config_free( &conf );
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

    //mbedtls_psa_crypto_free();

    while(1) bsp_taskdelay(200);

    //return;
}


extern int tcpLoopTest(uint8_t tcp_type, void *argv);
int sslLoopTest(void)
{
    COMM_SSL_CONFIG tls_config;
    COMM_CERT_INFO cacert;
    memset(&tls_config, 0x00, sizeof(COMM_SSL_CONFIG));
    memset(&cacert, 0x00, sizeof(COMM_CERT_INFO));

    cacert.ca_buf = ( const unsigned char *)mbedtls_test_example_pem;
    cacert.ca_len = mbedtls_test_example_pem_len;
    tls_config.cacert = &cacert;
    tls_config.clicert = NULL;
    tls_config.pkey = NULL;
    tls_config.tls_version =COMM_SSL_TLS1_2;
    tls_config.verify_flag = 2;
    tls_config.RFU[0] = 1;
    tls_config.srv_cn = "MEGAHUNT";

    return tcpLoopTest(SOCKET_PRT_SSL, (void *)&tls_config);
}

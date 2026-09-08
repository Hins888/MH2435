/*
 * ssl_socket.c
 *
 *  Created on: 2021年9月14日
 */
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bsp_api.h"
#include "api.h"
#include "os_socket.h"

#include "lwip/netdb.h"


#define TLS_MBEDTLS

#ifdef  TLS_MBEDTLS
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/net_sockets.h"


#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define MBEDTLS_EXIT_SUCCESS    0
#define MBEDTLS_EXIT_FAILURE    1
#endif

#define DEBUG_LEVEL 1
#define SSL_SOCKET_MAX_NUMBER   4

#define mbedtls_dbg printf
#define mbed_dbg(fmt, args...)  \
        do { if (DEBUG_LEVEL) \
            {mbedtls_dbg("%s %d: "fmt"", __FUNCTION__,__LINE__,##args);} \
        } while(0)

#define mbed_info(fmt, args...)  \
        do { if (DEBUG_LEVEL  >2) \
            {mbedtls_dbg("%s %d: "fmt"", __FUNCTION__,__LINE__,##args);} \
        } while(0)


typedef struct TLSContext {
    int debug_level;
    volatile int connect;
    volatile int verify_tag;
    volatile int router;
    char srv_url[128];
    const char *pers;
    char cli_pwd[128];
    int pwd_len;
    mbedtls_net_context server_fd;
    mbedtls_ssl_context ssl_context;
    mbedtls_ssl_config ssl_config;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt *ca_cert;
    mbedtls_x509_crt *cli_cert;
    mbedtls_pk_context *pkey;
} SSLSocketRes;


typedef struct SslSocketContext {
    volatile int steps;
    SSLSocketRes *resource;
    volatile unsigned int stop;
}SSLSocketContex;

SSLSocketContex socket_context[SSL_SOCKET_MAX_NUMBER] = { {0, NULL, 0}, {0, NULL, 0}, {0, NULL, 0}, {0, NULL, 0}};

static int tls_socket_verify_cert(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags);
static void tls_socket_debug( void *ctx, int level,
                      const char *file, int line, const char *str );
static int tls_get_socket_context(int fd);
static int tls_alloc_socket_context(void);
static int tls_free_socket_context(unsigned int ssl_id);
static void tls_free_cert_memery(SSLSocketRes *ssl);
static int tls_drbg_random(void *ctx, unsigned char *out, unsigned int len);
void mbedtls_pal_net_free( void *ctx, int force);
int mbedtls_pal_net_send( void *ctx, const unsigned char *buf, size_t len);
int mbedtls_pal_net_recv( void *ctx, unsigned char *buf, size_t len);
int mbedtls_pal_net_recv_timeout( void *ctx, unsigned char *buf, size_t len, uint32_t timeout);

extern int32_t tcpip_socket_open(SOCKET_TYPE type, void* args, void * config, uint32 timeoutMs);
extern int32_t tcpip_socket_close(int32_t fd, uint32 force);
extern int32_t tcpip_socket_send(int32_t fd, void* data, uint32 len, uint32_t flags);
extern int32_t tcpip_socket_recv(int32_t fd, void* data, uint32 len, uint32 timeoutMs);
//extern int32_t tcpip_socket_status(int32_t fd, uint32* status);
//extern int32_t tcpip_socket_clean(int32_t fd, uint32 flags);

int ssl_reset_error(int error)
{
    int value;
    switch(error)
    {
    case -2700:
        value =  OS_SOCKET_ECERT_VERIFY;
        break;
    case -7280:
    default:
        value =  OS_SOCKET_ECONNECT;
        break;
    }
    return value;
}

int ssl_socket_open(void* args, void *config, unsigned int timeout)
{
    int ret = 1;
    int verify_mode = COMM_VERIFY_NO;
    COMM_SSL_CONFIG  *ssl_socket_cfg = (COMM_SSL_CONFIG * )config;
    ST_NET_SOCKET* net_url = (ST_NET_SOCKET*)args;
    int ssl_id=-1;
    int ssl_fd = -1;
    //unsigned int t0;

    mbedtls_dbg("ip info:%s %d router %d\r\n", net_url->addr, net_url->port, net_url->router);
    //mbedtls_dbg("ssl info: %d timeout %d\r\n", net_url->addrType, timeout);
    if((net_url == NULL)||(net_url->router != 0) || (net_url->addr == NULL) ) return OS_EINVAL;

    /******************* check param******************************/
    if(config == NULL)
    {
        ret = COMM_VERIFY_NO;
    }
    verify_mode = ssl_socket_cfg->verify_flag;

    ssl_id =  tls_alloc_socket_context();
    if(ssl_id < 0 ) return OS_EIO;

    mbedtls_dbg("mbedtls cert config ssl_id %d mode=%d \r\n", ssl_id, verify_mode);
    SSLSocketRes *ssl  = mem_malloc(sizeof(SSLSocketRes));
    if(ssl == NULL) return OS_ENOMEM;
    memset(ssl, 0x00, sizeof(SSLSocketRes));
    /*
     * 0. Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
    ssl->debug_level = ssl_socket_cfg->RFU[0];
    ssl->debug_level = 1;
    if(ssl->debug_level)
    {
        mbedtls_ssl_conf_dbg( &ssl->ssl_config, tls_socket_debug,  &ssl->debug_level );
    }
#endif

#if defined(MBEDTLS_NET_C)
    mbedtls_net_init( &ssl->server_fd );
#else
    ssl->server_fd.fd = -1;
#endif
    mbedtls_ssl_init( &ssl->ssl_context );
    mbedtls_ssl_config_init( &ssl->ssl_config );
    mbedtls_ctr_drbg_init(&ssl->ctr_drbg);
    mbedtls_entropy_init(&ssl->entropy);


    mbedtls_printf("\n  . Seeding the random number generator...");
    if ((ret = mbedtls_ctr_drbg_seed(&ssl->ctr_drbg, mbedtls_entropy_func, &ssl->entropy,
                                     (const unsigned char *) ssl->pers,
                                     strlen(ssl->pers))) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        goto exit;
    }
    mbedtls_printf(" ok\n");

   /*
    * 0. Initialize certificates
    */
   ssl->verify_tag = (verify_mode & 0xf);
   if(verify_mode > COMM_VERIFY_NO )
   {
        ssl->ca_cert = (mbedtls_x509_crt *)mem_malloc(sizeof(mbedtls_x509_crt));
        if(ssl->ca_cert == NULL)
        {
            ret = OS_ENOMEM;
            goto exit;
        }

        mbed_dbg( "  . Loading the CA root certificate ..." );
        mbedtls_x509_crt_init(ssl->ca_cert);
        ret = mbedtls_x509_crt_parse(ssl->ca_cert, ssl_socket_cfg->cacert->ca_buf, ssl_socket_cfg->cacert->ca_len);
        if( ret < 0 )
        {
            mbed_dbg( " failed\n  !  mbedtls_x509_crt_parse ca returned -0x%x\r\n", -ret );
            goto exit;
        }
        mbed_dbg( " ok (%d skipped)\n", ret );
   }

    if (verify_mode == COMM_VERIFY_DIDR)
    {
        int len = 0;
        unsigned char _pwd[] = "";
        unsigned char * p_pwd;
        if((ssl_socket_cfg->pkey->pwd) && (ssl_socket_cfg->pkey->pwd[0] != 0) )
        {
            len = strlen(ssl_socket_cfg->pkey->pwd);
            p_pwd = (unsigned char *)ssl_socket_cfg->pkey->pwd;
        }
        else
        {
            len = 0;
            p_pwd = _pwd;
        }

        mbed_dbg( "  . Loading the client certificate and key..." );
        ssl->cli_cert = (mbedtls_x509_crt *)mem_malloc(sizeof(mbedtls_x509_crt));
        if(ssl->cli_cert == NULL)
        {
            ret = OS_ENOMEM;
            goto exit;
        }

        ssl->pkey = (mbedtls_pk_context *)mem_malloc(sizeof(mbedtls_pk_context));
        if(ssl->pkey == NULL)
        {
            ret = OS_ENOMEM;
            goto exit;
        }

        mbedtls_x509_crt_init(ssl->cli_cert);
        ret = mbedtls_x509_crt_parse(ssl->cli_cert, ssl_socket_cfg->clicert->ca_buf, ssl_socket_cfg->clicert->ca_len);
        if( ret < 0 )
        {
            printf( " failed\n  !  mbedtls_x509_crt_parse cli returned -0x%x\r\n", -ret );
            ret = OS_EIO;

            goto exit;
        }

        mbedtls_pk_init(ssl->pkey);
        ret = mbedtls_pk_parse_key(ssl->pkey, (unsigned char const *) ssl_socket_cfg->pkey->ca_buf, \
                                    ssl_socket_cfg->pkey->ca_len, p_pwd, len);
        if(ret != 0)
        {
            printf(" failed\n  !  mbedtls_pk_parse_key returned -0x%x while parsing private key\r\n", -ret);
            //printf(" path : %s ", ssl_socket_cfg->pkey->ca_buf);
            ret = OS_EIO;
            goto exit;
        }
        mbed_dbg( " ok \n" );
    }

   if((ssl_socket_cfg->srv_cn) && (strlen(ssl_socket_cfg->srv_cn) > 0 ) )
   {
        ssl->verify_tag |= 0x01<<4;
        memset(ssl->srv_url, 0x00, sizeof(ssl->srv_url) );
        strncpy(ssl->srv_url, ssl_socket_cfg->srv_cn, sizeof(ssl->srv_url));
   }
    mbed_dbg( ". context %p Seeding the random number generator...", ssl );
    /*
     * 1. Start the connection
     */

    ssl->router = net_url->router;
    mbed_dbg( "  . Connecting to tcp/%s/%d...\n", net_url->addr, net_url->port );
    ssl_fd = tcpip_socket_open(SOCKET_PRT_TCP, (void*)net_url, NULL, 5000);
    if(ssl_fd < 0)
    {
        printf( " failed!! ssl socket connect returned %d\r\n", ssl_fd );
        ret = ssl_fd;
        goto exit;
    }

    ssl->server_fd.fd = ssl_fd;
    socket_context[ssl_id].resource = ssl;
    socket_context[ssl_id].steps = 2;
    bsp_delayms(500);

    mbed_dbg( " ok\n\r socket ssl_id=%d  fd=%d\n\r" , ssl_id, ssl_fd);

    /*
     * 2. Setup stuff
     */
    mbed_dbg( "  . Setting up the SSL/TLS structure..." );
    //fflush( stdout );

    if( ( ret = mbedtls_ssl_config_defaults( &ssl->ssl_config,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
        mbedtls_dbg(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        goto exit;
    }
    mbed_dbg( " ok\n\r" );

    /***********************************Add code - Begin*************************************************/
    mbedtls_x509_crt_profile profile;
    memcpy(&profile, ssl->ssl_config.cert_profile, sizeof(mbedtls_x509_crt_profile));
    profile.allowed_mds = profile.allowed_mds | MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA1 );
    mbedtls_ssl_conf_cert_profile(&ssl->ssl_config, &profile);
    /***********************************Add code - End*************************************************/

    /* OPTIONAL is not optimal for security,
     * but makes interop easier in this simplified example */
    if(!timeout) timeout = 5000;
    mbedtls_ssl_conf_read_timeout(&ssl->ssl_config, timeout);
    mbedtls_ssl_conf_rng( &ssl->ssl_config, tls_drbg_random, NULL);
    mbedtls_ssl_set_bio( &ssl->ssl_context, (void *)&ssl->server_fd, mbedtls_pal_net_send,
                    NULL, mbedtls_pal_net_recv_timeout );
    if((ssl_socket_cfg->srv_cn) && (strlen(ssl_socket_cfg->srv_cn) > 0 ) )
    {
        if( ( ret = mbedtls_ssl_set_hostname( &ssl->ssl_context, ssl_socket_cfg->srv_cn ) ) != 0 )
        {
            mbedtls_dbg( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
            goto exit;
        }
    }

    if(verify_mode > COMM_VERIFY_NO)
    {
        mbed_dbg( "  . Config the SSL/TLS ca cert....");
        mbedtls_ssl_conf_ca_chain( &ssl->ssl_config, ssl->ca_cert, NULL );
        mbedtls_ssl_conf_authmode(&(ssl->ssl_config), MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_verify(&(ssl->ssl_config), tls_socket_verify_cert, NULL);
        if(verify_mode == COMM_VERIFY_DIDR)
        {
            if((ret = mbedtls_ssl_conf_own_cert(&(ssl->ssl_config), ssl->cli_cert, ssl->pkey)) != 0)
            {
                printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\r\n", ret);
                goto exit;
            }
        }
    }
    else
    {
        mbedtls_ssl_conf_authmode(&ssl->ssl_config, MBEDTLS_SSL_VERIFY_NONE);
    }

    mbed_dbg( "  . Config the SSL/TLS ....");
    if( ( ret = mbedtls_ssl_setup( &ssl->ssl_context, &ssl->ssl_config ) ) != 0 )
    {
        mbedtls_dbg( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        goto exit;
    }
    mbed_dbg( " ok\n\r" );

    if ((ret = mbedtls_ssl_set_hostname(&ssl->ssl_context, ssl->srv_url)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
        goto exit;
    }
    /*
     * 4. Handshake
     */
    mbed_dbg( "  . Performing the SSL/TLS handshake...");
    while( ( ret = mbedtls_ssl_handshake( &ssl->ssl_context ) ) != 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            mbedtls_dbg( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
            goto exit;
        }
    }
    mbed_dbg( " ok\n\r" );

    /*
     * 5. Verify the server certificate
     */
    mbed_dbg( "  . Verifying peer X.509 certificate..." );

    if (verify_mode > COMM_VERIFY_NO)  // verif_flag = 1为不验证     ///
    {
        /* In real life, we probably want to bail out when ret != 0 */
        int flags;
        if( ( flags = mbedtls_ssl_get_verify_result( &ssl->ssl_context ) ) != 0 )
        {
#if !defined(MBEDTLS_X509_REMOVE_INFO)
        char vrfy_buf[512];
#endif
            mbedtls_dbg( " failed\n" );
#if !defined(MBEDTLS_X509_REMOVE_INFO)
            mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags );
            mbedtls_dbg( "%s\n", vrfy_buf );
#endif
            ret = flags;
            goto exit;
        }
        else
            mbed_dbg( "  ok\n");
    }
    else
    {
        mbed_dbg(" Server Verification skipped\n");
        ret = 0;
    }
    mbedtls_ssl_conf_read_timeout(&ssl->ssl_config, 3000);

    socket_context[ssl_id].steps = 3;

    printf( "open ok socket fd=%d ssl id %d \n\r" , ssl->server_fd.fd, ssl_id);
    return ssl->server_fd.fd;

exit:
    #ifdef MBEDTLS_ERROR_C
        if( exit_code != MBEDTLS_EXIT_SUCCESS )
        {
            char error_buf[100];
            mbedtls_strerror( ret, error_buf, 100 );
            mbed_dbg("Last error was: %d - %s\n\n", ret, error_buf );
        }
    #endif
    socket_context[ssl_id].steps = 1;
    printf(" ssl socket open id=%d addr %s:[%u] fail [%d]!!!\r\n", ssl_id, net_url->addr, net_url->port, ret);
    if(ssl_fd >= 0)
    {
        mbedtls_ssl_close_notify(&ssl->ssl_context);
        bsp_delayms(10);
        mbedtls_pal_net_free(ssl, 1);
    }

    if(ssl)
    {
        tls_free_cert_memery(ssl);
        mbedtls_ssl_free( &ssl->ssl_context );
        mbedtls_ssl_config_free( &ssl->ssl_config );
        mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
        mbedtls_entropy_free(&ssl->entropy);
        //mbedtls_psa_crypto_free();
        printf("ssl free ssl socket resource\r\n");
        mem_free(ssl);
        ssl = NULL;
    }
    socket_context[ssl_id].steps = 0;
    socket_context[ssl_id].resource = NULL;
    socket_context[ssl_id].stop = 0;

    return (ssl_reset_error(ret));
}

int ssl_socket_close(int fd, unsigned int force)
{
    int id = tls_get_socket_context(fd);

    printf("mbedtls free ssl socket close  %d %d \n", fd, id);
    if(id < 0) return 0;
    tls_free_socket_context(id);


    return 0;
}

int ssl_socket_get_status(int fd)
{
    int id = -1;
    int connect_status;
    if(fd < 0 ) return OS_EINVAL;

    id = tls_get_socket_context(fd);
    if(id < 0) { return  OS_SOCKET_ECONNECT; }

    connect_status = socket_context[id].steps;
    //printf("SSL Socket id=%d handler=%d  status =%d\n", id, Handler, connect_status);
    if (connect_status >= 3)
        return SOCKET_S_CONNECTED;
    else if (connect_status == 2)
        return SOCKET_S_CONNECTING;
    else //if (connect_status <= 1)
        return SOCKET_S_DISCONNECT;
}

int ssl_socket_send(int fd,unsigned char *data,unsigned int dataLenght)
{
    int ret = 0;
    SSLSocketRes *ssl = NULL;
    int id = -1;

    if(fd < 0 ||data==0) return OS_EINVAL;
    if(dataLenght==0)return 0;

    id = tls_get_socket_context(fd);
    ssl = socket_context[id].resource;
    if((id < 0) || (ssl == NULL) || (ssl->server_fd.fd < 0) ) { return OS_EIO; }

    if(socket_context[id].steps < 3) return 0;

    ret = mbedtls_ssl_write(&ssl->ssl_context, data, dataLenght );
    if(ret < 0) printf("SSL Send id %d socket %d send %d ?= %d\r\n", fd, id,dataLenght, ret);
    return ret;
}

int ssl_socket_recv(int fd,unsigned char *data,unsigned int dataLenght,unsigned int timeoutMs)
{
    int ret = -1, size = 0;
    SSLSocketRes *ssl = NULL;
    int id = -1;
    int t0 = bsp_get_tick()+timeoutMs;

    if(fd < 0 ||data==0) return OS_EINVAL;
    if(dataLenght==0)return 0;

    id = tls_get_socket_context(fd);
    if((id < 0)) {return OS_EIO; }
    ssl = socket_context[id].resource;
    if( (ssl == NULL) || (ssl->server_fd.fd < 0) ){return OS_EIO; }
    if(socket_context[id].steps < 3) return 0;

    mbedtls_ssl_conf_read_timeout(&ssl->ssl_config, timeoutMs/4);
    size = 0;
    do
    {
        if(socket_context[id].resource == NULL) { if(size) goto _exit; }

        ret = mbedtls_ssl_read( &ssl->ssl_context, data +size, dataLenght -size);
        if( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE )ret = 0;
        if(ret==0)
        {
            if((size) || (timeoutMs==0) )break;
            else bsp_delayms(10);
        }
        else if(ret<0) {
            // printf("%d-%d\r\n",ret,MBEDTLS_ERR_SSL_INVALID_MAC);
            if(size) break;
            return ret;
        }
        else
        {
            size +=ret;
            if(size>=dataLenght) break;
        }
    } while( bsp_get_tick() < t0);

    if(size > 0) printf("SSL Socket Recv fd %d id %d leng %d \n", fd, id, size);

_exit:
    return size;
}

static int tls_socket_verify_cert(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    char buf[1024];
    ((void) data);

    mbed_dbg("\nVerify requested for (Depth %d):%x \n", depth, *flags);
    if((*flags) == 0) return 0;
    mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);
    mbed_dbg("%s", buf);

    if((*flags) == 0) {
        mbed_dbg("  This certificate has no flags\n");
    } else {
        //mbed_dbg(buf, sizeof(buf), "  ! ", *flags);
        mbed_dbg("\nVerify requested for (Depth %d):\n", depth );
        mbedtls_x509_crt_info( buf, sizeof( buf ) - 1, "", crt );
        mbed_dbg("%s", buf );

        if( ( (*flags) & MBEDTLS_X509_BADCERT_EXPIRED ) != 0 )
            mbed_dbg("  ! server certificate has expired\n" );

        if( ( (*flags) & MBEDTLS_X509_BADCERT_REVOKED ) != 0 )
            mbed_dbg("  ! server certificate has been revoked\n" );

        if( ( (*flags) & MBEDTLS_X509_BADCERT_CN_MISMATCH ) != 0 )
            mbed_dbg("  ! CN mismatch\n" );

        if( ( (*flags) & MBEDTLS_X509_BADCERT_NOT_TRUSTED ) != 0 )
            mbed_dbg("  ! self-signed or not signed by a trusted CA\n" );

        if( ( (*flags) & MBEDTLS_X509_BADCRL_NOT_TRUSTED ) != 0 )
            mbed_dbg("  ! CRL not trusted\n" );

        if( ( (*flags) & MBEDTLS_X509_BADCRL_EXPIRED ) != 0 )
            mbed_dbg("  ! CRL expired\n" );

        if( ( (*flags) & MBEDTLS_X509_BADCERT_OTHER ) != 0 )
            mbed_dbg("  ! other (unknown) flag\n" );

        if ( ( *flags ) == 0 )
            mbed_dbg("  This certificate has no flags\n" );
    }

    return 0;
}

static void tls_socket_debug( void *ctx, int level, const char *file, int line,
                      const char *str )
{
    //((void) level);
    int debug_l = *(int*)ctx;

    if(debug_l >= level)
        printf("%s:%04d: %s", file, line, str );
    //fflush(  (FILE *) ctx  );
}

static int tls_get_socket_context(int fd)
{
    int i;
    for(i = 0; i< SSL_SOCKET_MAX_NUMBER; i++)
    {
        //mbed_dbg("%s ssl context  %d\r\n", __func__, ssl_context[i] );
        if((socket_context[i].steps > 0) && (socket_context[i].resource) &&
                (socket_context[i].resource->server_fd.fd == fd) )
        {
            //mbed_dbg("%s get tls socket index %d\r\n", __func__, i );
            return i;
        }
    }
    mbed_dbg("get tls socket index fd %d flag %d %d %d %d\r\n", fd,
            socket_context[0].steps, socket_context[1].steps,
            socket_context[2].steps, socket_context[3].steps);
    return -1;
}

static int tls_alloc_socket_context(void)
{
    int i;
    //unsigned int flag;

    for(i = 0; i< SSL_SOCKET_MAX_NUMBER; i++)
    {
        //mbedtls_dbg("%s ssl context  %d\r\n", __func__, ssl_context[i] );
        if( socket_context[i].steps == 0 )
        {
            socket_context[i].resource = NULL;
            socket_context[i].stop = 0;
            socket_context[i].steps = 1;
            return i;
        }
    }
    return -1;
}

static void tls_free_cert_memery(SSLSocketRes *ssl)
{
    if(ssl == NULL) return;
    if(ssl->ca_cert) {
        mbedtls_x509_crt_free(ssl->ca_cert );
        mem_free(ssl->ca_cert );
        ssl->ca_cert = NULL;
    }
    if(ssl->cli_cert) {
        mbedtls_x509_crt_free(ssl->cli_cert );
        mem_free(ssl->cli_cert );
        ssl->cli_cert = NULL;
    }
    if(ssl->pkey)
    {
        mbedtls_pk_free(ssl->pkey);
        mem_free(ssl->pkey);
        ssl->pkey = NULL;
    }
}

static int tls_free_socket_context(unsigned int ssl_id)
{
    SSLSocketRes *ssl = NULL;
    int ssl_fd=-1;
    //unsigned int flag;

    if((socket_context[ssl_id].steps ==0) || (socket_context[ssl_id].resource==0) )
    {
        goto exit;
    }

    ssl = socket_context[ssl_id].resource;
    ssl_fd =ssl->server_fd.fd;
    socket_context[ssl_id].steps = 1;

    printf("mbedtls free ssl socket res id %d ss_fd =%d \n", ssl_id, ssl_fd);
    if(ssl_fd >= 0)
    {
        mbedtls_ssl_close_notify(&ssl->ssl_context);
        bsp_delayms(100);
        mbedtls_pal_net_free(ssl, 1);
    }
    printf("mbedtls free ssl socket res id %d %p \n", ssl_id, ssl);
    if(ssl)
    {
        tls_free_cert_memery(ssl);
        mbedtls_ssl_free( &ssl->ssl_context );
        mbedtls_ssl_config_free( &ssl->ssl_config );

        mem_free(ssl);
        ssl = NULL;
    }
    socket_context[ssl_id].steps = 0;
    socket_context[ssl_id].resource = NULL;
    socket_context[ssl_id].stop = 0;

exit:
    return 0;
}

static int tls_drbg_random(void *ctx, unsigned char *out, unsigned int len)
{
    int ret;
    ((void )ctx);
    ret = bsp_gen_random(out, len);
    if(ret == 0) return 0;
    else return OS_EIO;
}

int mbedtls_pal_net_open( void *ctx, void* args)
{
    return 0;
}

void mbedtls_pal_net_free( void *ctx, int force )
{
    mbedtls_net_context *server_fd = (mbedtls_net_context *)ctx;

    mbed_dbg("mbedtls close socket %d  \n", server_fd->fd);
    if(server_fd->fd < 0 ) { return;}

    tcpip_socket_close(server_fd->fd, 1);
    server_fd->fd = -1;
}

/*
 * Write at most 'len' characters
 */
int mbedtls_pal_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    int ret= 0;
    mbedtls_net_context *server_fd = (mbedtls_net_context *)ctx;
    int fd = server_fd->fd;
    unsigned int size;
    unsigned int t0 = bsp_get_tick()+1000;

    //printf("tls pal send fd=%d recv len:%d\r\n", fd, len);
    if( fd < 0 ) { return( MBEDTLS_ERR_NET_INVALID_CONTEXT ); }

    //printHex("ssl pal send", buf, len);
    size=0;
    while(1)
    {
        ret = tcpip_socket_send(server_fd->fd, (unsigned char *)buf+size, len-size, 1);
        if(ret < 0)
        {
            if (size) break;
            else return ret;
        }
        else if (ret == 0) { if (size) break;}

        size+=ret;
        if(size>=len) break;
        if(bsp_get_tick()>t0) break;
        bsp_delayms(10);
    }
    if(len != size) printf("ssl socket pal %d send data end %d ?= %d\r\n", fd, len, size);
    return size;
}

int mbedtls_pal_net_recv( void *ctx, unsigned char *buf, size_t len )
{
    int ret = 0;
    mbedtls_net_context *server_fd = (mbedtls_net_context *)ctx;
    int fd = server_fd->fd;

    //printf("tls pal send fd=%d recv len:%d\r\n", fd, len);
    if( fd < 0 ) { return( MBEDTLS_ERR_NET_INVALID_CONTEXT ); }

    ret = tcpip_socket_recv(fd, (unsigned char *)buf, len, 2000);
    // if(ret) printf("len:%d,ret:%d\r\n",len,ret);
    //mbed_dbg("pal_socket %d recv no time end:%d leng %d \n", fd, ret, len);
    return ret;
}

int mbedtls_pal_net_recv_timeout( void *ctx, unsigned char *buf, size_t len, uint32_t timeout )
{
    int size, ret= 0;
    mbedtls_net_context *server_fd = (mbedtls_net_context *)ctx;
    int fd = server_fd->fd;
    unsigned int  temp = 0 ;
    unsigned int t0 = bsp_get_tick()+timeout;

    if( fd < 0 ) { return( MBEDTLS_ERR_NET_INVALID_CONTEXT ); }

    //printf("tls socket pal fd=%d timout=%d recv len:%d\r\n", fd, timeout, len);
    size = 0;
    do
    {
        ret = tcpip_socket_recv(fd, (unsigned char *)buf +size, len -size, 10);
        if(ret< 0)
        {
            if(size) break;
            else return ret;
        }
        else if(ret==0)
        {
            if(size) {temp++; if(temp > 4) break;}
            if(bsp_get_tick()>= t0) return OS_ETIMEOUT;
            if(timeout) bsp_delayms(10);
        }
        else
        {
            size += ret;
            if( size >= len) break;
        }
    } while(bsp_get_tick() < t0);

    if(len != size) printf("tls socket pal timeout recv len:%d, ret:%d\r\n",len, size);
    return size;
}
#endif

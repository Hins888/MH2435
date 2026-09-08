
#define DEBUG


#include "drivers.h"
#include "app.h"
#include "api.h"

#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/tcp.h"
#include "lwip/err.h"

#include "os_socket.h"

#define CANONNAME_MAX 128

typedef struct socket_fd_t
{
    int32_t  m_fd;
    uint16_t m_id;
    u8  m_type;
    // 0:idle 1:处理出错，释放SOCKET资源 2:正在DNS解析
    // 3:正在连接SOCKET // 4:SOCKET连接成功
    atomic m_status;
    u32 m_timeid;
} socket_fd_t;

socket_fd_t socket_fd[8] = {0};

extern int ssl_socket_open(void* args, void *config, unsigned int timeout);
extern int ssl_socket_close(int fd, unsigned int force);
extern int ssl_socket_recv(int fd,unsigned char *data,unsigned int dataLenght,unsigned int timeoutMs);
extern int ssl_socket_send(int fd,unsigned char *data,unsigned int dataLenght);

int s_sockInit(void)
{
    int i;

    for(i=0;i<SOCKET_NUMS;i++)
    {
        socket_fd[i].m_type = 0;
        socket_fd[i].m_id = 0;
        socket_fd[i].m_fd = -1;
        atomic_set(&socket_fd[i].m_status, 0);
    }

    return 0;
}

int s_sockReinit(void)
{
    int i;
    //unsigned int flag;

    for(i=0;i<SOCKET_NUMS;i++)
    {
        socket_fd[i].m_type = 0;
        socket_fd[i].m_id = 0;
        atomic_set(&socket_fd[i].m_status, 0);
        socket_fd[i].m_fd = -1;
    }
    return 0;
}

int s_sockAlloc(void)
{
    int i, id= OS_ENXIO;
    //unsigned int flag;

    for(i=0;i<SOCKET_NUMS;i++)
    {
        if(socket_fd[i].m_status.val ==0)
        {
            id = i;
            atomic_set(&socket_fd[i].m_status, 0);
            socket_fd[i].m_type = 0;
            socket_fd[i].m_id = 0;
            socket_fd[i].m_fd = -1;
            break;
        }
    }

    return id;
}

int s_sockFree(int sock_id)
{
    if(sock_id>= SOCKET_NUMS) return OS_EINVAL;

    if(socket_fd[sock_id].m_status.val == 0) return 0;

    atomic_set(&socket_fd[sock_id].m_status, 0);
    socket_fd[sock_id].m_fd = -1;

    return 0;
}

/***********************************************************
* 函数名称: errno_transform
* 描       述: errno的转换
* 输入参数: int32_t systemErrno 系统的errno
*           int *netwokErrno 错误类型
*           int32_t *privateErrno 转换后的错误编码
* 输出参数:
* 返 回  值: 自定义errno
* 说       明: 将linux系统下的errno转换为sdk自定义的errno
************************************************************/
static int32_t errno_transform(int32_t systemErrno, int *netwokErrno, int32_t *privateErrno)
{
//    int32_t num = sizeof(g_networkErrnoTrans);
//    int32_t i = 0;
//    for(i = 0;i<num;i++)
//    {
//        if(g_networkErrnoTrans[i].systemData == systemErrno)
//        {
//            *netwokErrno = g_networkErrnoTrans[i].netwokErrno;
//            *privateErrno = g_networkErrnoTrans[i].privateData;
//            return NETWORK_SUCCESS;
//        }
//    }
//
    return OS_SOCKET_EFAIL;
}

/***********************************************************
* 函数名称: OS_get_errno
* 描       述: 获取SDK定义的errno
* 输入参数:
* 输出参数:
* 返 回  值: 自定义errno
* 说       明: 将linux系统下的errno转换为sdk自定义的errno
************************************************************/
int32_t OS_get_errno(void)
{
    int networkErrno = OS_SOCKET_EFAIL;
    int32_t private = 0;
    int32_t result = errno_transform(errno,&networkErrno,&private);

    if(0 != result)
    {
        p_err("network errno = %d",errno);
        return OS_SOCKET_EFAIL;
    }

    return private;
}

/***********************************************************
* 函数名称: OS_network_send
* 描       述: 发送网络数据接口
* 输入参数: int32_t fd socket描述符
*           void *buf 数据缓存
*           int32_t nbytes 缓存区大小
*           IOT_NET_TRANS_FLAGS_E flags 操作标志
* 输出参数:
* 返 回  值: 同linux系统下标准socket返回值
* 说       明: 网络数据发送，linux系统下的实现
*           源码和mbedtls中使用
************************************************************/
int32_t tcpip_socket_send(int32_t fd, void *buf, uint32 nbytes, uint32_t flags)
{
    uint32_t flag = 0;

    if( (fd < 0 ))
    {
        return OS_SOCKET_EFAIL;
    }
    //p_dbg("socket send, id=%d fd = %d", fd &0xff, fd_t);

    return send(fd, buf, nbytes, flag);
}

/***********************************************************
* 函数名称: OS_network_recv
* 描       述: 接收网络数据接口
* 输入参数: int32_t fd socket描述符
*           void *buf 数据缓存
*           int32_t nbytes 缓存区大小
*           IOT_NET_TRANS_FLAGS_E flags 操作标志
* 输出参数:
* 返 回  值: 同linux系统下标准socket返回值
* 说       明: 网络数据发送，linux系统下的实现
*           源码和mbedtls中使用
************************************************************/
int32_t tcpip_socket_recv(int32_t fd, void* data, uint32 len, uint32 timeoutMs)
{
    uint32_t flag = 0;

    if( (fd < 0 ))
    {
        return OS_SOCKET_EFAIL;
    }

    return recv(fd, data, len, flag);
}

/***********************************************************
* 函数名称: OS_network_close
* 描       述: 网络socket关闭
* 输入参数: int32_t fd socket描述符
* 输出参数:
* 返 回  值: 0：成功  非零：失败
* 说       明: linux系统socket关闭
*           mbedtls中使用
************************************************************/
int32_t tcpip_socket_close(int32_t fd, uint32 force)
{
    if(fd < 0 )
    {
        return OS_SOCKET_EFAIL;
    }

    close(fd);
    return 0;

}

/***********************************************************
* 函数名称: OS_network_create
* 描       述: 客户端网络资源创建、连接
* 输入参数: int8_t*host 域名或主机IP
*           int8_t*service 端口或服务名
*           IOT_NET_PROTOCOL_TYPE type 协议类型
* 输出参数:
* 返 回  值:  0：成功，非零：失败
* 说       明: 使用系统接口创建socket，获取域名对应主机IP，
*           并且建立TCP连接
*           源码以及mbedtls中使用
************************************************************/
int32_t tcpip_socket_open(SOCKET_TYPE type, void* args, void * config, uint32 timeoutMs)
{
    p_dbg_enter;
    ST_NET_SOCKET * ap_info = (ST_NET_SOCKET* )args;
    struct addrinfo hints;
    struct addrinfo *addrInfoList = NULL;
    struct addrinfo *cur = NULL;
    char port[10];
    int fd = 0;
    int rc = OS_ERR;
    memset( &hints, 0, sizeof(hints));

    //默认支持IPv4的服务
    if(ap_info->addrType == 1) return 0;

    memset(&hints, 0, sizeof(struct addrinfo));
    if(SOCKET_PRT_SSL == type)
    {
        return ssl_socket_open(args, config, timeoutMs);
    }
    else if(SOCKET_PRT_TCP == type)
    {
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
    }
    else
    {
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
    }

    snprintf(port, 10, "%d", ap_info->port);
    p_dbg("socket_open, ip=%s:%d port_s=%s port_D = %d", ap_info->addr, ap_info->port, port, atoi(port));
    if ((rc = getaddrinfo( ap_info->addr, port, &hints, &addrInfoList ))!= 0 )
    {
        p_err("getaddrinfo error! rc = %d",rc);
        return OS_SOCKET_ECONNECT;
    }

    for( cur = addrInfoList; cur != NULL; cur = cur->ai_next )
    {
        //默认只支持IPv4
        if (cur->ai_family != AF_INET)
        {
            p_err("socket type error");
            rc = OS_SOCKET_ECONNECT;
            continue;
        }

        fd = (int) socket( cur->ai_family, cur->ai_socktype,cur->ai_protocol );
        if( fd < 0 )
        {
            p_err("create socket error,fd = %d, errno = %d",fd,errno);
            rc = OS_SOCKET_ECONNECT;
            continue;
        }

        if( connect( fd, cur->ai_addr,cur->ai_addrlen ) == 0 )
        {
            rc = fd;
            break;
        }

        close( fd );
        p_err("connect error,errno = %d",errno);
        rc = OS_SOCKET_ECONNECT;
    }

    freeaddrinfo(addrInfoList);

    return rc;
}


/***********************************************************
* 函数名称: OS_network_select
* 描       述: 有可读可写事件判断接口
* 输入参数: int32_t fd socket描述符
*           IOT_NET_TRANS_TYPE_E type 判断读写事件类型
*           int timeoutMs 超时时间
* 输出参数: IOT_NET_FD_ISSET_E* result 可读可写事件结果
* 返 回  值: 同linux系统下标准select的返回值
* 说       明: 判断socket是否有可读写事件，使用linux系统下select接口进行实现
*           源码和mbedtls中使用
************************************************************/
int32_t tcpip_socket_select(int32_t fd, SOCKET_TRANS_TYPE type, int timeoutMs, SOCKET_FD_ISSET_E * result)
{
    struct timeval *timePointer = NULL;
    fd_set *rd_set = NULL;
    fd_set *wr_set = NULL;
    fd_set *ep_set = NULL;
    int rc = 0;
    fd_set sets;

    struct timeval timeout ;//= {timeoutMs/1000, (timeoutMs%1000)*1000};
    timeout.tv_sec = timeoutMs/1000;
    timeout.tv_usec = (timeoutMs%1000)*1000;
    *result = SOCKET_FD_NO_ISSET;

    if( fd < 0 )
    {
        return OS_SOCKET_EFAIL;
    }

    FD_ZERO(&sets);
    FD_SET(fd, &sets);

    if(SOCKET_TRANS_RECV == type)
    {
        rd_set = &sets;
    }
    else
    {
        wr_set = &sets;
    }

    if(0 != timeoutMs)
    {
        timePointer = &timeout;
    }
    else
    {
        timePointer = NULL;
    }

    rc = select(fd+1,rd_set,wr_set,ep_set, timePointer);
    //rc = select(fd+1,rd_set,wr_set,ep_set, NULL);
    if(rc > 0)
    {
        if (0 != FD_ISSET(fd, &sets))
        {
            *result = SOCKET_FD_ISSET;
        }
    }

    return rc;
}


/***********************************************************
* 函数名称: OS_network_get_nonblock
* 描       述: 获取socket非阻塞状态
* 输入参数: int32_t fd socket描述符
* 输出参数:
* 返 回  值: 0：阻塞  非零：非阻塞
* 说       明: linux系统下通过fcntl接口获取当前socket是否是非阻塞接口
*           mbedtls会使用此接口
*           mbedtls中使用
************************************************************/
int32_t tcpip_socket_get_nonblock(int32_t fd)
{
    if( fd < 0 )
    {
        return OS_SOCKET_EFAIL;
    }

    if( ( fcntl( fd, F_GETFL, 0 ) & O_NONBLOCK ) != O_NONBLOCK )
    {
        return 0;
    }

    if(errno == EAGAIN || errno == EWOULDBLOCK)
    {
        return 1;
    }

    return 0 ;
}

/***********************************************************
* 函数名称: OS_network_set_nonblock
* 描       述: 设置socket非阻塞状态
* 输入参数: int32_t fd socket描述符
* 输出参数:
* 返 回  值: 0：成功  非零：失败
* 说       明: linux系统下通过fcntl接口设置当前socket为非阻塞接口
*           mbedtls中使用
************************************************************/
int32_t tcpip_socket_set_nonblock(int32_t fd)
{
    int32_t flags = 0;
    if( fd < 0 )
    {
        return OS_SOCKET_EFAIL;
    }

    flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, (flags | O_NONBLOCK)) < 0)
    {
        return OS_SOCKET_EFAIL;
    }

    return 0;
}

/***********************************************************
* 函数名称: OS_network_set_block
* 描       述: 设置socket阻塞状态
* 输入参数: int32_t fd socket描述符
* 输出参数:
* 返 回  值: 0：成功  非零：失败
* 说       明: linux系统下通过fcntl接口设置当前socket为阻塞接口
*           mbedtls中使用
************************************************************/
int32_t tcpip_socket_set_block(int32_t fd)
{
    int32_t flags = 0;
    if( fd < 0 )
    {
        return OS_SOCKET_EFAIL;
    }

    flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, (flags & (~O_NONBLOCK))) < 0)
    {
        return OS_SOCKET_EFAIL;
    }

    return 0;
}


/***********************************************************
* 函数名称: OS_network_bind
* 描       述: 服务端网络资源创建、连接
* 输入参数: int8_t*host 域名或主机IP
*           int8_t*service 端口或服务名
*           IOT_NET_PROTOCOL_TYPE type 协议类型
* 输出参数:
* 返 回  值:  0：成功，非零：失败
* 说       明: 使用系统接口创建socket，获取域名对应主机IP，
*           并且建立TCP连接
*           mbedtls中使用
************************************************************/
int32_t tcpip_socket_bind(const int8_t*host, const int8_t*service, int type)
{
    int fd = 0;
    int n = 0;
    int ret = OS_ERR;
    struct addrinfo hints, *addrList, *cur;

    /* Bind to IPv6 and/or IPv4, but only in the desired protocol */
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_INET;

    hints.ai_socktype = (type == 0 ? SOCK_DGRAM : SOCK_STREAM);
    hints.ai_protocol = (type == 0 ? IPPROTO_UDP : IPPROTO_TCP);
    if( host == NULL )
    {
        hints.ai_flags = AI_PASSIVE;
    }

    if( getaddrinfo((const char*)host, (const char*)service, &hints, &addrList ) != 0 )
    {
        return( OS_SOCKET_ECONNECT );
    }

    for( cur = addrList; cur != NULL; cur = cur->ai_next )
    {
        fd = (int) socket( cur->ai_family, cur->ai_socktype,cur->ai_protocol );
        if( fd < 0 )
        {
            ret = OS_SOCKET_ECONNECT;
            continue;
        }

        n = 1;
        if( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,(const char *) &n, sizeof( n ) ) != 0 )
        {
            close(fd);
            ret = OS_SOCKET_ECONNECT;
            continue;
        }

        if( bind(fd, cur->ai_addr, cur->ai_addrlen ) != 0 )
        {
            close( fd );
            ret = OS_SOCKET_ECONNECT;
            continue;
        }

        /* Listen only makes sense for TCP */
        if(type == IPPROTO_TCP)
        {
            if( listen( fd, 10 ) != 0 )
            {
                close( fd );
                ret = OS_SOCKET_ECONNECT;
                continue;
            }
        }

        /* I we ever get there, it's a success */
        ret = fd;
        break;
    }

    freeaddrinfo( addrList );

    return( ret );
}


s32 tcpip_socket_status(int fd, u32* status)
{
    int ret = 0;
    u32 sock_status =1;
    //socket_fd_t *fd_t = NULL;

    if((fd < 0)|| (status == 0)) return OS_EINVAL;

    *status = sock_status;

    return ret;
}

s32 tcpip_socket_clean(int fd, u32 flags)
{
    //socket_fd_t *fd_t = NULL;
    if( fd < 0 )
    {
        return OS_SOCKET_EFAIL;
    }

    //fd_t =&socket_fd[fd &0xff];

    return 0;
}


/***********************************************************
* 函数名称: OS_network_send
* 描       述: 发送网络数据接口
* 输入参数: int32_t fd socket描述符
*           void *buf 数据缓存
*           int32_t nbytes 缓存区大小
*           IOT_NET_TRANS_FLAGS_E flags 操作标志
* 输出参数:
* 返 回  值: 同linux系统下标准socket返回值
* 说       明: 网络数据发送，linux系统下的实现
*           源码和mbedtls中使用
************************************************************/
int32_t OS_socket_send(int32_t fd, void *buf, uint32 nbytes, uint32_t flags)
{
    uint32_t flag = 0;
    int32_t fd_t = -1;

    if( (fd < 0 ) || ((fd & 0xff) > SOCKET_NUMS) )
    {
        return OS_SOCKET_EFAIL;
    }
    fd_t = socket_fd[fd &0xff].m_fd;

    p_dbg("socket send, id=%d fd = %d", fd &0xff, fd_t);
    if(socket_fd[fd &0xff].m_type == 2)
    {
        return ssl_socket_send(fd_t, buf, nbytes);
    }
    else return tcpip_socket_send(fd_t, buf, nbytes, flag);
}

/***********************************************************
* 函数名称: OS_network_recv
* 描       述: 接收网络数据接口
* 输入参数: int32_t fd socket描述符
*           void *buf 数据缓存
*           int32_t nbytes 缓存区大小
*           IOT_NET_TRANS_FLAGS_E flags 操作标志
* 输出参数:
* 返 回  值: 同linux系统下标准socket返回值
* 说       明: 网络数据发送，linux系统下的实现
*           源码和mbedtls中使用
************************************************************/
int32_t OS_socket_recv(int32_t fd, void* data, uint32 len, uint32 timeoutMs)
{
    uint32_t flag = 0;
    int32_t fd_t = -1;

    if( (fd < 0 ) || ((fd & 0xff) > SOCKET_NUMS) )
    {
        return OS_SOCKET_EFAIL;
    }
    fd_t = socket_fd[fd &0xff].m_fd;

    p_dbg("socket recv, id=%d fd = %d", fd &0xff, fd_t);
    if(socket_fd[fd &0xff].m_type == 2)
    {
        return ssl_socket_recv(fd_t, data, len, timeoutMs);
    }
    else return tcpip_socket_recv(fd_t, data, len, flag);
}

/***********************************************************
* 函数名称: OS_network_close
* 描       述: 网络socket关闭
* 输入参数: int32_t fd socket描述符
* 输出参数:
* 返 回  值: 0：成功  非零：失败
* 说       明: linux系统socket关闭
*           mbedtls中使用
************************************************************/
int32_t OS_socket_close(int32_t fd, uint32 force)
{
    int ret;
    int8_t idx = -1;
    int32_t fd_t = -1;

    if( (fd < 0 ) || ( (fd & 0xff) > SOCKET_NUMS) )
    {
        return OS_SOCKET_EFAIL;
    }
    idx = fd & 0xff;
    fd_t = socket_fd[idx].m_fd;

    p_dbg("socket_close, id=%d sock_type =%d fd = %d", idx, socket_fd[idx].m_type, fd_t);
    if(socket_fd[idx].m_type == 2)
    {
        ret =  ssl_socket_close(fd_t, force);
    }
    else
    {
        ret = tcpip_socket_close(fd_t, force);
    }
    s_sockFree(idx);
    return ret;
}

/***********************************************************
* 函数名称: OS_network_create
* 描       述: 客户端网络资源创建、连接
* 输入参数: int8_t*host 域名或主机IP
*           int8_t*service 端口或服务名
*           IOT_NET_PROTOCOL_TYPE type 协议类型
* 输出参数:
* 返 回  值:  0：成功，非零：失败
* 说       明: 使用系统接口创建socket，获取域名对应主机IP，
*           并且建立TCP连接
*           源码以及mbedtls中使用
************************************************************/
int32_t OS_socket_open(SOCKET_TYPE type, void* args, void * config, uint32 timeoutMs)
{
    p_dbg_enter;
    int ret = OS_ERR;
    int8_t sock_id = -1;

    sock_id = s_sockAlloc();
    if(sock_id < 0) return OS_ENOMEM;

    p_dbg("connect type =%d, timeoutMs =%d", type, timeoutMs);
    if(SOCKET_PRT_SSL == type)
    {
        ret = ssl_socket_open(args, config, timeoutMs);
    }
    else
        ret = tcpip_socket_open(type, args, config, timeoutMs);

    if(ret < 0) goto _exit;


    socket_fd[sock_id].m_fd = ret;
    socket_fd[sock_id].m_type = type;
    socket_fd[sock_id].m_id = (type<<16) + sock_id;

    p_dbg("connect ok, type=%d fd =%d sock_fd=%d", type, socket_fd[sock_id].m_fd, socket_fd[sock_id].m_id);
    return socket_fd[sock_id].m_id;

_exit:
    p_dbg("socket type =%d ctx=%d open error =%d", type, sock_id, ret);
    s_sockFree(sock_id);
    return ret;
}


/***********************************************************
* 函数名称: OS_network_select
* 描       述: 有可读可写事件判断接口
* 输入参数: int32_t fd socket描述符
*           IOT_NET_TRANS_TYPE_E type 判断读写事件类型
*           int timeoutMs 超时时间
* 输出参数: IOT_NET_FD_ISSET_E* result 可读可写事件结果
* 返 回  值: 同linux系统下标准select的返回值
* 说       明: 判断socket是否有可读写事件，使用linux系统下select接口进行实现
*           源码和mbedtls中使用
************************************************************/
int32_t OS_socket_select(int32_t fd, SOCKET_TRANS_TYPE type, int timeoutMs, SOCKET_FD_ISSET_E * result)
{
    struct timeval *timePointer = NULL;
    fd_set *rd_set = NULL;
    fd_set *wr_set = NULL;
    fd_set *ep_set = NULL;
    int rc = 0;
    fd_set sets;

    struct timeval timeout ;//= {timeoutMs/1000, (timeoutMs%1000)*1000};
    timeout.tv_sec = timeoutMs/1000;
    timeout.tv_usec = (timeoutMs%1000)*1000;
    *result = SOCKET_FD_NO_ISSET;

    if( fd < 0 )
    {
        return OS_SOCKET_EFAIL;
    }

    FD_ZERO(&sets);
    FD_SET(fd, &sets);

    if(SOCKET_TRANS_RECV == type)
    {
        rd_set = &sets;
    }
    else
    {
        wr_set = &sets;
    }

    if(0 != timeoutMs)
    {
        timePointer = &timeout;
    }
    else
    {
        timePointer = NULL;
    }

    rc = select(fd+1,rd_set,wr_set,ep_set, timePointer);
    //rc = select(fd+1,rd_set,wr_set,ep_set, NULL);
    if(rc > 0)
    {
        if (0 != FD_ISSET(fd, &sets))
        {
            *result = SOCKET_FD_ISSET;
        }
    }

    return rc;
}


/***********************************************************
* 函数名称: OS_network_get_nonblock
* 描       述: 获取socket非阻塞状态
* 输入参数: int32_t fd socket描述符
* 输出参数:
* 返 回  值: 0：阻塞  非零：非阻塞
* 说       明: linux系统下通过fcntl接口获取当前socket是否是非阻塞接口
*           mbedtls会使用此接口
*           mbedtls中使用
************************************************************/
int32_t OS_socket_get_nonblock(int32_t fd)
{
    if( fd < 0 )
    {
        return OS_SOCKET_EFAIL;
    }

    if( ( fcntl( fd, F_GETFL, 0 ) & O_NONBLOCK ) != O_NONBLOCK )
    {
        return 0;
    }

    if(errno == EAGAIN || errno == EWOULDBLOCK)
    {
        return 1;
    }

    return 0 ;
}

/***********************************************************
* 函数名称: OS_network_set_nonblock
* 描       述: 设置socket非阻塞状态
* 输入参数: int32_t fd socket描述符
* 输出参数:
* 返 回  值: 0：成功  非零：失败
* 说       明: linux系统下通过fcntl接口设置当前socket为非阻塞接口
*           mbedtls中使用
************************************************************/
int32_t OS_socket_set_nonblock(int32_t fd)
{
    int32_t flags = 0;
    if( fd < 0 )
    {
        return OS_SOCKET_EFAIL;
    }

    flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, (flags | O_NONBLOCK)) < 0)
    {
        return OS_SOCKET_EFAIL;
    }

    return 0;
}

/***********************************************************
* 函数名称: OS_network_set_block
* 描       述: 设置socket阻塞状态
* 输入参数: int32_t fd socket描述符
* 输出参数:
* 返 回  值: 0：成功  非零：失败
* 说       明: linux系统下通过fcntl接口设置当前socket为阻塞接口
*           mbedtls中使用
************************************************************/
int32_t OS_socket_set_block(int32_t fd)
{
    int32_t flags = 0;
    if( fd < 0 )
    {
        return OS_SOCKET_EFAIL;
    }

    flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, (flags & (~O_NONBLOCK))) < 0)
    {
        return OS_SOCKET_EFAIL;
    }

    return 0;
}


/***********************************************************
* 函数名称: OS_network_bind
* 描       述: 服务端网络资源创建、连接
* 输入参数: int8_t*host 域名或主机IP
*           int8_t*service 端口或服务名
*           IOT_NET_PROTOCOL_TYPE type 协议类型
* 输出参数:
* 返 回  值:  0：成功，非零：失败
* 说       明: 使用系统接口创建socket，获取域名对应主机IP，
*           并且建立TCP连接
*           mbedtls中使用
************************************************************/
int32_t OS_socket_bind(const int8_t*host, const int8_t*service, int type)
{
    int fd = 0;
    int n = 0;
    int ret = OS_ERR;
    struct addrinfo hints, *addrList, *cur;

    /* Bind to IPv6 and/or IPv4, but only in the desired protocol */
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_INET;

    hints.ai_socktype = (type == 0 ? SOCK_DGRAM : SOCK_STREAM);
    hints.ai_protocol = (type == 0 ? IPPROTO_UDP : IPPROTO_TCP);
    if( host == NULL )
    {
        hints.ai_flags = AI_PASSIVE;
    }

    if( getaddrinfo((const char*)host, (const char*)service, &hints, &addrList ) != 0 )
    {
        return( OS_SOCKET_ECONNECT );
    }

    for( cur = addrList; cur != NULL; cur = cur->ai_next )
    {
        fd = (int) socket( cur->ai_family, cur->ai_socktype,cur->ai_protocol );
        if( fd < 0 )
        {
            ret = OS_SOCKET_ECONNECT;
            continue;
        }

        n = 1;
        if( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,(const char *) &n, sizeof( n ) ) != 0 )
        {
            close(fd);
            ret = OS_SOCKET_ECONNECT;
            continue;
        }

        if( bind(fd, cur->ai_addr, cur->ai_addrlen ) != 0 )
        {
            close( fd );
            ret = OS_SOCKET_ECONNECT;
            continue;
        }

        /* Listen only makes sense for TCP */
        if(type == IPPROTO_TCP)
        {
            if( listen( fd, 10 ) != 0 )
            {
                close( fd );
                ret = OS_SOCKET_ECONNECT;
                continue;
            }
        }

        /* I we ever get there, it's a success */
        ret = fd;
        break;
    }

    freeaddrinfo( addrList );

    return( ret );
}


s32 OS_socket_status(int fd, u32* status)
{
    int ret = 0;
    u32 sock_status =1;
    //socket_fd_t *fd_t = NULL;

    if((fd < 0)|| (status == 0)) return OS_EINVAL;

    *status = sock_status;

    return ret;
}

s32 OS_socket_clean(int fd, u32 flags)
{
    //socket_fd_t *fd_t = NULL;
    if( fd < 0 )
    {
        return OS_SOCKET_EFAIL;
    }

    //fd_t =&socket_fd[fd &0xff];

    return 0;
}


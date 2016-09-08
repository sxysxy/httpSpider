#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <arap/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#endif 
#include "httpSpider.h"
#include "linkqueue.h"
#include "trie.h"

#define logfile "spiderLog.txt"
const char *httpHeader = "GET %s HTTP/1.0 \r\n" \
             "User-Agent: Mozilla/5.0(compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0 \r\n\r\n";

#ifdef _WIN32
void initSocket()
{
    WSADATA ws;
    WSAStartup(MAKEWORD(2, 0), &ws);
}
void cleanSocket()
{
    WSACleanup();
}
#endif

FILE *flog;
void initSpider()
{
    flog = fopen(logfile, "w");
}
void termSpider()
{
    fflush(flog);
    fclose(flog);
}

#ifdef __linux__
typedef int SOCKET;
#endif
#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif
#ifdef _WIN32
#define close closesocket
#endif
long getIP(const char *host)
{
    long r;
    struct addrinfo *h;
    if(getaddrinfo(host, NULL, NULL, &h))
        return 0;
    else
    {
        //inet_ntop(AF_INET, &(((struct sockaddr_in *)(h->ai_addr))->sin_addr), sp -> host, 16);
        //InetNtop(AF_INET, &(((struct sockaddr_in *)(h->ai_addr))->sin_addr), sp -> host, 16); 
        r = ((struct sockaddr_in *)(h->ai_addr))->sin_addr.s_addr;
    }
    return r;
}

int request(spider *sp, char *host, char *path, int port, char *buffer, int maxb)
{
    SOCKET sokfd;
    struct sockaddr_in sokad;
    int sdlen, rlen;
	
    memset(&sokad, 0, sizeof(sokad));
    sokad.sin_family = AF_INET;     //ipv4
    sokad.sin_port = htons(port);
    if(! sp -> ip)
    {
        if((sokad.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
        {
            puts("ip无效，具体信息已输入日志");
            fprintf(flog, "-- ip无效，具体信息:\n  主机:%s 资源:%s 端口:%d\n\n", host, path, port);
            puts("已退出");
            return -1;
        }
    }else
    {
        sokad.sin_addr.s_addr = sp -> ip;
    }
	
    if((sokfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        puts("创建socket描述符失败，具体信息已输入日志");
        fprintf(flog, "-- 创建socket描述符失败，具体信息:\n  主机:%s 资源:%s 端口:%d\n\n", host, path, port);
        return -1;
    }
    
    if(connect(sokfd, (struct sockaddr *)&sokad, sizeof(sokad)) < 0)
    {
        puts("连接失败，具体信息已输入日志");
        fprintf(flog, "-- 连接失败，具体信息:\n  主机:%s 资源:%s 端口:%d\n\n", host, path, port);
    }
    char *rqbuf = (char *)malloc(sizeof(char) * 512);
    sdlen = sprintf(rqbuf, httpHeader, path);
	
    send(sokfd, rqbuf, sdlen, 0);
    free(rqbuf);
	
    rlen = recv(sokfd, buffer, maxb, 0);
    if(rlen < 0)
    {
        puts("接收数据失败，具体信息已输入日志");
        fprintf(flog, "-- 接收数据失败，具体信息:\n  主机:%s 资源:%s 端口:%d\n\n", host, path, port);
        return -1;
    }
    close(sokfd);
    return rlen;
}

void attachPlug(spiderPlug *p, spider *sp)
{
    #ifdef _WIN32
    HINSTANCE hlib = LoadLibrary(p -> plug);
    p -> nativePointer = (long)hlib;
    if(!hlib)
    {
        puts("加载外挂dll失败");
        fprintf(flog, "-- 加载外挂dll失败, 具体信息:\n dll路径:%s\n\n", p -> plug);
        //not return, just a warning
    }
    if(!(sp -> analyzer = (analyzerType)GetProcAddress(hlib, p -> func)))
    {
        printf("警告:找不到外挂dll中的分析函数%s\n", p -> func);
        fprintf(flog, "-- 警告:找不到外挂dll中的分析函数%s:\n dll路径:%s\n\n",p -> func, p -> plug);
    }
    p -> attached = true;
    #endif
}
void detachPlug(spiderPlug *p)
{
    #ifdef _WIN32
    FreeLibrary((HINSTANCE)p -> nativePointer);
    #endif
}

//核心的搜索部分
bool processUrl(spider *sp, ansiString *ret, char *str)  //str内 存着路径
{
    if(str[0] == '#')return false;  //肯定还是本页面...
    int len = strlen(str);
    int xlen;
    if(len > ANSISTRING_MAXLEN) len = ANSISTRING_MAXLEN;
    
    char buf[ANSISTRING_MAXLEN];
    char host[ANSISTRING_MAXLEN];
    char *ptr;
    buf[0] = 0;                     //标记清0
    //
    for(int i = 0; i < len; i++)
    {
        if(i+4 < len)  //不能越界
        {
            if(str[i] == 'h')
                if(str[i+1] == 't')
                    if(str[i+2] == 't')
                        if(str[i+3] == 'p')
                        {
                            i = i+4;  
                            if(i+2 < len)
                            {
                                if(str[i+1] == 's')
                                    if(str[i+2] == ':')
                                        return false;     //不滋瓷https口啊
                            }
                            if(str[i] == ':')
                            {
                                if(i + 2 < len)
                                {
                                    if(str[i+1] == '/')
                                        if(str[i+2] == '/')
                                        {
                                            i = i+3;     //
                                            while(i < len && str[i] == ' ')i++;
                                            int j = 0;
                                            while(i < len && str[i] != '/') //直到找到后面第一个分隔符
                                                host[j++] = str[i++];
                                            host[j] = 0;
                                            
                                            //后面就是相对路径
                                            j = 0;
                                            while(i < len && j < ANSISTRING_MAXLEN)
                                                buf[j++] = str[i++];
                                            buf[j] = 0;
                                            xlen = j;
                                            break;   //完成，走人(这可是在for里面)
                                        }
                                }
                            }
                        }
        }
    }
    if(buf[0])    //找到绝对路径中的相对路径
    {
        long tmp;
        if((tmp = getIP(host)) != 0 && tmp != sp -> ip)return false;  //不是本网站(ip不一样)
        if(tmp == 0)return false;
        ptr = buf;
    }else
        ptr = str;
    int nl = strlen(ptr);
    for(int i = nl-1; i > 1 && ptr[i] == ' '; i--)   //去掉多余的 / 
        ptr[i] = 0;
    //判重
    nl = strlen(ptr);
    initAnsiString2(ret, ptr, nl);
    
    //return !existWord(&sp -> slot, str, strlen(str));
    //initAnsiString2(ret, str, len);
    return true;
}

#define BUF_MAXSZ 100000
void bfs(spider *sp)
{
    char *data = (char *)malloc(sizeof(char) * BUF_MAXSZ);
    char pathb[ANSISTRING_MAXLEN];
    puts("开始搜索");
    fprintf(flog, "-- 开始搜索\n\n");
    
    linkQueue q;
    initQueue(&q);
    initTrie(&sp -> slot);
    
    ansiString root;
    initAnsiString(&root, "/");
    insertWord(&sp -> slot, "/", 1);
    pushQueue(&q, root);
    
    //还要判重，字典树先留个坑
    
    while(q.size > 0)
    {
        ansiString curl = popQueue(&q);
        int len = request(sp, sp -> host, curl.buffer, 80, data, BUF_MAXSZ);
        if(len < 0)continue;
        //分析网页源代码，扩展下一层节
        
        char *ps = data;
        for(int i = 0; i < len; i++)
        {
            int bk = 1;
            if(ps[i] == '<')
                bk++;
            else if(ps[i] == '>')
                bk--;
            if(bk)      //在html的标签里面
            {
                if(i+4 >= len)continue;
                if(ps[i] == 'h')                 //peek h
                    if(ps[i+1] == 'r')           //peek r
                        if(ps[i+2] == 'e')       //peek e
                            if(ps[i+3] == 'f')   //peek f
                            {
                                i = i+4;
                                while(i < len && ps[i] != '=')i++;
                                while(i < len && ps[i] != '\"')i++;  // href = "
                                i++;
                                while(i < len && ps[i] == ' ')i++;    //忽略多余空格
                                
                                //找到链接
                                int j = 0;
                                while(i < len && j < ANSISTRING_MAXLEN && ps[i] != '\"')
                                    pathb[j++] = ps[i++];
                                pathb[j] = 0;
                                
                                ansiString res;
                                if(processUrl(sp, &res, pathb)); //滤掉不合法的url，这个函数好像还是有点bug但是并不影响功能...(噗)
                                {
                                    if(existWord(&sp -> slot, res.buffer, res.length))continue;;
                                    fprintf(flog, "\n%s \n", res.buffer);
                                    insertWord(&sp -> slot, pathb, j);
                                    pushQueue(&q, res);
                                    
                                }
                            }
            }
        }
        printf("页面%s 搜索完毕， 深入下一层页面\n", curl.buffer);
        destroyAnsiString(&curl);
       
    }
     
    destroyQueue(&q);
    destroyTrie(&sp -> slot);
    free(data);
}

void useDomain(spider *sp)
{
    if(!(sp -> ip = getIP(sp -> host)))
    {
        printf("致命错误: 域名%s 无法识别其ip地址!\n", sp -> host);
        fprintf(flog, "致命错误: 域名%s 无法识别其ip地址!\n\n", sp -> host);
        puts("已退出");
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    #ifdef _WIN32
    initSocket();
    #endif
    initSpider(); 
	
    spider sp;
    spiderPlug pg;
    
    memset(&pg, 0, sizeof(spiderPlug));
    memset(&sp, 0, sizeof(spider));
    strcpy(pg.func, "analyzer");
    strcpy(pg.plug, "plugin.dll");
    attachPlug(&pg, &sp);
    
    // -----实验阶段
    if(argc > 1)
    {
        strcpy(sp.host, argv[1]);
        useDomain(&sp);
        sp.port = 80;
        bfs(&sp);
    }else
    {
        puts("请给出链接");
    }
    //-------
    
    detachPlug(&pg);
    termSpider();
    #ifdef _WIN32
    cleanSocket();
    #endif
    return 0;
}
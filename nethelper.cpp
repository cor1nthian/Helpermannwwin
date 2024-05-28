#define _WINSOCK_DEPRECATED_NO_WARNINGS

/*       In loving memory of @pixo $oft       */
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOKERNEL
#define NOMEMMGR
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NODRAWTEXT
#define NOWINSTYLES
#define NOSHOWWINDOW
#define NOWINMESSAGES
#define NOWH

#if defined(_WIN32) || defined(_WIN64) 
    // #undef _WINSOCKAPI_
    #define _WINSOCKAPI_
    #include <winsock2.h>	
    #include <ws2tcpip.h>
    #include <Windows.h>
    #include <iphlpapi.h>
    #include <icmpapi.h>
    #include <stdio.h>
#else
    #include <netdb.h>
    #include <arpa/inet.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
    #pragma comment(lib, "iphlpapi.lib")
    #pragma comment(lib, "ws2_32.lib")
#endif

#include "nethelper.h"

PingResult ping(char* address) {
    PingResult res;
    HANDLE hIcmpFile;
    unsigned long ipaddr = INADDR_NONE;
    unsigned long dwRetVal = 0;
    char SendData[32] = "Data Buffer";
    void* ReplyBuffer = 0;
    unsigned long ReplySize = 0;
    ipaddr = inet_addr(address);
    if (ipaddr != INADDR_NONE) {
        hIcmpFile = IcmpCreateFile();
        if (hIcmpFile != INVALID_HANDLE_VALUE) {
            ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
            ReplyBuffer = malloc(ReplySize);
            if (ReplyBuffer != 0) {
                dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
                if (dwRetVal != 0) {
                    PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
                    struct in_addr ReplyAddr;
                    ReplyAddr.S_un.S_addr = pEchoReply->Address;
                    res.Result = true;
                    res.tripTime = pEchoReply->RoundTripTime;
                } else {
                    // IcmpSendEcho failed
                }
            } else {
                // Unable to allocate memory
            }
        } else {
            // LPWSTR buf = (LPWSTR)calloc(48, 48 * sizeof(WCHAR));
            // wsprintf(buf, L"[Nethelper] Unable to open handle. IcmpCreatefile returned error: %06d", GetLastError());
            // free(buf);
        }
    } else {
        // Address incorrect
    }
    free(ReplyBuffer);
    ReplyBuffer = 0;
    return res;
}

char* lookupIPAddress(const char* dnsName) {
    char* ret = (char*)calloc(7, 7 * sizeof(char));
    if (ret) strcpy(ret, "0.0.0.0");
    struct hostent* he;
#if defined(_WIN32) || defined(_WIN64) 
    WSADATA data;
    if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
        //  WSAStartup failed
    }
#endif
    he = gethostbyname(dnsName);
    if (he == 0) {
        switch (h_errno) {
            case HOST_NOT_FOUND: {
                // The host was not found
            } break;
            case NO_ADDRESS: {
                // The name is valid but it has no address
            } break;
            case NO_RECOVERY: {
                //  A non-recoverable name server error occurred
            } break;
            case TRY_AGAIN: {
                // The name server is temporarily unavailable
            } break;
        }
    } else {
        free(ret);
        ret = inet_ntoa(*((struct in_addr*)he->h_addr_list[0]));
    }
#if defined(_WIN32) || defined(_WIN64) 
    WSACleanup();
#endif
    return ret;
}

char* getHostname(char* ip, unsigned short int port) {
    char* ret = (char*)calloc(7, 7 * sizeof(char));
    if (ret) strcpy(ret, "0.0.0.0");
#if defined(_WIN32) || defined(_WIN64) 
    WSADATA data;
    if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
        // Could not initialise Winsock
    }
#endif
    struct sockaddr_in saGNI;
    char hostname[NI_MAXHOST] = "none";
    char servInfo[NI_MAXSERV];
    saGNI.sin_family = AF_INET;
    saGNI.sin_addr.s_addr = inet_addr(ip);
    saGNI.sin_port = htons(port);
    if (getnameinfo((struct sockaddr*)&saGNI, sizeof(struct sockaddr), hostname,
        NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV)) {
        // getnameinfo failed
    }
#if defined(_WIN32) || defined(_WIN64) 
    WSACleanup();
#endif
    return hostname;
}
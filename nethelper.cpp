#include <iostream>
#include "nethelper.h"

#define DEF_PACKET_SIZE 32
#define MAX_PACKET 1024
#define ICMP_MIN 8 

#if defined(_WIN32) || defined(_WIN64)
    bool g_WSAStarted = false;
#endif`

NetOpResult ping(std::vector<PingResult> &results, const std::string address,
    const unsigned short int numAttempts, const unsigned short int timeout,
    const unsigned short int timeoutBetweenPings) {
    std::string pingtgt = address;
    unsigned char addrtestres = isStringIP(pingtgt);
    if (2 == addrtestres) {
        pingtgt = lookupIPAddress(pingtgt);
        if (!pingtgt.length()) {
            return NetOpResult::Fail;
        }
    } else if (3 == addrtestres) {
        return NetOpResult::Fail;
    }
    unsigned short pingattempts = numAttempts;
    if (!pingattempts) {
        pingattempts = 1;
    } else if (pingattempts > MAXPINGATTEMPTS) {
        pingattempts = MAXPINGATTEMPTS;
    }
    unsigned short pingtimeout = timeout;
    if (!pingtimeout) {
        pingtimeout = 1;
    } else if (pingtimeout > MAXPINGTIMEOUT) {
        pingtimeout = MAXPINGTIMEOUT;
    }
    unsigned short timeoutbwpings = timeoutBetweenPings;
    if (timeoutbwpings > MAXTIMEOUTBWPINGS) {
        timeoutbwpings = MAXTIMEOUTBWPINGS;
    }
#if defined(_WIN32) || defined(_WIN64) 
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return NetOpResult::Fail;
        }
        g_WSAStarted = true;
    }
#endif
    unsigned long ipaddr = INADDR_NONE;
    unsigned long retVal = 0;
    char SendData[32] = "Data Buffer";
    void* ReplyBuffer = 0;
    unsigned long ReplySize = 0;
    ipaddr = inet_addr(pingtgt.c_str());
    if (ipaddr != INADDR_NONE) {
        HANDLE hIcmpFile = IcmpCreateFile();
        if (hIcmpFile != INVALID_HANDLE_VALUE) {
            ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
            ReplyBuffer = malloc(ReplySize);
            if (ReplyBuffer != 0) {
                for (size_t i = 0; i < pingattempts; ++i) {
                    memset(ReplyBuffer, 0, ReplySize);
                    retVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer,
                        ReplySize, (unsigned long)pingtimeout);
                    if (retVal != 0) {
                        ICMP_ECHO_REPLY* pEchoReply = (ICMP_ECHO_REPLY*)ReplyBuffer;
                        results.emplace_back(true, pEchoReply->RoundTripTime, pEchoReply->Options.Ttl);
                    } else {
                        // IcmpSendEcho failed
                    }
                    if (timeoutbwpings) {
                        Sleep(timeoutbwpings);
                    }
                }
            } else {
                // Unable to allocate memory
            }
            IcmpCloseHandle(hIcmpFile);
        } else {
            // Unable to open handle. IcmpCreatefile returned error: %06d", GetLastError()
        }
    } else {
        // Address incorrect
    }
    SAFE_FREE(ReplyBuffer);
#if defined(_WIN32) || defined(_WIN64) 
    if (g_WSAStarted) {
        if (WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return NetOpResult::Success;
}

NetOpResult ping(std::vector<PingResult> &results, const std::wstring address,
    const unsigned short int numAttempts, const unsigned short int timeout,
    const unsigned short int timeoutBetweenPings) {
    std::wstring pingtgt = address;
    unsigned char addrtestres = isStringIP(pingtgt);
    if (2 == addrtestres) {
        pingtgt = lookupIPAddress(pingtgt);
        if (!pingtgt.length()) {
            return NetOpResult::Fail;
        }
    } else if (3 == addrtestres) {
        return NetOpResult::Fail;
    }
    unsigned short pingattempts = numAttempts;
    if (!pingattempts) {
        pingattempts = 1;
    } else if (pingattempts > MAXPINGATTEMPTS) {
        pingattempts = MAXPINGATTEMPTS;
    }
    unsigned short pingtimeout = timeout;
    if (!pingtimeout) {
        pingtimeout = 1;
    } else if (pingtimeout > MAXPINGTIMEOUT) {
        pingtimeout = MAXPINGTIMEOUT;
    }
    unsigned short timeoutbwpings = timeoutBetweenPings;
    if (timeoutbwpings > MAXTIMEOUTBWPINGS) {
        timeoutbwpings = MAXTIMEOUTBWPINGS;
    }
#if defined(_WIN32) || defined(_WIN64) 
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return NetOpResult::Fail;
        }
        g_WSAStarted = true;
    }
#endif
    unsigned long ipaddr = INADDR_NONE;
    unsigned long retVal = 0;
    char SendData[32] = "Data Buffer";
    void* ReplyBuffer = 0;
    unsigned long ReplySize = 0;
    ipaddr = inet_addr(wstr2str(pingtgt).c_str());
    if (ipaddr != INADDR_NONE) {
        HANDLE hIcmpFile = IcmpCreateFile();
        if (hIcmpFile != INVALID_HANDLE_VALUE) {
            ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
            ReplyBuffer = malloc(ReplySize);
            if (ReplyBuffer != 0) {
                for (size_t i = 0; i < pingattempts; ++i) {
                    memset(ReplyBuffer, 0, ReplySize);
                    retVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer,
                        ReplySize, (unsigned long)pingtimeout);
                    if (retVal != 0) {
                        ICMP_ECHO_REPLY* pEchoReply = (ICMP_ECHO_REPLY*)ReplyBuffer;
                        results.emplace_back(true, pEchoReply->RoundTripTime, pEchoReply->Options.Ttl);
                    } else {
                        // IcmpSendEcho failed
                    }
                    if (timeoutbwpings) {
                        Sleep(timeoutbwpings);
                    }
                }
            } else {
                // Unable to allocate memory
            }
            IcmpCloseHandle(hIcmpFile);
        } else {
            // Unable to open handle. IcmpCreatefile returned error: %06d", GetLastError()
        }
    } else {
        // Address incorrect
    }
    SAFE_FREE(ReplyBuffer);
#if defined(_WIN32) || defined(_WIN64) 
    if (g_WSAStarted) {
        if (WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return NetOpResult::Success;
}

/*  Send packets with increasing TTL until you get a reply from target host or max hops reached.
    Packets with TTL = 1 dont leave LAN */
NetOpResult traceroute(std::vector<TracertResult> &results, const std::string address,
    const unsigned char maxHops, const unsigned short int tracertTimeout,
    const bool doPings, const unsigned short int pingAttempts,
    const unsigned short int pingTimeout, const unsigned short int timeoutBetweenPings) {
    TracertResult trres;
    unsigned short int maxhops = maxHops;
    if (!maxhops) {
        maxhops = 1;
    } else if (maxhops > MAXTRACERTHOPS) {
        maxhops = MAXTRACERTHOPS;
    }
    unsigned short int tracerttimeout = tracertTimeout;
    if (tracerttimeout > MAXTRACERTTIMEOUT) {
        tracerttimeout = MAXTRACERTTIMEOUT;
    }
    unsigned short timeoutbwpings = timeoutBetweenPings;
    if (timeoutbwpings > MAXTIMEOUTBWPINGS) {
        timeoutbwpings = MAXTIMEOUTBWPINGS;
    }
    std::string tracerttgt = address;
    unsigned char addrtestres = isStringIP(tracerttgt);
    if (2 == addrtestres) {
        tracerttgt = lookupIPAddress(tracerttgt);
        if (!tracerttgt.length()) {
            return NetOpResult::Fail;
        }
    } else if (3 == addrtestres) {
        return NetOpResult::Fail;
    }
#if defined(_WIN32) || defined(_WIN64)
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return NetOpResult::Fail;
        }
        g_WSAStarted = true;
    }
#endif
    unsigned char hops = 1;
    unsigned long dwRetVal = 0;
    char SendData[32] = "Data Buffer\0";
    void* ReplyBuffer = 0;
    unsigned long ReplySize = 0;
    HANDLE hIcmpFile = IcmpCreateFile();
    if (hIcmpFile != INVALID_HANDLE_VALUE) {
        ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
        ReplyBuffer = malloc(ReplySize);
        if (ReplyBuffer != 0) {
            ip_option_information ip_option;
            memset(&ip_option, 0, sizeof(ip_option_information));
            hostent* host = gethostbyname(tracerttgt.c_str());
            if (!host) {
                return NetOpResult::Fail;
            }
            SOCKADDR_IN Addr;
            Addr.sin_addr.s_addr = *((u_long*)host->h_addr_list[0]);
            std::string taddr;
            do {
                ip_option.Ttl = hops;
                dwRetVal = IcmpSendEcho(hIcmpFile, Addr.sin_addr.s_addr, SendData, sizeof(SendData),
                    &ip_option, ReplyBuffer, ReplySize, (unsigned long)tracerttimeout);
                if (dwRetVal) {
                    memset(&trres, 0, sizeof(TracertResult));
                    ICMP_ECHO_REPLY* pEchoReply = (ICMP_ECHO_REPLY*)ReplyBuffer;
                    host = gethostbyaddr((char*)&pEchoReply->Address, 4, PF_INET);
                    if (host) {
                        addrtestres = isStringIP(host->h_name);
                        if (0 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(host->h_name);
                            trres.Address = str2wstr(getHostname(host->h_name));
                        } else if (1 == addrtestres) {
                        } else if (2 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(lookupIPAddress(host->h_name));
                            trres.Address = str2wstr(host->h_name);
                        } else {}
                        // trres.AddressIPV4 = str2wstr(host->h_name);
                        printf("ttl: %d\t%s\n", hops, host->h_name);
                    } else {
                        taddr = inet_ntoa(*(struct in_addr*)&pEchoReply->Address);
                        addrtestres = isStringIP(taddr);
                        if (0 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(taddr);
                            trres.Address = str2wstr(getHostname(taddr));
                        } else if (1 == addrtestres) {
                        } else if (2 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(lookupIPAddress(taddr));
                            trres.Address = str2wstr(taddr);
                        } else {}
                        // trres.Address = str2wstr(inet_ntoa(*(struct in_addr*)&pEchoReply->Address));
                        printf("ttl: %d\t%s\n", hops, inet_ntoa(*(struct in_addr*)&pEchoReply->Address));
                    }
                    trres.RoundTripTime = pEchoReply->RoundTripTime;
                    trres.TTL = pEchoReply->Options.Ttl;
                    results.push_back(trres);
                    if (lower_copy(lookupIPAddress(trres.Address)) == str2wstr(lower_copy(tracerttgt))) {
                        break;
                    }
                    ++hops;
                } else {
                    // IcmpSendEcho failed
                    if (WSAETIMEDOUT == WSAGetLastError() ||
                        WSA_QOS_ADMISSION_FAILURE == WSAGetLastError()) {
                        trres.Address = L"*";
                        trres.AddressIPV4 = L"";
                        trres.AddressIPV6 = L"";
                        trres.TTL = 0;
                        trres.RoundTripTime = 0;
                        results.push_back(trres);
                        printf("ttl: %d\t *\n", hops);
                        ++hops;
                        continue;
                    }
                }
            } while (hops <= maxhops);
        } else {
            // Unable to allocate memory
        }
        SAFE_FREE(ReplyBuffer);
        IcmpCloseHandle(hIcmpFile);
        if (doPings) {
            for (size_t i = 0; i < results.size(); ++i) {
                if (results[i].Address != L"*") {
                    if (results[i].AddressIPV4.length()) {
                        if (NetOpResult::Success != ping(results[i].Pings, results[i].AddressIPV4,
                            pingAttempts, pingTimeout, timeoutBetweenPings)) {
                            return NetOpResult::Fail;
                        }
                    } else {
                        if (NetOpResult::Success != ping(results[i].Pings, results[i].Address,
                            pingAttempts, pingTimeout, timeoutBetweenPings)) {
                            return NetOpResult::Fail;
                        }
                    }
                }
            }
        }
    } else {
        // Unable to open handle. IcmpCreatefile returned error: %06d", GetLastError()
    }
#if defined(_WIN32) || defined(_WIN64)
    if (g_WSAStarted) {
        if (WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    SAFE_FREE(ReplyBuffer);
    return NetOpResult::Success;
}

/*  Send packets with increasing TTL until you get a reply from target host or max hops reached.
    Packets with TTL = 1 dont leave LAN */
NetOpResult traceroute(std::vector<TracertResult> &results, const std::wstring address,
    const unsigned char maxHops, const unsigned short int tracertTimeout,
    const bool doPings, const unsigned short int pingAttempts, const unsigned short int timeout,
    const unsigned short int timeoutBetweenPings) {
    return traceroute(results, wstr2str(address), maxHops, tracertTimeout, doPings, pingAttempts, timeout,
        timeoutBetweenPings);
}

NetOpResult traceroute_RawSocket(std::vector<TracertResult> &results, const std::string address, const unsigned char maxHops) {
    TracertResult res;
    std::string tracerttgt = address;
    unsigned char addrtestres = isStringIP(tracerttgt);
    if (2 == addrtestres) {
        tracerttgt = lookupIPAddress(tracerttgt);
        if (!tracerttgt.length()) {
            return NetOpResult::Fail;
        }
    } else if (3 == addrtestres) {
        return NetOpResult::Fail;
    }
    SOCKADDR_IN dest = { 0 }, from = { 0 };
    HOSTENT* hp = 0;
    memset(&dest, 0, sizeof(SOCKADDR_IN));
    memset(&from, 0, sizeof(SOCKADDR_IN));
#if defined(_WIN32) || defined(_WIN64)
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return NetOpResult::Fail;
        }
        g_WSAStarted = true;
    }
#endif
    SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == sockRaw) {
        return NetOpResult::Fail;
    }
    int timeout = 1000;
    int ret = 0;
    ret = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    if (ret == SOCKET_ERROR) {
        return NetOpResult::Fail;
    }
    timeout = 1000;
    ret = setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    if (ret == SOCKET_ERROR) {
        return NetOpResult::Fail;
    }
    //
    // We need to resolve the host's ip address.  We check to see
    // if it is an actual Internet name versus an dotted decimal
    // IP address string.
    //
    dest.sin_family = AF_INET;
    if ((dest.sin_addr.s_addr = inet_addr(tracerttgt.c_str())) == INADDR_NONE) {
        hp = gethostbyname(tracerttgt.c_str());
        if (hp) {
            memcpy(&(dest.sin_addr), hp->h_addr, hp->h_length);
        } else {
            return NetOpResult::Fail;
        }
    }
    int datasize = DEF_PACKET_SIZE + sizeof(ICMPHeader);
    char* icmp_data = (char*)malloc(MAX_PACKET);
    if (!icmp_data) {
        return NetOpResult::Fail;
    }
    memset(icmp_data, 0, MAX_PACKET);
    char* recvbuf = (char*)malloc(MAX_PACKET);
    if (!recvbuf) {
        return NetOpResult::Fail;
    }
    memset(recvbuf, 0, MAX_PACKET);
    int bOpt = 1;
    if (SOCKET_ERROR == setsockopt(sockRaw, SOL_SOCKET, SO_DONTROUTE, (char*)&bOpt,
        sizeof(int))) {
        return NetOpResult::Fail;
    }
    ProcessHandler proc;
    ICMPHeader* icmp_hdr = (ICMPHeader*)icmp_data;
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->id = proc.GetCurrentProcPid();
    icmp_hdr->checksum = 0;
    icmp_hdr->seqnum = 0;
    char* datapart = icmp_data + sizeof(ICMPHeader);
    //
    // Place some junk in the buffer. Don't care about the data...
    //
    memset(datapart, 'E', datasize - sizeof(ICMPHeader));
    int bwrote = 0, fromlen = sizeof(from), ttl = 1;
    bool done = false;
    unsigned short seq_no = 0;
    for (ttl = 1; ((ttl < maxHops) && (!done)); ++ttl) {
        if (SOCKET_ERROR == setsockopt(sockRaw, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(int))) {
            return NetOpResult::Fail;
        }
        ((ICMPHeader*)icmp_data)->checksum = 0;
        ((ICMPHeader*)icmp_data)->timestamp = GetTickCount64();
        ((ICMPHeader*)icmp_data)->seqnum = seq_no++;
        ((ICMPHeader*)icmp_data)->checksum = ICMPHeaderChecksum((unsigned short*)icmp_data, datasize);
        if (SOCKET_ERROR == sendto(sockRaw, icmp_data, datasize, 0, (SOCKADDR*)&dest, sizeof(dest))) {
            if (WSAETIMEDOUT == WSAGetLastError()) {
                decodeResponse(recvbuf, ret, &from, ttl);
                continue;
            } else {
                return NetOpResult::Fail;
            }
        }
        if (SOCKET_ERROR == recvfrom(sockRaw, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&from, &fromlen)) {
            if (WSAETIMEDOUT == WSAGetLastError()) {
                decodeResponse(recvbuf, ret, &from, ttl);
                continue;
            } else {
                return NetOpResult::Fail;
            }
        }
        done = decodeResponse(recvbuf, ret, &from, ttl);
    }
    SAFE_FREE(icmp_data);
    SAFE_FREE(recvbuf);
#if defined(_WIN32) || defined(_WIN64)
    if (g_WSAStarted) {
        if (WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return NetOpResult::Success;
}

NetOpResult traceroute_RawSocket(std::vector<TracertResult> &results,
    const std::wstring address, const unsigned char maxHops) {
    return traceroute_RawSocket(results, wstr2str(address), maxHops);
}

std::string lookupIPAddress(const std::string dnsName) {
    char* ret = (char*)calloc(15, 15 * sizeof(char));
    if (ret) {
        strcpy(ret, "0.0.0.0");
    } else {
        return "";
    }
    struct hostent* he;
#if defined(_WIN32) || defined(_WIN64) 
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return "";
        }
        g_WSAStarted = true;
    }
#endif
    he = gethostbyname(dnsName.c_str());
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
    if (g_WSAStarted) {
        if (WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    if (ret) {
        return ret;
    } else {
        return "";
    }
}

std::wstring lookupIPAddress(const std::wstring dnsName) {
    char* ret = (char*)calloc(15, 15 * sizeof(char));
    if (ret) {
        strcpy(ret, "0.0.0.0");
    } else {
        return L"";
    }
    struct hostent* he;
#if defined(_WIN32) || defined(_WIN64) 
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return L"";
        }
        g_WSAStarted = true;
    }
#endif
    he = gethostbyname(wstr2str(dnsName).c_str());
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
    if (g_WSAStarted) {
        if (WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    if (ret) {
        return str2wstr(ret);
    } else {
        return L"";
    }
}

std::string getHostname(const std::string ip, unsigned short int port) {
    char* ret = (char*)calloc(15, 15 * sizeof(char));
    if (ret) {
        strcpy(ret, "0.0.0.0");
    } else {
        return "";
    }
    unsigned char addrtestres = isStringIP(ip);
    if (addrtestres > 1) {
        return "";
    }
#if defined(_WIN32) || defined(_WIN64) 
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return "";
        }
        g_WSAStarted = true;
    }
#endif
    struct sockaddr_in saGNI;
    char hostname[NI_MAXHOST] = "none";
    char servInfo[NI_MAXSERV];
    saGNI.sin_family = AF_INET;
    saGNI.sin_addr.s_addr = inet_addr(ip.c_str());
    saGNI.sin_port = htons(port);
    if (getnameinfo((struct sockaddr*)&saGNI, sizeof(struct sockaddr), hostname,
        NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV)) {
        // getnameinfo failed
    }
#if defined(_WIN32) || defined(_WIN64) 
    if (g_WSAStarted) {
        if (WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return hostname;
}

std::wstring getHostname(const std::wstring ip, unsigned short int port) {
    char* ret = (char*)calloc(15, 15 * sizeof(char));
    if (ret) {
        strcpy(ret, "0.0.0.0");
    } else {
        return L"";
    }
    unsigned char addrtestres = isStringIP(ip);
    if (addrtestres > 1) {
        return L"";
    }
#if defined(_WIN32) || defined(_WIN64) 
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return L"";
        }
        g_WSAStarted = true;
    }
#endif
    struct sockaddr_in saGNI;
    char hostname[NI_MAXHOST] = "none";
    char servInfo[NI_MAXSERV];
    saGNI.sin_family = AF_INET;
    saGNI.sin_addr.s_addr = inet_addr(wstr2str(ip).c_str());
    saGNI.sin_port = htons(port);
    if (getnameinfo((struct sockaddr*)&saGNI, sizeof(struct sockaddr), hostname,
        NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV)) {
        // getnameinfo failed
    }
#if defined(_WIN32) || defined(_WIN64) 
    if (g_WSAStarted) {
        if (WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return str2wstr(hostname);
}

unsigned short ICMPHeaderChecksum(unsigned short* buffer, int size) {
    unsigned long cksum = 0;
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(unsigned short);
    }
    if (size) {
        cksum += *(unsigned char*)buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (unsigned short)(~cksum);
}

int decodeResponse(char* buf, int bytes, SOCKADDR_IN* from, int ttl) {
    IPHeader* iphdr = 0;
    ICMPHeader* icmphdr = 0;
    unsigned short  iphdrlen;
    struct hostent* lpHostent = NULL;
    struct in_addr  inaddr = from->sin_addr;
    iphdr = (IPHeader*)buf;
    // Number of 32-bit words * 4 = bytes
    iphdrlen = iphdr->headerlen * 4;

    if (bytes < iphdrlen + ICMP_MIN) {
        printf("Too few bytes from %s\n", inet_ntoa(from->sin_addr));
    }
    icmphdr = (ICMPHeader*)(buf + iphdrlen);
    switch (icmphdr->type) {
        case ICMP_ECHOREPLY: { // Response from destination
            lpHostent = gethostbyaddr((const char*)&from->sin_addr, AF_INET, sizeof(struct in_addr));
            if (lpHostent != 0) {
                printf("%2d  %s (%s)\n", ttl, lpHostent->h_name, inet_ntoa(inaddr));
                return 1;
            }
        } break;
        case ICMP_TIMEOUT: { // Response from router along the way
            printf("%2d  %s\n", ttl, inet_ntoa(inaddr));
            return 0;
        } break;
        case ICMP_DESTUNREACH: { // Can't reach the destination at all
            printf("%2d  %s  reports: Host is unreachable\n", ttl, inet_ntoa(inaddr));
            return 1;
        } break;
        default: {
               printf("non-echo type %d recvd\n", icmphdr->type);
               return 1;
               break;
        } break;
    }
    return 0;
}
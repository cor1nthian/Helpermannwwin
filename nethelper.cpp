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
        pingtgt = lookupIPV4Address(pingtgt);
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
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return NetOpResult::Fail;
        }
        g_WSAStarted = true;
    }
#endif
    unsigned long ipaddr = INADDR_NONE;
    unsigned long retVal = 0;
    char SendData[32] = "Data Buffer\0";
    void* ReplyBuffer = 0;
    unsigned long ReplySize = 0;
    ipaddr = inet_addr(pingtgt.c_str());
    if (ipaddr != INADDR_NONE) {
        ::HANDLE hIcmpFile = ::IcmpCreateFile();
        if (hIcmpFile != INVALID_HANDLE_VALUE) {
            ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
            ReplyBuffer = malloc(ReplySize);
            if (ReplyBuffer != 0) {
                for (size_t i = 0; i < pingattempts; ++i) {
                    memset(ReplyBuffer, 0, ReplySize);
                    retVal = ::IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer,
                        ReplySize, (unsigned long)pingtimeout);
                    if (retVal != 0) {
                        ICMP_ECHO_REPLY* pEchoReply = (ICMP_ECHO_REPLY*)ReplyBuffer;
                        results.emplace_back(true, pEchoReply->RoundTripTime, pEchoReply->Options.Ttl);
                    } else {
                        // IcmpSendEcho failed
                        results.emplace_back(false, 0, 0);
                    }
                    if (timeoutbwpings) {
                        Sleep(timeoutbwpings);
                    }
                }
            } else {
                // Unable to allocate memory
            }
            ::IcmpCloseHandle(hIcmpFile);
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
        pingtgt = lookupIPV4Address(pingtgt);
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
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return NetOpResult::Fail;
        }
        g_WSAStarted = true;
    }
#endif
    unsigned long ipaddr = INADDR_NONE;
    unsigned long retVal = 0;
    char SendData[32] = "Data Buffer\0";
    void* ReplyBuffer = 0;
    unsigned long ReplySize = 0;
    ipaddr = inet_addr(wstr2str(pingtgt).c_str());
    if (ipaddr != INADDR_NONE) {
        ::HANDLE hIcmpFile = ::IcmpCreateFile();
        if (hIcmpFile != INVALID_HANDLE_VALUE) {
            ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
            ReplyBuffer = malloc(ReplySize);
            if (ReplyBuffer != 0) {
                for (size_t i = 0; i < pingattempts; ++i) {
                    memset(ReplyBuffer, 0, ReplySize);
                    retVal = ::IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer,
                        ReplySize, (unsigned long)pingtimeout);
                    if (retVal != 0) {
                        ICMP_ECHO_REPLY* pEchoReply = (ICMP_ECHO_REPLY*)ReplyBuffer;
                        results.emplace_back(true, pEchoReply->RoundTripTime, pEchoReply->Options.Ttl);
                    } else {
                        // IcmpSendEcho failed
                        results.emplace_back(false, 0, 0);
                    }
                    if (timeoutbwpings) {
                        Sleep(timeoutbwpings);
                    }
                }
            } else {
                // Unable to allocate memory
            }
            ::IcmpCloseHandle(hIcmpFile);
        } else {
            // Unable to open handle. IcmpCreatefile returned error: %06d", GetLastError()
        }
    } else {
        // Address incorrect
    }
    SAFE_FREE(ReplyBuffer);
#if defined(_WIN32) || defined(_WIN64) 
    if (g_WSAStarted) {
        if (::WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return NetOpResult::Success;
}

NetOpResult ping_MultipleEndPoints(PINGMULTIPLEEPSA &results, const std::string address,
    const std::string portOrSvcName, const unsigned short numAttempts,
    const unsigned short timeout, const unsigned short timeoutBetweenPings) {
    HostNode hn;
    if (NetOpResult::Success != lookupIPAddresses(hn, lower_copy(address), portOrSvcName)) {
        return NetOpResult::Fail;
    }
    std::vector<PingResult> tvec;
    for (size_t i = 0; i < hn.Address.size(); ++i) {
        if (NetOpResult::Success == ping(tvec, address, numAttempts, timeout, timeoutBetweenPings)) {
            results[wstr2str(hn.Address[i].Address)] = tvec;
            tvec.clear();
        } else {
            return NetOpResult::Fail;
        }
    }
    return NetOpResult::Success;
}

NetOpResult ping_MultipleEndPoints(PINGMULTIPLEEPSW &results, const std::wstring address,
    const std::wstring portOrSvcName, const unsigned short numAttempts,
    const unsigned short timeout, const unsigned short timeoutBetweenPings) {
    HostNode hn;
    if (NetOpResult::Success != lookupIPAddresses(hn, lower_copy(address), portOrSvcName)) {
        return NetOpResult::Fail;
    }
    std::vector<PingResult> tvec;
    for (size_t i = 0; i < hn.Address.size(); ++i) {
        if (NetOpResult::Success == ping(tvec, address, numAttempts, timeout, timeoutBetweenPings)) {
            results[hn.Address[i].Address] = tvec;
            tvec.clear();
        } else {
            return NetOpResult::Fail;
        }
    }
    return NetOpResult::Success;
}

/*  Send packets with increasing TTL until you get a reply from target host or max hops limit reached.
    Packets with TTL = 1 dont leave LAN */
NetOpResult traceroute(std::vector<TracertResult> &results, const std::string address, const unsigned char maxHops,
    const bool doPings, const unsigned short int tracertTimeout, const unsigned short int pingAttempts,
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
        tracerttgt = lookupIPV4Address(tracerttgt);
        if (!tracerttgt.length()) {
            return NetOpResult::Fail;
        }
    } else if (3 == addrtestres) {
        return NetOpResult::Fail;
    }
#if defined(_WIN32) || defined(_WIN64)
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return NetOpResult::Fail;
        }
        g_WSAStarted = true;
    }
#endif
    unsigned char hops = 1;
    unsigned long retVal = 0;
    char SendData[32] = "Data Buffer\0";
    void* ReplyBuffer = 0;
    unsigned long ReplySize = 0;
    ::HANDLE hIcmpFile = ::IcmpCreateFile();
    if (hIcmpFile != INVALID_HANDLE_VALUE) {
        ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
        ReplyBuffer = malloc(ReplySize);
        if (ReplyBuffer != 0) {
            ip_option_information ip_option;
            memset(&ip_option, 0, sizeof(ip_option_information));
            unsigned long ipaddr = inet_addr(tracerttgt.c_str());
            hostent* host = 0;
            std::string taddr;
            do {
                ip_option.Ttl = hops;
                retVal = ::IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
                    &ip_option, ReplyBuffer, ReplySize, (unsigned long)tracerttimeout);
                if (retVal) {
                    memset(&trres, 0, sizeof(TracertResult));
                    ICMP_ECHO_REPLY* pEchoReply = (ICMP_ECHO_REPLY*)ReplyBuffer;
                    host = gethostbyaddr((char*)&pEchoReply->Address, 4, PF_INET);
                    if (host) {
                        addrtestres = isStringIP(host->h_name);
                        if (0 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(trim_copy(host->h_name));
                            trres.Address = str2wstr(trim_copy(getHostnameByIPV4(host->h_name)));
                        } else if (1 == addrtestres) {
                        } else if (2 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(lookupIPV4Address(host->h_name));
                            trres.Address = str2wstr(host->h_name);
                        } else {}
                        // printf("ttl: %d\t%s\n", hops, host->h_name);
                    } else {
                        taddr = inet_ntoa(*(struct in_addr*)&pEchoReply->Address);
                        addrtestres = isStringIP(taddr);
                        if (0 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(trim_copy(taddr));
                            trres.Address = str2wstr(trim_copy(getHostnameByIPV4(taddr)));
                        } else if (1 == addrtestres) {
                        } else if (2 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(trim_copy(lookupIPV4Address(taddr)));
                            trres.Address = str2wstr(trim_copy(taddr));
                        } else {}
                        // printf("ttl: %d\t%s\n", hops, inet_ntoa(*(struct in_addr*)&pEchoReply->Address));
                    }
                    trres.RoundTripTime = pEchoReply->RoundTripTime;
                    trres.TTL = pEchoReply->Options.Ttl;
                    results.push_back(trres);
                    if (trres.AddressIPV4 == str2wstr(tracerttgt)) {
                        break;
                    }
                    ++hops;
                } else {
                    // ::IcmpSendEcho failed
                    if (WSAETIMEDOUT == WSAGetLastError() ||
                        WSA_QOS_ADMISSION_FAILURE == WSAGetLastError()) {
                        trres.Address = L"*";
                        trres.AddressIPV4 = L"";
                        trres.AddressIPV6 = L"";
                        trres.TTL = 0;
                        trres.RoundTripTime = 0;
                        results.push_back(trres);
                        // printf("ttl: %d\t *\n", hops);
                        ++hops;
                        continue;
                    }
                }
            } while (hops <= maxhops);
        } else {
            // Unable to allocate memory
        }
        SAFE_FREE(ReplyBuffer);
        ::IcmpCloseHandle(hIcmpFile);
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
        if (::WSACleanup()) {
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

/*  Send packets with increasing TTL until you get a reply from target host or max hops limit reached.
    Packets with TTL = 1 dont leave LAN */
NetOpResult traceroute(std::vector<TracertResult> &results, const std::wstring address, const unsigned char maxHops,
    const bool doPings, const unsigned short int tracertTimeout, const unsigned short int pingAttempts,
    const unsigned short int timeout, const unsigned short int timeoutBetweenPings) {
    return traceroute(results, wstr2str(address), maxHops, doPings, tracertTimeout, pingAttempts, timeout,
        timeoutBetweenPings);
}

/*  Send packets with increasing TTL until you get a reply from target host or max hops limit reached.
    Packets with TTL = 1 dont leave LAN */
NetOpResult traceroute_MultipleEndPoints(TRACERTMULTIPLEEPS &results, const std::string address,
    const unsigned char maxHops, const std::string portOrSvcName,
    const bool doPings, const unsigned short int tracertTimeout,
    const unsigned short int pingAttempts, const unsigned short int pingTimeout,
    const unsigned short int timeoutBetweenPings) {
    TracertResult trres;
    HostNode hn;
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
        tracerttgt = lookupIPV4Address(tracerttgt);
        if (!tracerttgt.length()) {
            return NetOpResult::Fail;
        }
    } else if (3 == addrtestres) {
        return NetOpResult::Fail;
    }
    if (NetOpResult::Success != lookupIPAddresses(hn, lower_copy(address), portOrSvcName)) {
        return NetOpResult::Fail;
    }
#if defined(_WIN32) || defined(_WIN64)
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return NetOpResult::Fail;
        }
        g_WSAStarted = true;
    }
#endif
    bool stopflag = false;
    unsigned char hops = 1;
    unsigned long retVal = 0;
    char SendData[32] = "Data Buffer\0";
    void* ReplyBuffer = 0;
    unsigned long ReplySize = 0;
    ::HANDLE hIcmpFile = ::IcmpCreateFile();
    if (hIcmpFile != INVALID_HANDLE_VALUE) {
        ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
        ReplyBuffer = malloc(ReplySize);
        if (ReplyBuffer != 0) {
            unsigned long ipaddr = inet_addr(tracerttgt.c_str());
            ip_option_information ip_option;
            memset(&ip_option, 0, sizeof(ip_option_information));
            hostent* host = { 0 };
            std::string taddr;
            do {
                ip_option.Ttl = hops;
                retVal = ::IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
                    &ip_option, ReplyBuffer, ReplySize, (unsigned long)tracerttimeout);
                if (retVal) {
                    memset(&trres, 0, sizeof(TracertResult));
                    ICMP_ECHO_REPLY* pEchoReply = (ICMP_ECHO_REPLY*)ReplyBuffer;
                    host = gethostbyaddr((char*)&pEchoReply->Address, 4, PF_INET);
                    if (host) {
                        addrtestres = isStringIP(host->h_name);
                        if (0 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(trim_copy(host->h_name));
                            trres.Address = str2wstr(trim_copy(getHostnameByIPV4(host->h_name)));
                        } else if (1 == addrtestres) {
                        } else if (2 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(trim_copy(lookupIPV4Address(host->h_name)));
                            trres.Address = str2wstr(trim_copy(host->h_name));
                        } else {}
                        // printf("ttl: %d\t%s\n", hops, host->h_name);
                    } else {
                        taddr = inet_ntoa(*(struct in_addr*)&pEchoReply->Address);
                        addrtestres = isStringIP(taddr);
                        if (0 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(trim_copy(taddr));
                            trres.Address = str2wstr(trim_copy(getHostnameByIPV4(taddr)));
                        } else if (1 == addrtestres) {
                        } else if (2 == addrtestres) {
                            trres.AddressIPV4 = str2wstr(trim_copy(lookupIPV4Address(taddr)));
                            trres.Address = str2wstr(trim_copy(taddr));
                        } else {}
                        //printf("ttl: %d\t%s\n", hops, inet_ntoa(*(struct in_addr*)&pEchoReply->Address));
                    }
                    trres.RoundTripTime = pEchoReply->RoundTripTime;
                    trres.TTL = pEchoReply->Options.Ttl;
                    results.push_back(trres);
                    for (size_t i = 0; i < hn.Address.size(); ++i) {
                        if (trres.AddressIPV4 == hn.Address[i].Address) {
                            stopflag = true;
                            break;
                        }
                    }
                    if (stopflag) {
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
                        // printf("ttl: %d\t *\n", hops);
                        ++hops;
                        continue;
                    }
                }
            } while (hops <= maxhops);
        } else {
            // Unable to allocate memory
        }
        SAFE_FREE(ReplyBuffer);
        ::IcmpCloseHandle(hIcmpFile);
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
        if (::WSACleanup()) {
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

/*  Send packets with increasing TTL until you get a reply from target host or max hops limit reached.
    Packets with TTL = 1 dont leave LAN */
NetOpResult traceroute_MultipleEndPoints(TRACERTMULTIPLEEPS &results, const std::wstring address,
    const std::wstring portOrSvcName, const unsigned char maxHops,
    const bool doPings, const unsigned short int tracertTimeout,
    const unsigned short int pingAttempts, const unsigned short int pingTimeout,
    const unsigned short int timeoutBetweenPings) {
    return(traceroute_MultipleEndPoints(results, wstr2str(address), maxHops, wstr2str(portOrSvcName), doPings, tracertTimeout,
        pingAttempts, pingTimeout, timeoutBetweenPings));
}

/*  Send packets with increasing TTL until you get a reply from target host or max hops limit reached.
    Packets with TTL = 1 dont leave LAN */
NetOpResult traceroute_MultipleStartPointsMultipleEndPoints(TRACERTMULTIPLESPSEPSA &results,
    const std::string address, const std::string portOrSvcName, const unsigned char maxHops, const bool doPings,
    const bool strictEndPointMatch, const unsigned short int tracertTimeout,
    const unsigned short int pingAttempts, const unsigned short int pingTimeout,
    const unsigned short int timeoutBetweenPings) {
    TracertResult trres;
    HostNode hn;
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
        tracerttgt = lookupIPV4Address(tracerttgt);
        if (!tracerttgt.length()) {
            return NetOpResult::Fail;
        }
    } else if (3 == addrtestres) {
        return NetOpResult::Fail;
    }
    if (NetOpResult::Success != lookupIPAddresses(hn, lower_copy(address), portOrSvcName)) {
        return NetOpResult::Fail;
    }
#if defined(_WIN32) || defined(_WIN64)
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return NetOpResult::Fail;
        }
        g_WSAStarted = true;
    }
#endif
    unsigned short int i = 0, j = 0;
    bool stopflag = false;
    unsigned char hops = 1;
    unsigned long retVal = 0, ipaddr = INADDR_NONE;
    char SendData[32] = "Data Buffer\0";
    void* ReplyBuffer = 0;
    unsigned long ReplySize = 0;
    ::HANDLE hIcmpFile = ::IcmpCreateFile();
    if (hIcmpFile != INVALID_HANDLE_VALUE) {
        ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
        ReplyBuffer = malloc(ReplySize);
        if (ReplyBuffer != 0) {
            ip_option_information ip_option;
            hostent* host;
            std::string taddr;
            std::vector<TracertResult> resvec;
            for (i = 0; i < hn.Address.size(); ++i) {
                hops = 1;
                resvec.clear();
                memset(&ip_option, 0, sizeof(ip_option_information));
                ipaddr = inet_addr(wstr2str(hn.Address[i].Address).c_str());
                do {
                    stopflag = false;
                    ip_option.Ttl = hops;
                    retVal = ::IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
                        &ip_option, ReplyBuffer, ReplySize, (unsigned long)tracerttimeout);
                    if (retVal) {
                        memset(&trres, 0, sizeof(TracertResult));
                        ICMP_ECHO_REPLY* pEchoReply = (ICMP_ECHO_REPLY*)ReplyBuffer;
                        host = gethostbyaddr((char*)&pEchoReply->Address, 4, PF_INET);
                        if (host) {
                            addrtestres = isStringIP(host->h_name);
                            if (0 == addrtestres) {
                                trres.AddressIPV4 = str2wstr(host->h_name);
                                trres.Address = str2wstr(trim_copy(getHostnameByIPV4(host->h_name)));
                            } else if (1 == addrtestres) {
                            } else if (2 == addrtestres) {
                                trres.AddressIPV4 = str2wstr(lookupIPV4Address(host->h_name));
                                trres.Address = str2wstr(host->h_name);
                            } else {}
                            // printf("ttl: %d\t%s\n", hops, host->h_name);
                        } else {
                            taddr = inet_ntoa(*(struct in_addr*)&pEchoReply->Address);
                            addrtestres = isStringIP(taddr);
                            if (0 == addrtestres) {
                                trres.AddressIPV4 = str2wstr(trim_copy(taddr));
                                trres.Address = str2wstr(trim_copy(getHostnameByIPV4(taddr)));
                            } else if (1 == addrtestres) {
                            } else if (2 == addrtestres) {
                                trres.AddressIPV4 = str2wstr(trim_copy(lookupIPV4Address(taddr)));
                                trres.Address = str2wstr(trim_copy(taddr));
                            } else {}
                            // printf("ttl: %d\t%s\n", hops, inet_ntoa(*(struct in_addr*)&pEchoReply->Address));
                        }
                        trres.RoundTripTime = pEchoReply->RoundTripTime;
                        trres.TTL = pEchoReply->Options.Ttl;
                        resvec.push_back(trres);
                        if (strictEndPointMatch) {
                            if (trres.AddressIPV4 == hn.Address[i].Address) {
                                stopflag = true;
                                break;
                            }
                        } else {
                            for (j = 0; j < hn.Address.size(); ++j) {
                                // std::wstring tstr = trim_copy(lookupIPAddress(trres.Address));
                                if (trres.AddressIPV4 == hn.Address[j].Address) {
                                    stopflag = true;
                                    break;
                                }
                            }
                        }
                        if (stopflag) {
                            break;
                        }
                        ++hops;
                    } else {
                        // ::IcmpSendEcho failed
                        if (WSAETIMEDOUT == WSAGetLastError() ||
                            WSA_QOS_ADMISSION_FAILURE == WSAGetLastError()) {
                            trres.Address = L"*";
                            trres.AddressIPV4 = L"";
                            trres.AddressIPV6 = L"";
                            trres.TTL = 0;
                            trres.RoundTripTime = 0;
                            resvec.push_back(trres);
                            //printf("ttl: %d\t *\n", hops);
                            ++hops;
                            continue;
                        }
                    }
                } while (hops <= maxhops);
                results[wstr2str(hn.Address[i].Address)] = resvec;
            }
        } else {
            // Unable to allocate memory
            return NetOpResult::Fail;
        }
        SAFE_FREE(ReplyBuffer);
        ::IcmpCloseHandle(hIcmpFile);
        if (doPings) {
            std::map<std::string, std::vector<TracertResult>>::iterator it;
            for (it = results.begin(); it != results.end(); ++it) {
                std::vector<TracertResult> temptracert = it->second;
                for (j = 0; j < temptracert.size(); ++j) {
                    if (temptracert[j].Address != L"*") {
                        if (temptracert[j].AddressIPV4.length()) {
                            if (NetOpResult::Success != ping(temptracert[j].Pings, temptracert[j].AddressIPV4,
                                pingAttempts, pingTimeout, timeoutBetweenPings)) {
                                return NetOpResult::Fail;
                            }
                        } else {
                            if (NetOpResult::Success != ping(temptracert[j].Pings, temptracert[j].Address,
                                pingAttempts, pingTimeout, timeoutBetweenPings)) {
                                return NetOpResult::Fail;
                            }
                        }
                        results[it->first] = temptracert;
                    }
                }
            }
        }
    } else {
        // Unable to open handle. IcmpCreatefile returned error: %06d", GetLastError()
    }
#if defined(_WIN32) || defined(_WIN64)
    if (g_WSAStarted) {
        if (::WSACleanup()) {
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

/*  Send packets with increasing TTL until you get a reply from target host or max hops limit reached.
    Packets with TTL = 1 dont leave LAN */
NetOpResult traceroute_MultipleStartPointsMultipleEndPoints(TRACERTMULTIPLESPSEPSW &results,
    const std::wstring address, const std::wstring portOrSvcName, const unsigned char maxHops,
    const bool doPings, const bool strictEndPointMatch, const unsigned short int tracertTimeout,
    const unsigned short int pingAttempts, const unsigned short int pingTimeout,
    const unsigned short int timeoutBetweenPings) {
    std::map <std::string, std::vector<TracertResult>> temptrmap;
    if (NetOpResult::Success == traceroute_MultipleStartPointsMultipleEndPoints(temptrmap, wstr2str(address),
        wstr2str(portOrSvcName), maxHops, doPings, strictEndPointMatch, tracertTimeout, pingAttempts, pingTimeout,
        timeoutBetweenPings)) {
        for (auto &it : temptrmap) {
            results[str2wstr(it.first)] = it.second;
        }
        return NetOpResult::Success;
    } else {
        return NetOpResult::Fail;
    }
}

/*  Send packets with increasing TTL until you get a reply from target host or max hops limit reached.
    Packets with TTL = 1 dont leave LAN */
NetOpResult traceroute_RawSocket(std::vector<TracertResult> &results, const std::string address, const unsigned char maxHops) {
    TracertResult res;
    std::string tracerttgt = address;
    unsigned char addrtestres = isStringIP(tracerttgt);
    if (2 == addrtestres) {
        tracerttgt = lookupIPV4Address(tracerttgt);
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
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
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
        if (::WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return NetOpResult::Success;
}

/*  Send packets with increasing TTL until you get a reply from target host or max hops limit reached.
    Packets with TTL = 1 dont leave LAN */
NetOpResult traceroute_RawSocket(std::vector<TracertResult> &results,
    const std::wstring address, const unsigned char maxHops) {
    return traceroute_RawSocket(results, wstr2str(address), maxHops);
}

NetOpResult lookupIPAddresses(HostNode &node, const std::string dnsName, const std::string portOrSvcName,
    const SocketType socketType, const NWProtocol protocol, const AddressFamily addressFamily) {
    std::string poslow = lower_copy(portOrSvcName);
#if defined(_WIN32) || defined(_WIN64)
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return NetOpResult::Fail;
        }
        g_WSAStarted = true;
    }
#endif
    int iresult = 0, retval = 0;
    unsigned long dwRetval = 0, ipbufferlength = 46;
    int i = 1;
    addrinfo* result = 0;
    addrinfo* ptr = 0;
    addrinfo hints = { 0 };
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = static_cast<int>(addressFamily);
    hints.ai_socktype = static_cast<int>(socketType);
    hints.ai_protocol = static_cast<int>(protocol);
    HostNodeAddr hna;
    if (!getaddrinfo(dnsName.c_str(), poslow.c_str(), &hints, &result)) {
        for (ptr = result; ptr != 0; ptr = ptr->ai_next) {
            switch (ptr->ai_family) {
                case AF_UNSPEC: {} break;
                case AF_INET: {
                    hna.Address = str2wstr(inet_ntoa(((struct sockaddr_in*)ptr->ai_addr)->sin_addr));
                    hna.AddrType = AddressType::IPv4;
                } break;
                case AF_INET6: {
                    char addrv6[INET6_ADDRSTRLEN] = { 0 };
                    inet_ntop(AF_INET6, &ptr->ai_addr, addrv6, INET6_ADDRSTRLEN);
                    hna.Address = str2wstr(addrv6);
                    hna.AddrType = AddressType::IPv6;
                } break;
                case AF_NETBIOS: {} break;
                default: {} break;
            }
            hna.SockType = (SocketType)ptr->ai_socktype;
            hna.Protocol = (NWProtocol)ptr->ai_protocol;
            node.Address.push_back(hna);
        }
        ::freeaddrinfo(result);
    } else {
        ::freeaddrinfo(result);
        return NetOpResult::Fail;
    }
#if defined(_WIN32) || defined(_WIN64)
    if (g_WSAStarted) {
        if (::WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return NetOpResult::Success;
}

NetOpResult lookupIPAddresses(HostNode &node, const std::wstring dnsName, const std::wstring portOrSvcName,
    const SocketType socketType, const NWProtocol protocol, const AddressFamily addressFamily) {
    return lookupIPAddresses(node, wstr2str(dnsName), wstr2str(portOrSvcName));
}

std::string lookupIPV4Address(const std::string dnsName) {
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
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
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
        if (::WSACleanup()) {
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

std::wstring lookupIPV4Address(const std::wstring dnsName) {
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
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
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
        if (::WSACleanup()) {
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

std::string getHostnameByIPV4(const std::string ip, unsigned short int port) {
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
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return "";
        }
        g_WSAStarted = true;
    }
#endif
    struct sockaddr_in saGNI;
    char hostname[NI_MAXHOST] = "none\0";
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
        if (::WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return hostname;
}

std::wstring getHostnameByIPV4(const std::wstring ip, unsigned short int port) {
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
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
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
        if (::WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return str2wstr(hostname);
}

std::string getHostnameByIPV6(const std::string ip, unsigned short int port) {
    /*unsigned char addrtestres = isStringIP(ip);
    if (1 != addrtestres) {
        return "";
    }*/
#if defined(_WIN32) || defined(_WIN64) 
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return "";
        }
        g_WSAStarted = true;
    }
#endif
    struct sockaddr_in6 saGNI;
    memset(&saGNI, 0, sizeof(saGNI));
    char hostname[NI_MAXHOST] = "none\0";
    char servInfo[NI_MAXSERV] = { 0 };
    saGNI.sin6_family = AF_INET6;
    inet_pton(AF_INET6, ip.c_str(), &saGNI.sin6_addr);
    saGNI.sin6_port = htons(port);
    if (getnameinfo((struct sockaddr*)&saGNI, sizeof(saGNI), hostname, NI_MAXHOST, servInfo, NI_MAXSERV,
        NI_NUMERICSERV)) {
        // getnameinfo failed
    }
#if defined(_WIN32) || defined(_WIN64) 
    if (g_WSAStarted) {
        if (::WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return hostname;
}

std::wstring getHostnameByIPV6(const std::wstring ip, unsigned short int port) {
    /*unsigned char addrtestres = isStringIP(ip);
    if (1 != addrtestres) {
        return "";
    }*/
#if defined(_WIN32) || defined(_WIN64) 
    if (!g_WSAStarted) {
        WSADATA wsd = { 0 };
        if (::WSAStartup(MAKEWORD(2, 2), &wsd)) {
            return L"";
        }
        g_WSAStarted = true;
    }
#endif
    struct sockaddr_in6 saGNI;
    memset(&saGNI, 0, sizeof(saGNI));
    char hostname[NI_MAXHOST] = "none\0";
    char servInfo[NI_MAXSERV] = { 0 };
    saGNI.sin6_family = AF_INET6;
    inet_pton(AF_INET6, wstr2str(ip).c_str(), &saGNI.sin6_addr);
    saGNI.sin6_port = htons(port);
    if (getnameinfo((struct sockaddr*)&saGNI, sizeof(saGNI), hostname, NI_MAXHOST, servInfo, NI_MAXSERV,
        NI_NUMERICSERV)) {
        // getnameinfo failed
    }
#if defined(_WIN32) || defined(_WIN64) 
    if (g_WSAStarted) {
        if (::WSACleanup()) {
            // handle WSACleanup error
            // int wsaerr = WSAGetLastError();
        } else {
            g_WSAStarted = false;
        }
    }
#endif
    return str2wstr(hostname);
}

NetOpResult getHostnameByIPV4_DNSQuery(std::string &hostName, const std::string ipAddr,
    const DNSQueryOpts queryOptions, const std::string dnsAddr) {
    unsigned char addrtestres = isStringIP(ipAddr);
    if (1 < addrtestres) {
        return NetOpResult::Fail;
    }
    std::string addrrev = reverseIPV4_copy(ipAddr);
    void* dnsptr = 0;
    if (dnsAddr.length()) {
        unsigned char dnssrvaddrtestres = isStringIP(dnsAddr);
        if (0 != dnssrvaddrtestres) {
            return NetOpResult::Fail;
        }
        dnsptr = ::LocalAlloc(LPTR, sizeof(IP4_ARRAY));
        if (!dnsptr) {
            return NetOpResult::Fail;
        }
        IP4_ARRAY* srvList = (IP4_ARRAY*)dnsptr;
        srvList->AddrCount = 1;
        srvList->AddrArray[0] = inet_addr(dnsAddr.c_str());
    }
    PDNS_RECORD dnsrec = { 0 };
    DNS_FREE_TYPE freetype = ::DnsFreeRecordListDeep;
    unsigned long dnsres = ::DnsQuery(str2wstr(addrrev + ".in-addr.arpa").c_str(),
        static_cast<unsigned short>(DNSRecordType::PtrRec), static_cast<unsigned long>(queryOptions),
        dnsptr, &dnsrec, 0);
    if (DNS_RCODE_NOERROR != dnsres) {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    if (dnsrec) {
        hostName = wstr2str(dnsrec->Data.PTR.pNameHost);
    } else {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    SAFE_LOCALFREE(dnsptr);
    ::DnsRecordListFree(dnsrec, freetype);
    return NetOpResult::Success;
}

NetOpResult getHostnameByIPV4_DNSQuery(std::wstring &hostName, const std::wstring ipAddr,
    const DNSQueryOpts queryOptions, const std::wstring dnsAddr) {
    unsigned char addrtestres = isStringIP(ipAddr);
    if (1 < addrtestres) {
        return NetOpResult::Fail;
    }
    std::wstring addrrev = reverseIPV4_copy(ipAddr);
    void* dnsptr = 0;
    if (dnsAddr.length()) {
        unsigned char dnssrvaddrtestres = isStringIP(dnsAddr);
        if (0 != dnssrvaddrtestres) {
            return NetOpResult::Fail;
        }
        dnsptr = ::LocalAlloc(LPTR, sizeof(IP4_ARRAY));
        if (!dnsptr) {
            return NetOpResult::Fail;
        }
        IP4_ARRAY* srvList = (IP4_ARRAY*)dnsptr;
        srvList->AddrCount = 1;
        srvList->AddrArray[0] = inet_addr(wstr2str(dnsAddr).c_str());
    }
    PDNS_RECORD dnsrec = { 0 };
    DNS_FREE_TYPE freetype = ::DnsFreeRecordListDeep;
    unsigned long dnsres = ::DnsQuery((addrrev + L".in-addr.arpa").c_str(), static_cast<unsigned short>(DNSRecordType::PtrRec),
        static_cast<unsigned long>(queryOptions), dnsptr, &dnsrec, 0);
    if (DNS_RCODE_NOERROR != dnsres) {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    if(dnsrec) {
        hostName = dnsrec->Data.PTR.pNameHost;
    } else {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    SAFE_LOCALFREE(dnsptr);
    ::DnsRecordListFree(dnsrec, freetype);
    return NetOpResult::Success;
}

NetOpResult getHostnameByIPV6_DNSQuery(std::string &hostName, const std::string ipAddr,
    const DNSQueryOpts queryOptions, const std::string dnsAddr) {
    unsigned char addrtestres = isStringIP(ipAddr);
    if (1 < addrtestres) {
        return NetOpResult::Fail;
    }
    std::string addrrev = reverseIPV6_copy(ipAddr);
    void* dnsptr = 0;
    if (dnsAddr.length()) {
        unsigned char dnssrvaddrtestres = isStringIP(dnsAddr);
        if (0 != dnssrvaddrtestres) {
            return NetOpResult::Fail;
        }
        dnsptr = ::LocalAlloc(LPTR, sizeof(IP4_ARRAY));
        if (!dnsptr) {
            return NetOpResult::Fail;
        }
        IP4_ARRAY* srvList = (IP4_ARRAY*)dnsptr;
        srvList->AddrCount = 1;
        srvList->AddrArray[0] = inet_addr(dnsAddr.c_str());
    }
    PDNS_RECORD dnsrec = { 0 };
    DNS_FREE_TYPE freetype = ::DnsFreeRecordListDeep;
    unsigned long dnsres = ::DnsQuery((str2wstr(addrrev) + L".ip6.arpa").c_str(),
        static_cast<unsigned short>(DNSRecordType::PtrRec), static_cast<unsigned long>(queryOptions),
        dnsptr, &dnsrec, 0);
    if (DNS_RCODE_NOERROR != dnsres) {
        dnsres = ::DnsQuery((str2wstr(addrrev) + L".ip6.int").c_str(),
            static_cast<unsigned short>(DNSRecordType::PtrRec), static_cast<unsigned long>(queryOptions),
            dnsptr, &dnsrec, 0);
        if (DNS_RCODE_NOERROR != dnsres) {
            SAFE_LOCALFREE(dnsptr);
            ::DnsRecordListFree(dnsrec, freetype);
            return NetOpResult::Fail;
        }
    }
    if (dnsrec) {
        hostName = wstr2str(dnsrec->Data.PTR.pNameHost);
    } else {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    SAFE_LOCALFREE(dnsptr);
    ::DnsRecordListFree(dnsrec, freetype);
    return NetOpResult::Success;
}

NetOpResult getHostnameByIPV6_DNSQuery(std::wstring &hostName, const std::wstring ipAddr,
    const DNSQueryOpts queryOptions, const std::wstring dnsAddr) {
    unsigned char addrtestres = isStringIP(ipAddr);
    if (1 < addrtestres) {
        return NetOpResult::Fail;
    }
    std::wstring addrrev = reverseIPV6_copy(ipAddr);
    void* dnsptr = 0;
    if (dnsAddr.length()) {
        unsigned char dnssrvaddrtestres = isStringIP(dnsAddr);
        if (0 != dnssrvaddrtestres) {
            return NetOpResult::Fail;
        }
        dnsptr = ::LocalAlloc(LPTR, sizeof(IP4_ARRAY));
        if (!dnsptr) {
            return NetOpResult::Fail;
        }
        IP4_ARRAY* srvList = (IP4_ARRAY*)dnsptr;
        srvList->AddrCount = 1;
        srvList->AddrArray[0] = inet_addr(wstr2str(dnsAddr).c_str());
    }
    PDNS_RECORD dnsrec = { 0 };
    DNS_FREE_TYPE freetype = ::DnsFreeRecordListDeep;
    unsigned long dnsres = ::DnsQuery((addrrev + L".ip6.arpa").c_str(),
        static_cast<unsigned short>(DNSRecordType::PtrRec), static_cast<unsigned long>(queryOptions),
        dnsptr, &dnsrec, 0);
    if (DNS_RCODE_NOERROR != dnsres) {
        dnsres = ::DnsQuery((addrrev + L".ip6.int").c_str(),
            static_cast<unsigned short>(DNSRecordType::PtrRec), static_cast<unsigned long>(queryOptions),
            dnsptr, &dnsrec, 0);
        if (DNS_RCODE_NOERROR != dnsres) {
            SAFE_LOCALFREE(dnsptr);
            ::DnsRecordListFree(dnsrec, freetype);
            return NetOpResult::Fail;
        }
    }
    if (dnsrec) {
        hostName = dnsrec->Data.PTR.pNameHost;
    } else {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    SAFE_LOCALFREE(dnsptr);
    ::DnsRecordListFree(dnsrec, freetype);
    return NetOpResult::Success;
}

NetOpResult getIPV4Addr_DNSQuery(std::string &ipAddr, const std::string hostName,
    const DNSQueryOpts queryOptions, const std::string dnsAddr) {
    unsigned char addrtestres = isStringIP(hostName);
    if (2 != addrtestres) {
        return NetOpResult::Fail;
    }
    void* dnsptr = 0;
    if (dnsAddr.length()) {
        unsigned char dnssrvaddrtestres = isStringIP(dnsAddr);
        if (0 != dnssrvaddrtestres) {
            return NetOpResult::Fail;
        }
        dnsptr = ::LocalAlloc(LPTR, sizeof(IP4_ARRAY));
        if (!dnsptr) {
            return NetOpResult::Fail;
        }
        IP4_ARRAY* srvList = (IP4_ARRAY*)dnsptr;
        srvList->AddrCount = 1;
        srvList->AddrArray[0] = inet_addr(dnsAddr.c_str());
    }
    PDNS_RECORD dnsrec = { 0 };
    DNS_FREE_TYPE freetype = ::DnsFreeRecordListDeep;
    unsigned long dnsres = ::DnsQuery(str2wstr(hostName).c_str(), static_cast<unsigned short>(DNSRecordType::ARec),
        static_cast<unsigned long>(queryOptions), dnsptr, &dnsrec, 0);
    if (DNS_RCODE_NOERROR != dnsres) {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    if (dnsrec) {
        char buf[32] = { 0 };
        inet_ntop(AF_INET, &dnsrec->Data.A.IpAddress, buf, INET_ADDRSTRLEN);
        ipAddr = buf;
    } else {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    SAFE_LOCALFREE(dnsptr);
    ::DnsRecordListFree(dnsrec, freetype);
    return NetOpResult::Success;
}

NetOpResult getIPV4Addr_DNSQuery(std::wstring &ipAddr, const std::wstring hostName,
    const DNSQueryOpts queryOptions, const std::wstring dnsAddr) {
    unsigned char addrtestres = isStringIP(hostName);
    if (2 != addrtestres) {
        return NetOpResult::Fail;
    }
    void* dnsptr = 0;
    if (dnsAddr.length()) {
        unsigned char dnssrvaddrtestres = isStringIP(dnsAddr);
        if (0 != dnssrvaddrtestres) {
            return NetOpResult::Fail;
        }
        dnsptr = ::LocalAlloc(LPTR, sizeof(IP4_ARRAY));
        if (!dnsptr) {
            return NetOpResult::Fail;
        }
        IP4_ARRAY* srvList = (IP4_ARRAY*)dnsptr;
        srvList->AddrCount = 1;
        srvList->AddrArray[0] = inet_addr(wstr2str(dnsAddr).c_str());
    }
    PDNS_RECORD dnsrec = { 0 };
    DNS_FREE_TYPE freetype = ::DnsFreeRecordListDeep;
    unsigned long dnsres = ::DnsQuery(hostName.c_str(), static_cast<unsigned short>(DNSRecordType::ARec),
        static_cast<unsigned long>(queryOptions), dnsptr, &dnsrec, 0);
    if (DNS_RCODE_NOERROR != dnsres) {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    if (dnsrec) {
        char buf[32] = { 0 };
        inet_ntop(AF_INET, &dnsrec->Data.A.IpAddress, buf, INET_ADDRSTRLEN);
        ipAddr = str2wstr(buf);
    } else {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    SAFE_LOCALFREE(dnsptr);
    ::DnsRecordListFree(dnsrec, freetype);
    return NetOpResult::Success;
}

NetOpResult getIPV6Addr_DNSQuery(std::string &ipAddr, const std::string hostName,
    const DNSQueryOpts queryOptions, const std::string dnsAddr) {
    unsigned char addrtestres = isStringIP(hostName);
    if (2 != addrtestres) {
        return NetOpResult::Fail;
    }
    void* dnsptr = 0;
    if (dnsAddr.length()) {
        unsigned char dnssrvaddrtestres = isStringIP(dnsAddr);
        if (0 != dnssrvaddrtestres) {
            return NetOpResult::Fail;
        }
        dnsptr = ::LocalAlloc(LPTR, sizeof(IP4_ARRAY));
        if (!dnsptr) {
            return NetOpResult::Fail;
        }
        IP4_ARRAY* srvList = (IP4_ARRAY*)dnsptr;
        srvList->AddrCount = 1;
        srvList->AddrArray[0] = inet_addr(dnsAddr.c_str());
    }
    PDNS_RECORD dnsrec = { 0 };
    DNS_FREE_TYPE freetype = ::DnsFreeRecordListDeep;
    unsigned long dnsres = ::DnsQuery(str2wstr(hostName).c_str(), static_cast<unsigned short>(DNSRecordType::AAAARec),
        static_cast<unsigned long>(queryOptions), dnsptr, &dnsrec, 0);
    if (DNS_RCODE_NOERROR != dnsres) {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    if (dnsrec) {
        char buf[INET6_ADDRSTRLEN] = { 0 };
        inet_ntop(AF_INET6, &dnsrec->Data.AAAA.Ip6Address, buf, INET6_ADDRSTRLEN);
        ipAddr = buf;
    } else {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    SAFE_LOCALFREE(dnsptr);
    ::DnsRecordListFree(dnsrec, freetype);
    return NetOpResult::Success;
}

NetOpResult getIPV6Addr_DNSQuery(std::wstring &ipAddr, const std::wstring hostName,
    const DNSQueryOpts queryOptions, const std::wstring dnsAddr) {
    unsigned char addrtestres = isStringIP(hostName);
    if (2 != addrtestres) {
        return NetOpResult::Fail;
    }
    void* dnsptr = 0;
    if (dnsAddr.length()) {
        unsigned char dnssrvaddrtestres = isStringIP(dnsAddr);
        if (0 != dnssrvaddrtestres) {
            return NetOpResult::Fail;
        }
        dnsptr = ::LocalAlloc(LPTR, sizeof(IP4_ARRAY));
        if (!dnsptr) {
            return NetOpResult::Fail;
        }
        IP4_ARRAY* srvList = (IP4_ARRAY*)dnsptr;
        srvList->AddrCount = 1;
        srvList->AddrArray[0] = inet_addr(wstr2str(dnsAddr).c_str());
    }
    PDNS_RECORD dnsrec = { 0 };
    DNS_FREE_TYPE freetype = ::DnsFreeRecordListDeep;
    unsigned long dnsres = ::DnsQuery(hostName.c_str(), static_cast<unsigned short>(DNSRecordType::AAAARec),
        static_cast<unsigned long>(queryOptions), dnsptr, &dnsrec, 0);
    if (DNS_RCODE_NOERROR != dnsres) {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    if (dnsrec) {
        char buf[INET6_ADDRSTRLEN] = { 0 };
        inet_ntop(AF_INET6, &dnsrec->Data.AAAA.Ip6Address, buf, INET6_ADDRSTRLEN);
        ipAddr = str2wstr(buf);
    } else {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    SAFE_LOCALFREE(dnsptr);
    ::DnsRecordListFree(dnsrec, freetype);
    return NetOpResult::Success;
}

NetOpResult customDNSQuery(PDNS_RECORD &queryResults, const std::string objectName,
    const DNSQueryOpts queryOptions, const DNSRecordType dnsRecordType, const std::wstring dnsAddr) {
    PDNS_RECORD dnsrec = { 0 };
    queryResults = dnsrec;
    unsigned char addrtestres = isStringIP(objectName);
    if (3 == addrtestres) {
        return NetOpResult::Fail;
    }
    void* dnsptr = 0;
    if (dnsAddr.length()) {
        unsigned char dnssrvaddrtestres = isStringIP(dnsAddr);
        if (0 != dnssrvaddrtestres) {
            return NetOpResult::Fail;
        }
        dnsptr = ::LocalAlloc(LPTR, sizeof(IP4_ARRAY));
        if (!dnsptr) {
            return NetOpResult::Fail;
        }
        IP4_ARRAY* srvList = (IP4_ARRAY*)dnsptr;
        srvList->AddrCount = 1;
        srvList->AddrArray[0] = inet_addr(wstr2str(dnsAddr).c_str());
    }
    DNS_FREE_TYPE freetype = ::DnsFreeRecordListDeep;
    unsigned long dnsres = ::DnsQuery(str2wstr(objectName).c_str(), static_cast<unsigned short>(dnsRecordType),
        static_cast<unsigned long>(queryOptions), dnsptr, &dnsrec, 0);
    if (DNS_RCODE_NOERROR != dnsres) {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    if (dnsrec) {
        queryResults = dnsrec;
    } else {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    SAFE_LOCALFREE(dnsptr);
    ::DnsRecordListFree(dnsrec, freetype);
    return NetOpResult::Success;
}

NetOpResult customDNSQuery(PDNS_RECORD &queryResults, const std::wstring objectName,
    const DNSQueryOpts queryOptions, const DNSRecordType dnsRecordType, const std::wstring dnsAddr) {
    PDNS_RECORD dnsrec = { 0 };
    queryResults = dnsrec;
    unsigned char addrtestres = isStringIP(objectName);
    if (3 == addrtestres) {
        return NetOpResult::Fail;
    }
    void* dnsptr = 0;
    if (dnsAddr.length()) {
        unsigned char dnssrvaddrtestres = isStringIP(dnsAddr);
        if (0 != dnssrvaddrtestres) {
            return NetOpResult::Fail;
        }
        dnsptr = ::LocalAlloc(LPTR, sizeof(IP4_ARRAY));
        if (!dnsptr) {
            return NetOpResult::Fail;
        }
        IP4_ARRAY* srvList = (IP4_ARRAY*)dnsptr;
        srvList->AddrCount = 1;
        srvList->AddrArray[0] = inet_addr(wstr2str(dnsAddr).c_str());
    }
    DNS_FREE_TYPE freetype = ::DnsFreeRecordListDeep;
    unsigned long dnsres = ::DnsQuery(objectName.c_str(), static_cast<unsigned short>(dnsRecordType),
        static_cast<unsigned long>(queryOptions), dnsptr, &dnsrec, 0);
    if (DNS_RCODE_NOERROR != dnsres) {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    if (dnsrec) {
        queryResults = dnsrec;
    } else {
        SAFE_LOCALFREE(dnsptr);
        ::DnsRecordListFree(dnsrec, freetype);
        return NetOpResult::Fail;
    }
    SAFE_LOCALFREE(dnsptr);
    ::DnsRecordListFree(dnsrec, freetype);
    return NetOpResult::Success;
}

std::wstring getDNSOpTextResult(const DNSResponseCode resultCode) {
    std::wstring ret;
    for (const auto &it : gc_DnsResultTest) {
        if (it.first == resultCode) {
            ret = it.second;
        }
    }
    if (!ret.length()) {
        ret = getErrorDetails(static_cast<unsigned long>(resultCode), L"DNSQuery");
    }
#ifdef DNSHELPER_SHOWERRORMSGBOX
    errorMsgBox(ret);
#endif
    return ret;
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
        // printf("Too few bytes from %s\n", inet_ntoa(from->sin_addr));
    }
    icmphdr = (ICMPHeader*)(buf + iphdrlen);
    switch (icmphdr->type) {
        case ICMP_ECHOREPLY: { // Response from destination
            lpHostent = gethostbyaddr((const char*)&from->sin_addr, AF_INET, sizeof(struct in_addr));
            if (lpHostent != 0) {
                // printf("%2d  %s (%s)\n", ttl, lpHostent->h_name, inet_ntoa(inaddr));
                return 1;
            }
        } break;
        case ICMP_TIMEOUT: { // Response from router along the way
            // printf("%2d  %s\n", ttl, inet_ntoa(inaddr));
            return 0;
        } break;
        case ICMP_DESTUNREACH: { // Can't reach the destination at all
            printf("%2d  %s  reports: Host is unreachable\n", ttl, inet_ntoa(inaddr));
            return 1;
        } break;
        default: {
               // printf("non-echo type %d recvd\n", icmphdr->type);
               return 1;
               break;
        } break;
    }
    return 0;
}
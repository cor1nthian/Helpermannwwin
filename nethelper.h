#ifndef _NETHELPER_H
#define _NETHELPER_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS

/*       In loving memory of @pixo $oft       */
#define WIN32_LEAN_AND_MEAN
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
	#define _WINSOCKAPI_
	#include <winsock2.h>	
	#include <ws2tcpip.h>
	#include <iphlpapi.h>
	#include <icmpapi.h>
	#include <Windows.h>
	#include <stdio.h>
#else
	#include <netdb.h>
	#include <arpa/inet.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
	#pragma comment(lib, "iphlpapi.lib")
	#pragma comment(lib, "ws2_32.lib")
#endif

#include "strhelper.h"
#include "prochelper.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

#define ICMP_ECHOREPLY		0
#define ICMP_DESTUNREACH	3
#define ICMP_SRCQUENCH		4
#define ICMP_REDIRECT		5
#define ICMP_ECHO			8
#define ICMP_TIMEOUT		11
#define ICMP_PARMERR		12

#define MAXPINGATTEMPTS		1024
#define MAXPINGTIMEOUT		3000
#define MAXTIMEOUTBWPINGS	5000
#define MAXTRACERTTIMEOUT	3000
#define MAXTRACERTHOPS		255
#define MAXTRACERTPINGS		32

#if defined(_WIN32) || defined(_WIN64)
	extern bool g_WSAStarted;
#endif

enum class NetOpResult : unsigned char {
	Success,
	Fail
};

struct PingResult {
	PingResult() { Result = false; RoundTripTime = 0; TTL = 0;  }
	PingResult(const bool result, const unsigned short int rtt, const unsigned short int ttl) {
		Result = result;
		RoundTripTime = rtt;
		TTL = ttl;
	}
	PingResult(const PingResult &other) {
		Result = other.Result;
		RoundTripTime = other.RoundTripTime;
		TTL = other.TTL;
	}
	PingResult& operator=(const PingResult &other) {
		Result = other.Result;
		RoundTripTime = other.RoundTripTime;
		TTL = other.TTL;
		return *this;
	}
	bool operator==(const PingResult &other) const {
		return(Result == other.Result &&
			RoundTripTime == other.RoundTripTime &&
			TTL == other.TTL);
	}
	bool operator!=(const PingResult& other) const {
		return(Result != other.Result ||
			RoundTripTime != other.RoundTripTime ||
			TTL != other.TTL);
	}
	~PingResult() {}
	bool Result;
	unsigned short int TTL;
	unsigned short int RoundTripTime;
};

struct TracertResult {
	TracertResult() { TTL = 0; RoundTripTime = 0; }
	TracertResult(const std::wstring address, const std::wstring addressv4,
		const std::wstring addressv6, const unsigned short int ttl,
		const unsigned short int rtt, const std::vector<PingResult> pings) {
		TTL = ttl;
		RoundTripTime = rtt;
		Address = address;
		AddressIPV4 = addressv4;
		AddressIPV6 = addressv6;
		Pings = pings;
	}
	TracertResult(const TracertResult &other) {
		TTL = other.TTL;
		RoundTripTime = other.RoundTripTime;
		Address = other.Address;
		AddressIPV4 = other.AddressIPV4;
		AddressIPV6 = other.AddressIPV6;
		Pings = other.Pings;
	}
	TracertResult& operator=(const TracertResult &other) {
		TTL = other.TTL;
		RoundTripTime = other.RoundTripTime;
		Address = other.Address;
		AddressIPV4 = other.AddressIPV4;
		AddressIPV6 = other.AddressIPV6;
		Pings = other.Pings;
		return *this;
	}
	bool operator==(const TracertResult &other) const {
		return(Pings == other.Pings && TTL == other.TTL &&
			RoundTripTime == other.RoundTripTime &&
			lower_copy(Address) == lower_copy(other.Address) &&
			lower_copy(AddressIPV4) == lower_copy(other.AddressIPV4) &&
			lower_copy(AddressIPV6) == lower_copy(other.AddressIPV6));
	}
	bool operator!=(const TracertResult &other) const {
		return(Pings != other.Pings || TTL != other.TTL ||
			RoundTripTime != other.RoundTripTime ||
			lower_copy(Address) != lower_copy(other.Address) ||
			lower_copy(AddressIPV4) != lower_copy(other.AddressIPV4) ||
			lower_copy(AddressIPV6) != lower_copy(other.AddressIPV6));
	}
	~TracertResult() {}
	unsigned short int TTL;
	unsigned short int RoundTripTime;
	std::wstring Address;
	std::wstring AddressIPV4;
	std::wstring AddressIPV6;
	std::vector<PingResult> Pings;
};

struct ICMPHeader {
	ICMPHeader() {
		type = 0;
		code = 0;
		checksum = 0;
		id = 0;
		seqnum = 0;
		timestamp = 0;
	}
	ICMPHeader(const ICMPHeader &other) {
		type = other.type;
		code = other.code;
		checksum = other.checksum;
		id = other.id;
		seqnum = other.seqnum;
		timestamp = other.timestamp;
	}
	~ICMPHeader() {}
	ICMPHeader& operator=(const ICMPHeader &other) {
		type = other.type;
		code = other.code;
		checksum = other.checksum;
		id = other.id;
		seqnum = other.seqnum;
		timestamp = other.timestamp;
		return *this;
	}
	bool operator==(const ICMPHeader &other) const {
		return (type == other.type &&
			code == other.code &&
			checksum == other.checksum &&
			id == other.id &&
			seqnum == other.seqnum &&
			timestamp == other.timestamp);
	}
	bool operator!=(const ICMPHeader &other) const {
		return (type != other.type ||
			code != other.code ||
			checksum != other.checksum ||
			id != other.id ||
			seqnum != other.seqnum ||
			timestamp != other.timestamp);
	}
	unsigned char type;				// ICMP message type
	unsigned char code;				// Sub code
	unsigned short checksum;		// Checksum
	unsigned short seqnum;			// Sequence number
	unsigned long id;				// Unique id
	// This is not the std header, but we reserve space for time
	unsigned long timestamp;
};

struct IPHeader {
	IPHeader() {
		headerlen = 4;
		version = 4;
		typeofoservice = 0;
		packetlen = 0;
		identifier = 0;
		flags = 0;
		ttl = 0;
		protocol = 0;
		checksum = 0;
		sourceIP = 0;
		destIP = 0;
	}
	IPHeader(const IPHeader &other) {
		headerlen = other.headerlen;
		version = other.version;
		typeofoservice = other.typeofoservice;
		packetlen = other.packetlen;
		identifier = other.identifier;
		flags = other.flags;
		ttl = other.ttl;
		protocol = other.protocol;
		checksum = other.checksum;
		sourceIP = other.sourceIP;
		destIP = other.destIP;
	}
	~IPHeader() {}
	IPHeader& operator=(const IPHeader &other) {
		headerlen = other.headerlen;
		version = other.version;
		typeofoservice = other.typeofoservice;
		packetlen = other.packetlen;
		identifier = other.identifier;
		flags = other.flags;
		ttl = other.ttl;
		protocol = other.protocol;
		checksum = other.checksum;
		sourceIP = other.sourceIP;
		destIP = other.destIP;
		return *this;
	}
	bool operator==(const IPHeader &other) const {
		return (headerlen == other.headerlen &&
			version == other.version &&
			typeofoservice == other.typeofoservice &&
			packetlen == other.packetlen &&
			identifier == other.identifier &&
			flags == other.flags &&
			ttl == other.ttl &&
			protocol == other.protocol &&
			checksum == other.checksum &&
			sourceIP == other.sourceIP &&
			destIP == other.destIP);
	}
	bool operator!=(const IPHeader &other) const {
		return (headerlen != other.headerlen ||
			version != other.version ||
			typeofoservice != other.typeofoservice ||
			packetlen != other.packetlen ||
			identifier != other.identifier ||
			flags != other.flags ||
			ttl != other.ttl ||
			protocol != other.protocol ||
			checksum != other.checksum ||
			sourceIP != other.sourceIP ||
			destIP != other.destIP);
	}
	unsigned int   headerlen;		// Length of the header
	unsigned int   version;			// Version of IP
	unsigned char  typeofoservice;	// Type of service
	unsigned short packetlen;		// Total length of the packet
	unsigned short identifier;		// Unique identifier
	unsigned short flags;			// Flags
	unsigned char  ttl;				// Time to live
	unsigned char  protocol;		// Protocol (TCP, UDP etc)
	unsigned short checksum;		// IP checksum
	unsigned int   sourceIP;		// Source IP
	unsigned int   destIP;			// Destination IP
};

NetOpResult ping(std::vector<PingResult> &results, const std::string address, const unsigned short numAttempts = 4,
	const unsigned short timeout = 1000, const unsigned short timeoutBetweenPings = 1000);
NetOpResult ping(std::vector<PingResult> &results, const std::wstring address, const unsigned short numAttempts = 4,
	const unsigned short timeout = 1000, const unsigned short timeoutBetweenPings = 1000);
NetOpResult traceroute(std::vector<TracertResult> &results, const std::string address, const unsigned char maxHops = 30,
	const unsigned short int tracertTimeout = 1000, const bool doPings = true, const unsigned short int pingAttempts = 4,
	const unsigned short int pingTimeout = 1000, const unsigned short int timeoutBetweenPings = 1000);
NetOpResult traceroute(std::vector<TracertResult> &results, const std::wstring address, const unsigned char maxHops = 30,
	const unsigned short int tracertTimeout = 1000, const bool doPings = true, const unsigned short int pingAttempts = 4,
	const unsigned short int pingTimeout = 1000, const unsigned short int timeoutBetweenPings = 1000);
NetOpResult traceroute_RawSocket(std::vector<TracertResult> &results,
	const std::string address, const unsigned char maxHops = 30);
NetOpResult traceroute_RawSocket(std::vector<TracertResult> &results,
	const std::wstring address, const unsigned char maxHops = 30);
std::string lookupIPAddress(const std::string dnsName);
std::wstring lookupIPAddress(const std::wstring dnsName);
std::string getHostname(const std::string ip, unsigned short int port = 27015);
std::wstring getHostname(const std::wstring ip, unsigned short int port = 27015);
unsigned short ICMPHeaderChecksum(unsigned short* buffer, int size);
int decodeResponse(char* buf, int bytes, SOCKADDR_IN* from, int ttl);

#endif // _NETHELPER_H
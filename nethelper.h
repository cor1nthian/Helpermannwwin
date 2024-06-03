#ifndef _NETHELPER_H
#define _NETHELPER_H

#include "strhelper.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

struct PingResult {
	PingResult() { Result = false; tripTime = 0; }
	PingResult(const PingResult &other) {
		Result = other.Result;
		tripTime = other.tripTime;
	}
	PingResult& operator=(const PingResult &other) {
		Result = other.Result;
		tripTime = other.tripTime;
		return *this;
	}
	bool operator!=(const PingResult &other) const {
		return(Result != other.Result || tripTime != other.tripTime);
	}
	bool operator==(const PingResult &other) const {
		return(Result == other.Result && tripTime == other.tripTime);
	}
	~PingResult() {}
	bool Result;
	short int tripTime;
};

PingResult ping(const std::string address);
PingResult ping(const std::wstring address);
std::string lookupIPAddress(const std::string dnsName);
std::wstring lookupIPAddress(const std::wstring dnsName);
std::string getHostname(const std::string ip, unsigned short int port = 27015);
std::wstring getHostname(const std::wstring ip, unsigned short int port = 27015);

#endif // _NETHELPER_H
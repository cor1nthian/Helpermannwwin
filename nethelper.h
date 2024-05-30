#ifndef _NETHELPER_H
#define _NETHELPER_H

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

PingResult ping(const char* address);
char* lookupIPAddress(const char* dnsName);
char* getHostname(const char* ip, unsigned short int port = 27015);

#endif // _NETHELPER_H
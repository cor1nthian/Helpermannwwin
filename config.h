#ifndef _CONFIG_H
#define _CONFIG_H

// Common Defines

// malloc with ease
#define MALLOC_NULLIFY(x, type, sz) type* x = (type*)malloc(sz); if(x) { *x = { 0 }; }

// malloc with ease
#define MALLOC_NULLIFY_NO_REDEFINE(x, type, sz) x = (type*)malloc(sz); if(x) { *x = { 0 }; }

// malloc with ease
#define MALLOC_NULLIFY_NO_REDEFINE_NO_CONV(x, sz) x = malloc(sz); if(x) { *x = { 0 }; }

// new with ease
#define NEW_NULLIFY(x, type) type* x = 0; x = new type; if(x) { *x = { 0 }; }

// new with ease
#define NEW_ARR_NULLIFY_NO_REDEFINE(x, type, sz) x = new type[sz]; if(x) { *x = { 0 }; }

// new with ease
#define NEW_ARR_NULLIFY(x, type, sz) type* x = 0; x = new type[sz]; if(x) { *x = { 0 }; }

// new with ease
#define NEW_ARR_NULLIFY_ET(x, type, sz) type x = 0; x = new type[sz]; if(x) { *x = { 0 }; }

// safety first! no errors trying to at least delete this pointer further
#define SAFE_DELETE(x) delete x; x = 0

// safety first! no errors trying to at least delete this pointer further
#define SAFE_ARR_DELETE(x) delete[] x; x = 0

// safety first! no errors trying to at least free this pointer further
#define SAFE_FREE(x) free(x); x = 0

// safety first! no errors trying to at least free this pointer further
#define SAFE_NetApiBufferFree(x) ::NetApiBufferFree(x); x = 0

// safety first! no errors trying to at least free this pointer further
#define SAFE_LOCALFREE(x) ::LocalFree((HLOCAL)x); x =0

// can be used for short, unsigned short, word, unsigned word (2-byte types)
#define BYTESWAP16(n) (((n&0xFF00)>>8)|((n&0x00FF)<<8))

// can be used for int or unsigned int or float (4-byte types)
#define BYTESWAP32(n) ((BYTESWAP16((n&0xFFFF0000)>>16))|((BYTESWAP16(n&0x0000FFFF))<<16))

// can be used for unsigned long long or double (8-byte types)
#define BYTESWAP64(n) ((BYTESWAP32((n&0xFFFFFFFF00000000)>>32))|((BYTESWAP32(n&0x00000000FFFFFFFF))<<32))

// Disable warnings
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

// Libs
#if defined(_WIN32) || defined(_WIN64)
	#pragma comment(lib, "Kernel32.lib")
	#pragma comment(lib, "Wininet.lib")
	#pragma comment(lib, "iphlpapi.lib")
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "Dnsapi.lib")
	#pragma comment(lib, "Advapi32.lib")
	#pragma comment(lib, "netapi32.lib")
#endif

// NetHelper Defines
#ifdef _DEBUG
	#define DNSHELPER_SHOWERRORMSGBOX
#endif

#endif

#ifndef _CONFIG_H
#define _CONFIG_H

// Common Defines

typedef long NTSTATUS;

// Macro Defines

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
#define SAFE_LOCALFREE(x) ::LocalFree((::HLOCAL)x); x =0

// can be used for short, unsigned short, word, unsigned word (2-byte types)
#define BYTESWAP16(n) (((n&0xFF00)>>8)|((n&0x00FF)<<8))

// can be used for int or unsigned int or float (4-byte types)
#define BYTESWAP32(n) ((BYTESWAP16((n&0xFFFF0000)>>16))|((BYTESWAP16(n&0x0000FFFF))<<16))

// can be used for unsigned long long or double (8-byte types)
#define BYTESWAP64(n) ((BYTESWAP32((n&0xFFFFFFFF00000000)>>32))|((BYTESWAP32(n&0x00000000FFFFFFFF))<<32))

#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))

#ifndef _MSC_VER
    #define MAKEWORD(a, b)  ((unsigned short)(((unsigned char)(((unsigned long long)(a)) & 0xFF)) | ((unsigned short)((unsigned char)(((unsigned long long)(b)) & 0xFF))) << 8))
    #define MAKELONG(a, b)  ((long)(((unsigned short)(((unsigned long long)(a)) & 0xFFFF)) | ((unsigned long)((unsigned short)(((unsigned long long)(b)) & 0xFFFF))) << 16))
    #define LOWORD(l)       ((unsigned short)(((unsigned long long)(l)) & 0xFFFF))
    #define HIWORD(l)       ((unsigned short)((((unsigned long long)(l)) >> 16) & 0xFFFF))
    #define LOBYTE(w)       ((unsigned char)(((unsigned long long)(w)) & 0xFF))
    #define HIBYTE(w)       ((unsigned char)((((unsigned long long)(w)) >> 8) & 0xFF))
#endif

// C++ version

/* With MSVS build with flags: 
    /std:c++latest /Zc:__cplusplus
    Project properties => C/C++ => Command Line => Additional flags */
#if defined(_MSC_VER) && (defined(_WIN32) || defined(_WIN64))
    #if (__cplusplus == 202101L)
        #define COMPILERVER 23
    #elif (__cplusplus == 202002L) || (__cplusplus == 202004L)
        #define COMPILERVER 20
    #elif (__cplusplus == 201703L)
        #define COMPILERVER 17
    #elif (__cplusplus == 201402L)
        #define COMPILERVER 14
    #elif (__cplusplus == 201103L)
        #define COMPILERVER 11
    #elif (__cplusplus == 199711L)
        #define COMPILERVER 98
    #endif
#endif

// Disable warnings
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

// Libs
#if (defined(_WIN32) || defined(_WIN64))
	#pragma comment(lib, "Kernel32.lib")
	#pragma comment(lib, "Wininet.lib")
	#pragma comment(lib, "iphlpapi.lib")
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "Dnsapi.lib")
	#pragma comment(lib, "netapi32.lib")
    #pragma comment(lib, "Advapi32.lib")
#endif

// FSHelper Defines

#define DE_SAMEFILE 			0x71
#define DE_MANYSRC1DEST			0x72
#define DE_DIFFDIR				0x73
#define DE_ROOTDIR				0x74
#define DE_OPCANCELLED			0x75
#define DE_DESTSUBTREE			0x76
#define DE_ACCESSDENIEDSRC 		0x78
#define DE_PATHTOODEEP			0x79
#define DE_MANYDEST				0x7A
#define DE_INVALIDFILES 		0x7C
#define DE_DESTSAMETREE 		0x7D
#define DE_FLDDESTISFILE 		0x7E
#define DE_FILEDESTISFLD 		0x80
#define DE_FILENAMETOOLONG 		0x81
#define DE_DEST_IS_CDROM 		0x82
#define DE_DEST_IS_DVD			0x83
#define DE_DEST_IS_CDRECORD		0x84
#define DE_FILE_TOO_LARGE 		0x85
#define DE_SRC_IS_CDROM 		0x86
#define DE_SRC_IS_DVD			0x87
#define DE_SRC_IS_CDRECORD		0x88
#define DE_ERROR_MAX			0xB7
#define DE_UNKNOWN				0x402
#define DE_ERRORONDEST			0x10000
#define DE_CANTRENAME			0x10074

#define WM_USER                 0x0400

#define FSH_SHORTNAMELENGTH		12 // symbols
#define FSH_PHYSDRIVEBUFLEN     256
#define FSH_FULLPHYSDRIVESTRING

#define STATUS_SUCCESS			(NTSTATUS)0x00000000L
#define NT_SUCCESS(Status)		((NTSTATUS)(Status) >= 0)
#define FILE_OPEN				0x00000001
#define FILE_OPEN_BY_FILE_ID	0×00002000
#define OBJ_CASE_INSENSITIVE	0x00000040L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define FILE_DIRECTORY_FILE		0x00000001
#define FILE_NON_DIRECTORY_FILE	0x00000040

#define InitializeObjectAttributes( p, n, a, r, s ) {	\
    (p)->uLength = sizeof( ObjectAttributes );			\
    (p)->hRootDirectory = r;                            \
    (p)->uAttributes = a;                               \
    (p)->pObjectName = n;                               \
    (p)->pSecurityDescriptor = s;                       \
    (p)->pSecurityQualityOfService = 0;					\
}

// NetHelper Defines

#ifdef _DEBUG
	#define DNSHELPER_SHOWERRORMSGBOX
#endif

// DBHelper defines

// max database connections
#define MSSQLNULLSIZE			6
#define MSSQLMAXCONN			64
#define MSSQLDISPLAYMAX			64
#define MSSQLDISPLAYFORMATEXTRA	3  // Per column extra display bytes (| <data> )
#define MSSQLDISPLAYFORMAT		L"%c %*.*s "
#define MSSQLDISPLAYFORMATC		L"%c %-*.*s "
// max connection out buffer size, characters
#define MSSQLMAXOUTBUF			2048
#define MSSQLMAXINFOBUF			1024
// min connection out buffer size, characters
#define MSSQLMINOUTBUF			1024
#define MSSQLCONNTOOMANY		-1

#endif
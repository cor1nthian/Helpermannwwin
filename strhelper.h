#ifndef _STRHELPER_H
#define _STRHELPER_H

/*       In loving memory of @pixo $oft       */
#if defined(_WIN32) || defined(_WIN64)
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
#endif

#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

// #include <codecvt>
#if defined(_WIN32) || defined(_WIN64)
	#include <combaseapi.h>
#endif
#include <string>
#include <locale>
#include <algorithm>
#include <cctype>
#include <vector>
#include <map>
#include <regex>
#include "config.h"

// new with ease
#define NEW_NULLIFY(x, type) type* x = 0; x = new type; *x = { 0 }

// new with ease
#define NEW_ARR_NULLIFY(x, type, sz) type* x = 0; x = new type[sz]; *x = { 0 }

// new with ease
#define NEW_ARR_NULLIFY_ET(x, type, sz) type x = 0; x = new type[sz]; *x = { 0 }

// new with ease
#define NEW_ARR_NULLIFY_NO_REDEFINE(x, type, sz) x = new type[sz]; if(x) { *x = { 0 }; }

// safety first! no errors trying to at least delete this pointer further
#define SAFE_DELETE(x) delete x; x = 0

// safety first! no errors trying to at least delete this pointer further
#define SAFE_ARR_DELETE(x) delete[] x; x = 0

// safety first! no errors trying to at least free this pointer further
#define SAFE_FREE(x) free(x); x = 0

LIBCOMPONENT size_t wcslen_c(const wchar_t* line);
LIBCOMPONENT bool contentInBuf(const char* buf, size_t bufSz);
LIBCOMPONENT bool contentInBuf(const wchar_t* buf, size_t bufSz);
LIBCOMPONENT wchar_t* char2wchar(const char* linee, wchar_t* buf = 0);
LIBCOMPONENT char* wchar2char(const wchar_t* linee, char* buf = 0);
LIBCOMPONENT std::string ul2hexstr(const unsigned long val);
LIBCOMPONENT std::wstring ul2hexwstr(const unsigned long val);
LIBCOMPONENT std::string ull2hexstr(const unsigned long long val);
LIBCOMPONENT std::wstring ull2hexwstr(const unsigned long long val);
LIBCOMPONENT unsigned long str2ul(const std::string val);
LIBCOMPONENT unsigned long wstr2ul(const std::wstring val);
LIBCOMPONENT unsigned long long str2ull(const std::string val);
LIBCOMPONENT unsigned long long wstr2ull(const std::wstring val);
LIBCOMPONENT char* replaceChars(const char* source, const char* replaceWhat, const char* replaceWith,
LIBCOMPONENT const size_t srclen = 0, const size_t replacewhatlen = 0, const size_t replacewithlen = 0);
LIBCOMPONENT wchar_t* replaceChars(const wchar_t* source, const wchar_t* replaceWhat, const wchar_t* replaceWith,
LIBCOMPONENT const size_t srclen = 0, const size_t replacewhatlen = 0, const size_t replacewithlen = 0);
LIBCOMPONENT void lower_chars(char* chrs);
LIBCOMPONENT void lower_chars(wchar_t* chrs);
LIBCOMPONENT char* lower_chars_copy(const char* chrs);
LIBCOMPONENT wchar_t* lower_chars_copy(const wchar_t* chrs);
LIBCOMPONENT void upper_chars(char* chrs);
LIBCOMPONENT void upper_chars(wchar_t* chrs);
LIBCOMPONENT char* upper_chars_copy(const char* chrs);
LIBCOMPONENT wchar_t* upper_chars_copy(const wchar_t* chrs);
LIBCOMPONENT std::string lower_copy(const std::string s);
LIBCOMPONENT std::string upper_copy(const std::string s);
LIBCOMPONENT std::wstring lower_copy(const std::wstring s);
LIBCOMPONENT std::wstring upper_copy(const std::wstring s);
LIBCOMPONENT std::string wstr2str(const std::wstring str);
LIBCOMPONENT std::wstring str2wstr(const std::string str);
LIBCOMPONENT std::string GUID2str(const GUID guid);
LIBCOMPONENT std::wstring GUID2wstr(const GUID guid);
#if defined(_WIN32) || defined(_WIN64)
	LIBCOMPONENT std::string GUID2strMS(const GUID guid);
	LIBCOMPONENT std::wstring GUID2wstrMS(const GUID guid);
#endif
LIBCOMPONENT GUID str2GUID(const std::string str);
LIBCOMPONENT GUID wstr2GUID(const std::wstring str);
#if defined(_WIN32) || defined(_WIN64)
	LIBCOMPONENT GUID str2GUIDMS(const std::string str);
	LIBCOMPONENT GUID wstr2GUIDMS(const std::wstring str);
#endif
LIBCOMPONENT void ltrim(std::string &s);
LIBCOMPONENT void rtrim(std::string &s);
LIBCOMPONENT void trim(std::string &s);
LIBCOMPONENT std::string ltrim_copy(const std::string s);
LIBCOMPONENT std::string rtrim_copy(const std::string s);
LIBCOMPONENT std::string trim_copy(const std::string s);
LIBCOMPONENT void ltrim(std::wstring &s);
LIBCOMPONENT void rtrim(std::wstring &s);
LIBCOMPONENT void trim(std::wstring &s);
LIBCOMPONENT std::wstring ltrim_copy(const std::wstring s);
LIBCOMPONENT std::wstring rtrim_copy(const std::wstring s);
LIBCOMPONENT std::wstring trim_copy(const std::wstring s);
LIBCOMPONENT void lower(std::string &s);
LIBCOMPONENT void lower(std::wstring &s);
LIBCOMPONENT void upper(std::string &s);
LIBCOMPONENT void upper(std::wstring &s);
LIBCOMPONENT std::string lower_copy(const std::string s);
LIBCOMPONENT std::string upper_copy(const std::string s);
LIBCOMPONENT std::wstring lower_copy(const std::wstring s);
LIBCOMPONENT std::wstring upper_copy(const std::wstring s);
LIBCOMPONENT void prepRegexString(std::string &regexString);
LIBCOMPONENT void prepRegexString(std::wstring &regexString);
LIBCOMPONENT std::string prepRegexString_copy(const std::string regexString);
LIBCOMPONENT std::wstring prepRegexString_copy(const std::wstring regexString);
LIBCOMPONENT void removeFromStart(std::string& source, const std::string sequence, const bool ignoreCase = true);
LIBCOMPONENT void removeFromStart(std::wstring& source, const std::wstring sequence, const bool ignoreCase = true);
LIBCOMPONENT void removeFromEnd(std::string& source, const std::string sequence, const bool ignoreCase = true);
LIBCOMPONENT void removeFromEnd(std::wstring& source, const std::wstring sequence, const bool ignoreCase = true);
LIBCOMPONENT void removeFromBothSides(std::string& source, const std::string sequence, const bool ignoreCase = true);
LIBCOMPONENT void removeFromBothSides(std::wstring& source, const std::wstring sequence, const bool ignoreCase = true);
LIBCOMPONENT std::string removeFromStart_copy(const std::string source, const std::string sequence, const bool ignoreCase = true);
LIBCOMPONENT std::wstring removeFromStart_copy(const std::wstring source, const std::wstring sequence, const bool ignoreCase = true);
LIBCOMPONENT std::string removeFromEnd_copy(const std::string source, const std::string sequence, const bool ignoreCase = true);
LIBCOMPONENT std::wstring removeFromEnd_copy(const std::wstring source, const std::wstring sequence, const bool ignoreCase = true);
LIBCOMPONENT std::string removeFromBothSides_copy(const std::string source, const std::string sequence, const bool ignoreCase = true);
LIBCOMPONENT std::wstring removeFromBothSides_copy(const std::wstring source, const std::wstring sequence, const bool ignoreCase = true);
LIBCOMPONENT bool startsWith(const std::string source, const std::string sequence, const bool ignoreCase = true);
LIBCOMPONENT bool startsWith(const std::wstring source, const std::wstring sequence, const bool ignoreCase = true);
LIBCOMPONENT bool endsWith(const std::string source, const std::string sequence, const bool ignoreCase = true);
LIBCOMPONENT bool endsWith(const std::wstring source, const std::wstring sequence, const bool ignoreCase = true);
LIBCOMPONENT bool partialMatch(const std::string source, const std::string compare, const bool ignoreCase = true);
LIBCOMPONENT bool partialMatch(const std::wstring source, const std::wstring compare, const bool ignoreCase = true);
LIBCOMPONENT size_t countOccurence(const std::string source, const std::string sequence);
LIBCOMPONENT size_t countOccurence(const std::wstring source, const std::wstring sequence);
LIBCOMPONENT std::vector<std::string> splitStr(const std::string str, const char* delimiter = "?", const bool includeEmpty = false);
LIBCOMPONENT std::vector<std::wstring> splitStr(const std::wstring str, const wchar_t* delimiter = L"?", const bool includeEmpty = false);
LIBCOMPONENT std::map<std::string, std::vector<std::string>> splitStr(const std::string str, const std::vector<const char*> delimiters, const bool ignoreCase = true, const bool includeEmpty = false);
LIBCOMPONENT std::map<std::wstring, std::vector<std::wstring>> splitStr(const std::wstring str, const std::vector<const wchar_t*> delimiters, const bool ignoreCase = true, const bool includeEmpty = false);
LIBCOMPONENT std::string joinStrs(const std::vector<std::string> strs, const std::string delimiter = "",
	const size_t startIdx = 0, const size_t endIdx = 0);
LIBCOMPONENT std::wstring joinStrs(const std::vector<std::wstring> strs, const std::wstring delimiter = L"",
	const size_t startIdx = 0, const size_t endIdx = 0);
LIBCOMPONENT std::string reverseJoinStrs(const std::vector<std::string> strs, const std::string delimiter = "",
	const size_t startIdx = 0, const size_t endIdx = 0);
LIBCOMPONENT std::wstring reverseJoinStrs(const std::vector<std::wstring> strs, const std::wstring delimiter = L"",
	const size_t startIdx = 0, const size_t endIdx = 0);
LIBCOMPONENT void removeSubstr(std::string& str, const std::string substr, const bool ignoreCase = true);
LIBCOMPONENT void removeSubstr(std::wstring& str, const std::wstring substr, const bool ignoreCase = true);
LIBCOMPONENT void removeSubstr(std::string& str, const std::vector<std::string> sequences, const bool ignoreCase = true);
LIBCOMPONENT std::string removeSubstr_copy(const std::string str, const std::string substr, const bool ignoreCase = true);
LIBCOMPONENT std::wstring removeSubstr_copy(const std::wstring str, const std::wstring substr, const bool ignoreCase = true);
LIBCOMPONENT std::string removeSubstr_copy(const std::string str, const std::vector <std::string> sequences, const bool ignoreCase = true);
LIBCOMPONENT std::wstring removeSubstr_copy(const std::wstring str, const std::vector <std::wstring> sequences, const bool ignoreCase = true);
LIBCOMPONENT std::string replaceAll(const std::string source, const std::string replaceWhat, const  std::string replaceWith);
LIBCOMPONENT std::wstring replaceAll(const std::wstring source, const std::wstring replaceWhat, const  std::wstring replaceWith);
LIBCOMPONENT void replaceSubstr(std::string& source, const std::string replaceWhat, const std::string replaceWith, const bool ignoreCase = true);
LIBCOMPONENT void replaceSubstr(std::wstring& source, const std::wstring replaceWhat, const std::wstring replaceWith, const bool ignoreCase = true);
LIBCOMPONENT std::string replaceSubstr_copy(const std::string source, const std::string replaceWhat, const std::string replaceWith,
	const bool ignoreCase = true);
LIBCOMPONENT std::wstring replaceSubstr_copy(const std::wstring source, const std::wstring replaceWhat, const std::wstring replaceWith,
	const bool ignoreCase = true);
LIBCOMPONENT void reverseString(std::string &str);
LIBCOMPONENT void reverseString(std::wstring &str);
LIBCOMPONENT std::string reverseString_copy(const std::string str);
LIBCOMPONENT std::wstring reverseString_copy(const std::wstring str);
LIBCOMPONENT void reverseIPV4(std::string &ipaddrstr);
LIBCOMPONENT void reverseIPV4(std::wstring &ipaddrstr);
LIBCOMPONENT std::string reverseIPV4_copy(const std::string ipaddrstr);
LIBCOMPONENT std::wstring reverseIPV4_copy(const std::wstring ipaddrstr);
LIBCOMPONENT void reverseIPV6(std::string &ipaddrstr);
LIBCOMPONENT void reverseIPV6(std::wstring &ipaddrstr);
LIBCOMPONENT std::string reverseIPV6_copy(const std::string ipaddrstr);
LIBCOMPONENT std::wstring reverseIPV6_copy(const std::wstring ipaddrstr);
LIBCOMPONENT std::string firstNumberFromString(const std::string line);
LIBCOMPONENT std::wstring firstNumberFromString(const std::wstring line);
LIBCOMPONENT bool valInList(const std::vector<std::string> &list, const std::string val, const bool ignoreCase = true);
LIBCOMPONENT bool valInList(const std::vector<std::wstring> &list, const std::wstring val, const bool ignoreCase = true);
/* Returns:
   0 - not numeric
   1 - numeric w/o fract part
   2 - numeric with fract part */
LIBCOMPONENT unsigned char isNumericDec(const std::wstring str);
/* Returns:
   0 - not numeric
   1 - numeric w/o fract part
   2 - numeric with fract part */
LIBCOMPONENT unsigned char isNumericDec(const std::wstring str);
/* Returns:
   0 - ipv4
   1 - ipv6
   2 - dns name
   3 - none */
LIBCOMPONENT unsigned char isStringIP(const std::string testStr);
/* Returns:
   0 - ipv4
   1 - ipv6
   2 - dns name
   3 - none */
LIBCOMPONENT unsigned char isStringIP(const std::wstring testStr);

#endif // !_STRHELPER_H
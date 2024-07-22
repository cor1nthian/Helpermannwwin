#ifndef _CONFIG_H
#define _CONFIG_H

//Common Defines
#define DLLBUILD1

#ifdef DLLBUILD
	#define LIBCOMPONENT __declspec(dllexport)
#else
	#define LIBCOMPONENT // __declspec(dllimport)
#endif

// NetHelper Defines
#ifdef _DEBUG
	#define DNSHELPER_SHOWERRORMSGBOX
#endif

#endif

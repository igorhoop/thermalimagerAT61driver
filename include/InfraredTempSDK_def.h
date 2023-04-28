#pragma once
#include "InfEntity.h"


#if defined(JUST_WIN32)
#	ifdef INFRAREDTEMPSDK_EXPORTS
#		ifdef __cplusplus
#			define INFRAREDTEMPSDK_API extern "C" __declspec(dllexport)
#		else
#			define INFRAREDTEMPSDK_API __declspec(dllexport)
#		endif
#	else
#		ifdef __cplusplus
#			define INFRAREDTEMPSDK_API extern "C" __declspec(dllimport)
#		else
#			define INFRAREDTEMPSDK_API __declspec(dllimport)
#		endif
#	endif
//#endif
#elif defined(JUST_LINUX)
#	ifdef __cplusplus
#		define INFRAREDTEMPSDK_API extern "C"
#	else
#		define INFRAREDTEMPSDK_API
#	endif
#endif

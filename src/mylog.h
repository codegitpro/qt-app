#pragma once
#include "general.h"

namespace ai
{
#ifdef ANDROID
#include <android/log.h>
	static void out_log(const char* file, int line, const str& msg, int prio)
	{
		auto tag = str(file) + " (" + std::to_string(line) + ")";
		__android_log_write(prio, tag.c_str(), msg.c_str());
	}
#define LOGV(message) out_log(__FILE__,__LINE__,message,ANDROID_LOG_VERBOSE)
#define LOGD(message) out_log(__FILE__,__LINE__,message,ANDROID_LOG_DEBUG)
#define LOGI(message) out_log(__FILE__,__LINE__,message,ANDROID_LOG_INFO)
#define LOGW(message) out_log(__FILE__,__LINE__,message,ANDROID_LOG_WARN)
#define LOGE(message) out_log(__FILE__,__LINE__,message,ANDROID_LOG_ERROR)
#else
#include <iostream>
	static str format_log_message(const char* file, int line, const str& msg)
	{
		return msg + " " + str(file) + " (" + std::to_string(line) + ")";
	}
	static void out_log(const char* file, int line, const str& msg)
	{
		std::cout << format_log_message(file, line, msg) << "\n";
	}
	static void err_log(const char* file, int line, const str& msg)
	{
		std::cerr << format_log_message(file, line, msg) << "\n";
	}

#define LOGV(message) out_log(__FILE__,__LINE__,message)
#define LOGD(message) out_log(__FILE__,__LINE__,message)
#define LOGI(message) out_log(__FILE__,__LINE__,message)
#define LOGW(message) out_log(__FILE__,__LINE__,message)
#define LOGE(message) err_log(__FILE__,__LINE__,message)
#endif 

}
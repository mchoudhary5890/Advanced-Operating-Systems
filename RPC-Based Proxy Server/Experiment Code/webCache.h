#ifndef WEBCACHE_H
#define WEBCACHE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <curl/curl.h>
#include <map>
#include <string>
#include <iostream>

class WebCache
{
        protected:
	std::map <std::string, std::string> cachedWebPages;
	size_t maxwebCacheSize;
	size_t currentWebCacheSize;
	long cacheEntryRemovalCount; 
	public:
	WebCache(size_t);
        WebCache();
        size_t getMaxWebCacheSize();
	long getCacheEntryRemovalCount();
        static 	size_t write_data(void *, size_t, size_t, void *); 
	bool isWebCacheEmpty();
	bool haveSpaceInWebCache(size_t);
	void insertInWebCache(std::string, std::string);
        virtual void cacheWebPage(std::string,std::string);
	virtual bool createSpaceInWebCache()=0;
	std::string fetchFromWeb(std::string);
	virtual std::string fetchFromWebCache(std::string);
};
#endif
 

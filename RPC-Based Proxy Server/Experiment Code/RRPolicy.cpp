#include "RRPolicy.h"

RRPolicy::RRPolicy(size_t UserDefinedSize): WebCache(UserDefinedSize){}
RRPolicy::RRPolicy():WebCache(){}

bool RRPolicy::createSpaceInWebCache()
{
	cacheEntryRemovalCount++;
	std::map<std::string, std::string>:: iterator webCacheIterator = cachedWebPages.begin();
	std::advance(webCacheIterator, rand() % cachedWebPages.size());
	std::string randomURL = webCacheIterator->first;
	currentWebCacheSize -= cachedWebPages[randomURL].size();
	cachedWebPages.erase(randomURL);
	return true;
} 

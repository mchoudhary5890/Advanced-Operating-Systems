#ifndef MAXSOLICY_H
#define MAXSPOLICY_H
#include "webCache.h"
class MAXSPolicy:public WebCache
{
	std::multimap<size_t, std::string> sizedURLMap;
	public:
		MAXSPolicy(size_t);
		MAXSPolicy();
		virtual bool createSpaceInWebCache(); 
                virtual void cacheWebPage(std::string, std::string);
                virtual std::string fetchFromWebCache(std::string);
		void insertInSizedURLMap(size_t, std::string);
};
#endif

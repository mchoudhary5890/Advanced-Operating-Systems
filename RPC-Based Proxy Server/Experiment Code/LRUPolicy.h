#ifndef LRUPOLICY_H
#define LRUPOLICY_H
#include "webCache.h"
#include <boost/bimap.hpp>
#include <boost/config.hpp>
#include <boost/bimap/support/lambda.hpp>
using namespace boost::bimaps;
typedef boost::bimap<long, std::string> boostMap;
class LRUPolicy:public WebCache
{
        boostMap timestampedURLMap;
        long timestamp;
	public:
		LRUPolicy(size_t);
		LRUPolicy();
		virtual bool createSpaceInWebCache(); 
                virtual void cacheWebPage(std::string, std::string);
                virtual std::string fetchFromWebCache(std::string);
		void insertInTimestampedURLMap(std::string);
};
#endif

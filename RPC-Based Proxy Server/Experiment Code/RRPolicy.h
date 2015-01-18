#ifndef RRPOLICY_H
#define RRPOLICY_H
#include "webCache.h"

class RRPolicy:public WebCache
{
	public:
		RRPolicy(size_t);
		RRPolicy();
                virtual bool createSpaceInWebCache(); 
};
#endif

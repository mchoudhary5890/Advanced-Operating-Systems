#include "MAXSPolicy.h"


MAXSPolicy::MAXSPolicy(size_t UserDefinedSize): WebCache(UserDefinedSize){}
MAXSPolicy::MAXSPolicy():WebCache(){}

bool MAXSPolicy::createSpaceInWebCache()
{
	cacheEntryRemovalCount++;
	std::string URLToDelete = sizedURLMap.rbegin()->second;	
	currentWebCacheSize -= cachedWebPages[URLToDelete].size();
        cachedWebPages.erase(URLToDelete);
        sizedURLMap.erase(--sizedURLMap.end());
	return true;
} 

void MAXSPolicy::cacheWebPage(std::string URL, std::string webPage) 
{
	while(!(haveSpaceInWebCache(webPage.size())))
	{
		createSpaceInWebCache();
	}
	insertInSizedURLMap(webPage.size(), URL);
	insertInWebCache(URL, webPage);
}

void MAXSPolicy::insertInSizedURLMap(size_t webPageSize,std::string URL)
{
	try
	{	
		 sizedURLMap.insert(std::pair<size_t, std::string>(webPageSize, URL));	
	}
	catch(...)
	{
		 std::cout << "Exception Occured in insertInSizedURLMap()";
	}
	
}

std::string MAXSPolicy::fetchFromWebCache(std::string URL)
{
	try
	{
		std::map <std::string, std::string> :: iterator webCacheIterator;
		webCacheIterator = cachedWebPages.find(URL);
		if(webCacheIterator == cachedWebPages.end())
		{
			return "";
		}
		else
		{
			return webCacheIterator->second;
		}
	}
	catch(...)
	{
		std::cout << "Exception occured in fetchFromWebCache()";
	}		
} 
   

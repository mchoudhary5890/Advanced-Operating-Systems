#include "LRUPolicy.h"

LRUPolicy::LRUPolicy(size_t UserDefinedSize): WebCache(UserDefinedSize),timestamp(0){}
LRUPolicy::LRUPolicy():WebCache(),timestamp(0){}

bool LRUPolicy::createSpaceInWebCache()
{
	cacheEntryRemovalCount++;
	std::string URLToDelete = timestampedURLMap.left.begin()->second;
	currentWebCacheSize -= cachedWebPages[URLToDelete].size();
        cachedWebPages.erase(URLToDelete);
	timestampedURLMap.left.erase(timestampedURLMap.left.begin());
	return true;
} 

void LRUPolicy::cacheWebPage(std::string URL, std::string webPage) 
{
	while(!(haveSpaceInWebCache(webPage.size())))
	{
		createSpaceInWebCache();
	}
	insertInTimestampedURLMap(URL);
	insertInWebCache(URL, webPage);
}

void LRUPolicy::insertInTimestampedURLMap(std::string URL)
{
	try
	{	
                 timestampedURLMap.left.insert(boostMap::left_value_type(timestamp++, URL));
	}
	catch(...)
	{
		 std::cout << "Exception Occured in insertInTimestampedURLMap()";
	}
	
}

std::string LRUPolicy::fetchFromWebCache(std::string URL)
{
	try
	{
		std::map <std::string, std::string> :: iterator webCacheIterator;
		webCacheIterator = cachedWebPages.find(URL);
		if(webCacheIterator == cachedWebPages.end())
			return "";
		else
                { 
 			boostMap::right_iterator rightItr = timestampedURLMap.right.find(URL);
			boostMap::left_iterator leftItr = timestampedURLMap.project_left(rightItr);
			timestampedURLMap.left.modify_key(leftItr, _key = timestamp++);	
			return webCacheIterator->second;
                } 
	}
	catch(...)
	{
		std::cout << "Exception occured in fetchFromWebCache()";
	}		
} 
   

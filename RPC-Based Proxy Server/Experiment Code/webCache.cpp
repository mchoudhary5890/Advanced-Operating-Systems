#include "webCache.h"

struct wd_in {
  size_t size;
  size_t len;
  char *data;
};


using namespace std;
WebCache::WebCache(size_t UserDefinedSize)
{
	maxwebCacheSize = UserDefinedSize;
	currentWebCacheSize = 0;
	cacheEntryRemovalCount	= 0;
}

WebCache::WebCache()
{
	maxwebCacheSize = 2048;	
	currentWebCacheSize = 0;
	cacheEntryRemovalCount = 0;
}

size_t WebCache::getMaxWebCacheSize()
{
	return maxwebCacheSize;
}

long WebCache::getCacheEntryRemovalCount()
{
	return cacheEntryRemovalCount;
}

size_t  WebCache::write_data(void *buffer, size_t size, size_t nmemb, void *userp) 
{
	struct wd_in *wdi = (struct wd_in*)userp;

  	while(wdi->len + (size * nmemb) >= wdi->size) 
	{
	   	wdi->data = (char*)realloc(wdi->data, wdi->size*2);
		if(!(wdi->data))
		{
			fprintf(stderr, "Error: could not allocate space using realloc.\n");
	    		exit(EXIT_FAILURE);
		}
	    	wdi->size*=2;
	}
	memcpy(wdi->data + wdi->len, buffer, size * nmemb);
  	wdi->len+=size*nmemb;
	return size * nmemb;
}

bool WebCache::isWebCacheEmpty()
{
	try
	{
		return cachedWebPages.empty();
	}
	catch (...)
	{
		std::cout << "Exception Occured in isWebCacheEmpty()";
	}
}

bool WebCache::haveSpaceInWebCache(size_t webPageSize)
{
	try
	{
		if((currentWebCacheSize + webPageSize) > maxwebCacheSize)
			return false;
		else
			return true;
	}
	catch (...)
	{
		std::cout << "Exception Occured in haveSpaceInWebCache()";
	}
}
			
void WebCache::insertInWebCache(std::string URL, std::string webPage)
{
	try
	{
		 cachedWebPages.insert( std::pair<std::string,std::string>(URL, webPage));
		 currentWebCacheSize += webPage.size();
	}
	catch(...)
	{
		std::cout << "Exception Occured in insertInWebCache()";
	}			
}

std::string WebCache::fetchFromWeb(std::string URL)
{
	try
	{
		CURL *curl;
	  	//CURLcode res;
	  	struct wd_in wdi;
		memset(&wdi, 0, sizeof(wdi));
		curl = curl_easy_init();

	  	if (NULL != curl) 
		{
	    		wdi.size = 1024;
	    		wdi.data = (char*)malloc(wdi.size);
			if(!(wdi.data))
			{
				fprintf(stderr, "Error: could not allocate space using malloc.\n");
	    			exit(EXIT_FAILURE);
			}

	    		/* Set the URL for the operation. */
	   		curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

		    	/* "write_data" function to call with returned data. */
		    	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WebCache::write_data);

		    	/* userp parameter passed to write_data. */
		    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &wdi);

		   	/* Actually perform the query. */
		    	//res = curl_easy_perform(curl);
			curl_easy_perform(curl);
		    	/* Check the return value and do whatever. */

		    	/* Clean up after ourselves. */
		    	curl_easy_cleanup(curl);
	  	}
	  	else 
		{
	    		fprintf(stderr, "Error: could not get CURL handle.\n");
	    		exit(EXIT_FAILURE);
	  	}

		/* Now wdi.data has the data from the GET and wdi.len is the length
		   of the data available, so do whatever. */

		/* Write the content to stdout. */
		//write(STDOUT_FILENO, wdi.data, wdi.len);	
		return wdi.data;
	}
	catch(...)
	{
		std::cout << "Exception occured in fetchFromWeb()";
	}
}

void WebCache::cacheWebPage(std::string URL, std::string webPage) 
{	try
	{
		bool haveSpace = false;
		while(!(haveSpace = haveSpaceInWebCache(webPage.size())))
		{
			createSpaceInWebCache();
		}
		insertInWebCache(URL, webPage);
	}
	catch(...)
	{
		std::cout << "Exception occured in cacheWebPage()";
	}
}


std::string WebCache::fetchFromWebCache(std::string URL)
{
	try
	{
		std::map <std::string, std::string> :: iterator webCacheIterator;
		webCacheIterator = cachedWebPages.find(URL);
		if(webCacheIterator == cachedWebPages.end())
			return "";
		else
			return webCacheIterator->second;
	}
	catch(...)
	{
		std::cout << "Exception occured in fetchFromWebCache()";
	}		
}

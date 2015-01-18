proxy_server proxy_client: ProxyService.o ProxyService_Server.o proxy_types.o proxy_constants.o webCache.o RRPolicy.o LRUPolicy.o MAXSPolicy.o ProxyService_Client.o
	g++ -L/usr/local/lib ProxyService.o ProxyService_Server.o proxy_types.o proxy_constants.o webCache.o RRPolicy.o LRUPolicy.o MAXSPolicy.o -o proxy_server -lthrift -lcurl
	g++ -L/usr/local/lib ProxyService_Client.o ProxyService.o proxy_constants.o proxy_types.o -o proxy_client -lthrift

ProxyService.o ProxyService_Server.o proxy_types.o  proxy_constants.o : ProxyService.cpp ProxyService_Server.cpp proxy_types.cpp proxy_constants.cpp
	g++  -I/usr/local/include/thrift -I. -c ProxyService.cpp -o ProxyService.o 
	g++  -I/usr/local/include/thrift -I. -c ProxyService_Server.cpp -o ProxyService_Server.o 
	g++  -I/usr/local/include/thrift -I. -c proxy_types.cpp -o proxy_types.o 
	g++  -I/usr/local/include/thrift -I. -c proxy_constants.cpp -o proxy_constants.o 

webCache.o RRPolicy.o LRUPolicy.o MAXSPolicy.o : webCache.cpp RRPolicy.cpp LRUPolicy.cpp MAXSPolicy.cpp
	g++  -I/usr/local/include/thrift -I. -c webCache.cpp -o webCache.o -lcurl
	g++  -I/usr/local/include/thrift -I. -c RRPolicy.cpp -o RRPolicy.o 
	g++  -I/usr/local/include/thrift -I. -c LRUPolicy.cpp -o LRUPolicy.o 
	g++  -I/usr/local/include/thrift -I. -c MAXSPolicy.cpp -o MAXSPolicy.o 


ProxyService_Client.o: ProxyService_Client.cpp
	g++  -I/usr/local/include/thrift -c ProxyService_Client.cpp -o ProxyService_Client.o


clean : 
	rm -f *.o proxy_server proxy_client

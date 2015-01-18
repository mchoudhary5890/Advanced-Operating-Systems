#include "ProxyService.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace ProxyServer;

int main(int argc, char **argv) 
{
	if(argc != 4)
	{
		std::cout << "Usage: " << argv[0] << " <URL> " << " <IP Address> " << " <Port> "; 
		return 0;
	}	
	boost::shared_ptr<TSocket> socket(new TSocket((std::string)argv[2], atoi(argv[3])));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	ProxyServiceClient client(protocol);
	transport->open();
	std::string url = (std::string)argv[1];	
	std::string fetchedPage;
	client.fetchWebPage(fetchedPage, url);
	std::cout << "The Fetched Web Page is :" << fetchedPage << "\n";
	transport->close();
	return 0;
}

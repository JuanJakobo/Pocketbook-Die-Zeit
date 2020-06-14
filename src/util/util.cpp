//------------------------------------------------------------------
// util.cpp
//
// Author:           JuanJakobo          
// Date:             14.06.2020
//   
//-------------------------------------------------------------------
#include "util.h"
#include "inkview.h"

#include <string>

size_t Util::writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

size_t Util::writeData(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
	//TODO error handling or ostream?
    size_t written =  iv_fwrite(ptr, size, nmemb, stream);
    return written;
}

bool Util::connectToNetwork()
{
	iv_netinfo *netinfo = NetInfo();
	if (netinfo->connected) 
		return true;

	const char *network_name = nullptr;
	int result = NetConnect2(network_name, 1);
	if (result != 0) {
        Message(3,"Warning","cannot connect to Internet.",200);
		return false;
	}

	netinfo = NetInfo();
	if (netinfo->connected) 
		return true;

    Message(3,"Warning","cannot connect to Internet.",200);
	return false;
}


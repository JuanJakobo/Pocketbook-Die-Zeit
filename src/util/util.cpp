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
#include <math.h>

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

int Util::progress_callback(void *clientp,   double dltotal,   double dlnow,   double ultotal,   double ulnow)
{
    if (dltotal <= 0.0)
        return 0;

    int percentage = round(dlnow/dltotal * 100);
	if(percentage%10==0)
    	UpdateProgressbar("Downloading issue",percentage);

    return 0;

}

string Util::readStringSetting(const std::string& name)
{
    iconfigedit* temp = nullptr;
    iconfig  *SettingsConfig = OpenConfig(Settings_PATH.c_str(),temp);
    std::string value = ReadString(SettingsConfig,name.c_str(),"");
    CloseConfigNoSave(SettingsConfig);

	return value;
}

void Util::writeStringSetting(const std::string& name, const std::string& value)
{
    iconfigedit* temp = nullptr;
    iconfig  *SettingsConfig = OpenConfig(Settings_PATH.c_str(),temp);
    WriteString(SettingsConfig,name.c_str(),value.c_str());
    CloseConfig(SettingsConfig);
}



//------------------------------------------------------------------
// util.cpp
//
// Author:           JuanJakobo          
// Date:             14.06.2020
//   
//-------------------------------------------------------------------

#include "inkview.h"

#include <string>

using namespace std;

const string Settings_PATH = "/mnt/ext1/system/config/dieZeit/settings.cfg";

class Util
{
public:

    static size_t   writeCallback(void *contents, size_t size, size_t nmemb, void *userp);
    static size_t   writeData(void *ptr, size_t size, size_t nmemb, FILE *stream);
    static bool     connectToNetwork();
    static int      progress_callback(void *clientp,   double dltotal,   double dlnow,   double ultotal,   double ulnow);

    static string   readStringSetting(const string& name);
    static void     writeStringSetting(const string& name, const string& value);


private:
    Util() {}
};
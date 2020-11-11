//------------------------------------------------------------------
// util.cpp
//
// Author:           JuanJakobo          
// Date:             14.06.2020
//   
//-------------------------------------------------------------------

#include "inkview.h"

#include <string>
#include <sstream>

using std::string;
using std::ostringstream;

const string Settings_PATH = "/mnt/ext1/system/config/dieZeit/settings.cfg";

class Util
{
public:
    /**
    * Converts an value to an string, as C++11 command is not supported
    * 
    * @param value the int value that shall be converted
    * @return same value in string format
    */
    template <typename T>
    static string valueToString(const T value)
    {
        ostringstream stm;
        stm << value;
        return stm.str();
    };

    /**
    * Handles the return of curl command
    * 
    */
    static size_t   writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

    /**
    * Saves the return of curl command
    * 
    */
    static size_t   writeData(void *ptr, size_t size, size_t nmemb, FILE *stream);

    /**
    * Checks if a network connection can be established
    * 
    * @return true - network access succeeded, false - network access failed
    */
    static bool     connectToNetwork();

    /**
    * Returns an integer representing the download progress
    * 
    */
    static int      progress_callback(void *clientp,   double dltotal,   double dlnow,   double ultotal,   double ulnow);

    static string   readStringSetting(const string& name);
    static void     writeStringSetting(const string& name, const string& value);


private:
    Util() {}
};
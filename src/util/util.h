//------------------------------------------------------------------
// util.cpp
//
// Author:           JuanJakobo
// Date:             14.06.2020
//
//-------------------------------------------------------------------

#ifndef UTIL
#define UTIL

#include "inkview.h"

#include <string>
#include <sstream>

using std::ostringstream;
using std::string;

class Util
{
public:
    /**
    * Handles the return of curl command
    * 
    */
    static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

    /**
    * Saves the return of curl command
    * 
    */
    static size_t writeData(void *ptr, size_t size, size_t nmemb, FILE *stream);

    /**
    * Checks if a network connection can be established
    * 
    * @return true - network access succeeded, false - network access failed
    */
    static bool connectToNetwork();

    /**
    * Returns an integer representing the download progress
    * 
    */
    static int progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

    static string readStringSetting(const string &name);
    static void writeStringSetting(const string &name, const string &value);

    /**
        * Updates the library of the Pocketbook
        * 
        */
    static void updatePBLibrary(int seconds);

private:
    Util() {}
};
#endif
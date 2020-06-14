//------------------------------------------------------------------
// util.cpp
//
// Author:           JuanJakobo          
// Date:             14.06.2020
//   
//-------------------------------------------------------------------

#include "inkview.h"

#include <string>

class Util
{
public:

    //TODO where to?
    static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);
    static size_t writeData(void *ptr, size_t size, size_t nmemb, FILE *stream);
    static bool connectToNetwork();



private:
    Util() {}
};
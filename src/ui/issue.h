//------------------------------------------------------------------
// issue.h
//
// Author:           JuanJakobo          
// Date:             14.06.2020
// Description:      An issue of an journal  
//-------------------------------------------------------------------

#ifndef ISSUE
#define ISSUE

#include "inkview.h"

#include <string>
#include <vector>


using namespace std;

class Issue
{
    public:

        Issue(const string& Title, const string& ContentUrl);

        bool operator== (const Issue &iss) const;
        
        void    setRect(irect* Rect);
        string  getTitle() const {return title;};

        void    draw();

    private:
        irect*  rect;
        irect   downloadButton;
        string  title;
        string  contentUrl;
        bool    downloaded;

};
#endif
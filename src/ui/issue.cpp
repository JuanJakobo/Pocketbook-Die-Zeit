//------------------------------------------------------------------
// issue.cpp
//
// Author:           JuanJakobo          
// Date:             14.06.2020
//  
//-------------------------------------------------------------------

#include "issue.h"
#include "inkview.h"
#include "util.h"
#include "eventHandler.h"

#include <string>


using namespace std;

Issue::Issue(const string& Title, const string& ContentUrl): title(Title), contentUrl(ContentUrl)
{
    downloaded = false;
}

void Issue::setRect(irect* Rect)
{
    rect = Rect;
}

bool Issue::operator== (const Issue &iss) const
{
    if(this->title==iss.title)
        return true;
    return false;
}

void Issue::draw()
{
    FillAreaRect(rect,WHITE);

    DrawTextRect(rect->x,rect->y,rect->w,rect->h,title.c_str(),ALIGN_LEFT);
    int line = (rect->y+rect->h)-1;
    DrawLine(0,line,ScreenWidth(),line,BLACK);
    downloadButton = iRect(0,rect->y,rect->w,30,ALIGN_RIGHT);
    DrawTextRect2(&downloadButton,"Download");
}


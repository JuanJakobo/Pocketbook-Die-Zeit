//------------------------------------------------------------------
// issue.cpp
//
// Author:           JuanJakobo          
// Date:             14.06.2020
//  
//-------------------------------------------------------------------

#include "issue.h"
#include "inkview.h"
#include "dieZeit.h"
#include "util.h"
#include "eventHandler.h"

#include <string>
#include <sstream>
#include <curl/curl.h>
#include <algorithm>

using namespace std;

Issue::Issue(const string& Title, const string& ContentUrl): title(Title), contentUrl(ContentUrl)
{
    downloaded = false;
}

Issue::Issue(istringstream& str_strm)
{
    string tmp;

    getline(str_strm, tmp, ',');
    title = tmp;

    getline(str_strm, tmp, ',');
    contentUrl = tmp;

    getline(str_strm, tmp, ',');
    downloadUrl = tmp;

    getline(str_strm, tmp, ',');
    content = tmp;

    getline(str_strm, tmp, ',');
    path = tmp;

    getline(str_strm, tmp, ',');
    hidden = (tmp=="1") ? true : false;

    getline(str_strm, tmp, ',');
    downloaded = (tmp=="1") ? true : false;
}

void Issue::setRect(irect Rect)
{
    rect = Rect;
}

bool Issue::operator== (const Issue &iss) const
{
    if(this->title==iss.title)
        return true;
    return false;
}

void Issue::draw(ifont* font)
{
    FillAreaRect(&rect,WHITE);

    SetFont(font, BLACK);

    DrawTextRect(rect.x,rect.y,rect.w,rect.h,title.c_str(),ALIGN_LEFT);

    SetFont(font, WHITE);
    
    if(isDownloaded())
    {
        readButton = iRect(ScreenWidth()-200,rect.y,200,50,ALIGN_CENTER);
        FillAreaRect(&readButton, BLACK);
        DrawTextRect2(&readButton,"Read");

        removeButton = iRect(ScreenWidth()-200,rect.y+55,200,50,ALIGN_CENTER);
        FillAreaRect(&removeButton, BLACK);
        DrawTextRect2(&removeButton,"Remove");
    }
    else
    {
        downloadButton = iRect(ScreenWidth()-200,rect.y,200,50,ALIGN_CENTER);
        FillAreaRect(&downloadButton, BLACK);
        DrawTextRect2(&downloadButton,"Download");         
    }

    int line = (rect.y+rect.h)-1;
    DrawLine(0,line,ScreenWidth(),line,BLACK);
}

void Issue::isClicked(int x, int y, ifont* font)
{
    if(isDownloaded())
    {
        if(IsInRect(x,y,&readButton)==1)
        {

        }
        else if(IsInRect(x,y,&removeButton)==1)
        {
            downloaded = false;
        }
    }
    else
    {
        if(IsInRect(x,y,&downloadButton)==1)
        {
            downloaded = true;
        }
    }

    draw(font);

    PartialUpdate(0,rect.y,ScreenWidth(),rect.h);

}
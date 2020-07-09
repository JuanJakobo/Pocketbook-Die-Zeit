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

#include <iostream>
#include <string>
#include <sstream>
#include <curl/curl.h>
#include <algorithm>

using namespace std;

Issue::Issue(const string& Title, const string& ContentUrl, const tm& ReleaseDate): title(Title), contentUrl(ContentUrl), releaseDate(ReleaseDate)
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

    getline(str_strm,tmp,',');
	sscanf(tmp.c_str(),"%2d.%2d.%4d",&releaseDate.tm_mday,&releaseDate.tm_mon,&releaseDate.tm_year);
    releaseDate.tm_mon = releaseDate.tm_mon -1;
    releaseDate.tm_year = releaseDate.tm_year -1900;

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

bool Issue::operator > (const Issue& iss) const
{
    if (releaseDate.tm_year > iss.releaseDate.tm_year)
      return true;
    if (releaseDate.tm_year == iss.releaseDate.tm_year && releaseDate.tm_mon > iss.releaseDate.tm_mon)
      return true;
    if (releaseDate.tm_year == iss.releaseDate.tm_year &&  releaseDate.tm_mon == iss.releaseDate.tm_mon && releaseDate.tm_mday > iss.releaseDate.tm_mday)
      return true;
    return false;
}

bool Issue::operator== (const Issue& iss) const
{
    if(this->title==iss.title)
        return true;
    return false;
}

ostream& operator<< (ostream &out, Issue const& iss) {
    out << iss.title << ',' << iss.contentUrl << ',' << iss.getReleaseDate() << ','  
        << iss.downloadUrl << ',' << iss.path << ',' << iss.content << ',' 
        << iss.hidden << ',' << iss.downloaded << endl; 
    return out;
}

string  Issue::getReleaseDate() const {    
    char buffer [80];
    strftime (buffer,80,"%d.%m.%Y",&releaseDate);
    return buffer;
}

bool Issue::getInformation()
{
    if(!Util::connectToNetwork())
        return false;

    std::string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, contentUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Util::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
        //curl_easy_setopt(curl, CURLOPT_COOKIESESSION, true);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE,DIEZEIT_COOOKIE_PATH.c_str());  
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res == CURLE_OK)
        {
            // view-source:https://epaper.zeit.de/abo/diezeit/20.05.2020
            //   <a class="btn btn-default btn-block btn-border btn-md" href="https://premium.zeit.de/system/files/2020-21/epub/die_zeit_2020_22.epub" data-wt-click="{ct: '#download_zeit+magazin', ck: {4: 'undefined', 5: 'content', 6: 'reader', 9: 'epub'}}">
            //      EPUB FÜR E-READER LADEN    </a>
            size_t found = readBuffer.find("https://premium.zeit.de/system");

            if (found!=std::string::npos)
            {
                readBuffer = readBuffer.substr (found, 200);
                found = readBuffer.find('"');
                downloadUrl = readBuffer.substr (0,found);
                return true;
            }
            else 
            {
                downloadUrl= "";
                Message(ICON_ERROR, "Error", "Failed to get Content.", 600);

            }        
        }
        else
        {
            Message(ICON_ERROR, "Error", curl_easy_strerror(res), 600);
        }
        
    }
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
        //void DrawBorder(const irect *border_rect, int border_size, int style, int radius, int color);
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
            eliminate();
            downloaded = false;
        }
    }
    else
    {
        if(IsInRect(x,y,&downloadButton)==1)
        {
            if(download())
                downloaded = true;
        }
    }

    draw(font);

    PartialUpdate(0,rect.y,ScreenWidth(),rect.h);

}

bool Issue::download()
{
    OpenProgressbar(1,"Downloading...","Check network connection",0,EventHandler::DialogHandlerStatic);

    if(downloadUrl=="")
    {
        CloseProgressbar();
        Message(1,"Error","Download URl not found.",100);
        return false;
    }

    if(!Util::connectToNetwork())
    {
        CloseProgressbar();
        return false;
    }

    FILE *fp;
    CURLcode res;

    string temppath = title;
    replace(temppath.begin(),temppath.end(),'/','_');
    replace(temppath.begin(),temppath.end(),' ','_');

    //TODO check if available
    path = DIEZEIT_ISSUE_PATH + temppath + ".epub";

    CURL *curl = curl_easy_init();

    if(curl)
    {
        fp = iv_fopen(path.c_str(),"wb");
        curl_easy_setopt(curl, CURLOPT_URL, downloadUrl.c_str());
        //curl_easy_setopt(curl, CURLOPT_COOKIESESSION, true);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE,DIEZEIT_COOOKIE_PATH.c_str());   
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Util::writeData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Util::progress_callback); 

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        iv_fclose(fp);

        if (res == CURLE_OK) 
        {
            downloaded = true;
            CloseProgressbar();
            return true;
        }
    }
    CloseProgressbar();
    //path = nullptr;
    return false;
}

void Issue::eliminate()
{
    if(iv_access(path.c_str(),W_OK)==0)
        remove(path.c_str());

}
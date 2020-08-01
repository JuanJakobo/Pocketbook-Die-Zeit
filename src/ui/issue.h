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
#include <iostream>

using namespace std;

class Issue
{
    public:

        Issue(const string& Title, const string& ContentUrl, const tm& ReleaseDate);
        explicit Issue(istringstream& str_strm);

        friend ostream& operator<< (ostream &out, Issue const& iss);
        bool operator== (const Issue& iss) const;
        bool operator > (const Issue& iss) const;

        void    setRect(irect Rect);
        irect*  getRect(){return &rect;};
        string  getReleaseDate() const;
        bool    isHidden() const {return hidden;};
        bool    isDownloaded() const {return downloaded;};

        bool    getInformation();
        void    draw(ifont* font);
        void    isClicked(int x, int y, ifont* font);

    private:
        irect   rect;
        irect   downloadButton;
        irect   readButton;
        irect   removeButton;

        string  title;
        string  contentUrl;
        tm      releaseDate;
        string  downloadUrl;
        string  path;
        string  content;
        bool    hidden; 
        bool    downloaded;

        bool    download();
        void    eliminate();
        void    open();

};
#endif
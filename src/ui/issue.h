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
        Issue(istringstream& str_strm);

        bool operator== (const Issue &iss) const;
        
        void    setRect(irect Rect);
        irect*  getRect(){return &rect;};
        string  getTitle() const {return title;};
        string  getContentUrl() const {return contentUrl;};
        string  getDownloadUrl() const {return downloadUrl;};
        string  getPath() const {return path;};
        string  getContent() const {return content;};
        bool    isHidden() const {return hidden;};
        bool    isDownloaded() const {return downloaded;};

        void    draw(ifont* font);
        void    isClicked(int x, int y, ifont* font);

    private:
        irect   rect;
        irect   downloadButton;
        irect   readButton;
        irect   removeButton;

        string  title;
        string  contentUrl;
        string  downloadUrl;
        string  path;
        string  content;
        bool    hidden; 
        bool    downloaded;

};
#endif
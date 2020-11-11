//------------------------------------------------------------------
// item.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      Describes an WEBDAV item
//-------------------------------------------------------------------

#ifndef ITEM
#define ITEM

#include "inkview.h"

#include <string>
#include <iostream>

using std::istringstream;
using std::ostream;
using std::string;

enum FileState
{
    ICLOUD,
    ISYNCED
};

class Item
{
public:
    //Item(const string &Title, const string &ContentUrl,  const tm &releaseDate);
    Item(const string &Title, const string &ContentUrl);

    Item(istringstream &str_strm);
    Item(const string &_localPath);

    friend ostream &operator<<(ostream &out, Item const &iss);
    bool operator==(const Item &iss) const;
    bool operator>(const Item &iss) const;

    //string getReleaseDateString() const;

    bool getInformation();
    bool download();

    string getDownloadPath() const { return _downloadPath; };

    string getLocalPath() const { return _localPath; };

    string getTitle() const { return _title; };

    void setState(FileState state) { _state = state; };
    FileState getState() const { return _state; };

    void open() const;
    bool removeFile();

private:
    string _title;
    string _contentUrl;
    string _downloadPath;
    //tm _releaseDate; //TODO include default
    FileState _state{FileState::ICLOUD};
    string _localPath{""};
};
#endif
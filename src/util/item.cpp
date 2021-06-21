//------------------------------------------------------------------
// item.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "item.h"
#include "inkview.h"
#include "util.h"
#include "dieZeit.h"
#include "log.h"

#include <iostream>
#include <string>
#include <sstream>
#include <curl/curl.h>
#include <algorithm>

using namespace std;

//Item::Item(const string &title, const string &contentUrl, const tm &releaseDate): _title(title), _contentUrl(contentUrl), _releaseDate(releaseDate)
Item::Item(const string &title, const string &contentUrl) : _title(title), _contentUrl(contentUrl)
{
    string tmp = title;
    replace(tmp.begin(), tmp.end(), '/', '_');
    _localPath = DIEZEIT_ISSUE_PATH + tmp + ".epub";
}

Item::Item(istringstream &str_strm)
{
    string tmp;

    getline(str_strm, tmp, ',');
    _title = tmp;

    getline(str_strm, tmp, ',');
    _contentUrl = tmp;

    //getline(str_strm, tmp, ',');
    //sscanf(tmp.c_str(), "%2d.%2d.%4d", &_releaseDate.tm_mday, &_releaseDate.tm_mon, &_releaseDate.tm_year);
    //_releaseDate.tm_mon = _releaseDate.tm_mon - 1;
    //_releaseDate.tm_year = _releaseDate.tm_year - 1900;

    getline(str_strm, tmp, ',');
    _downloadPath = tmp;

    getline(str_strm, tmp, ',');

    if (iv_access(tmp.c_str(), R_OK) == 0)
    {
        _localPath = tmp;
        _state = FileState::ISYNCED;
    }
    else
    {
        tmp = _title;
        replace(tmp.begin(), tmp.end(), '/', '_');
        _localPath = DIEZEIT_ISSUE_PATH + tmp + ".epub";
    }
}

Item::Item(const string &localPath) : _localPath(localPath)
{
    _title = _localPath.substr(DIEZEIT_ISSUE_PATH.size(), (_localPath.size() - DIEZEIT_ISSUE_PATH.size() - 5));
    replace(_title.begin(), _title.end(), '_', '/');

    _state = FileState::ISYNCED;
}

bool Item::operator>(const Item &iss) const
{
    //here as old versions dont have this information in csv.
    //DIE ZEIT 48/2020

    int releaseYear1 = 0;
    int releaseEdition1 = 0;
    int releaseYear2 = 0;
    int releaseEdition2 = 0;

    if (iss.getTitle().empty())
        return true;

    std::size_t foundAt;

    string title1 = iss.getTitle();

    foundAt = title1.find("/");

    if (foundAt > 2)
    {
        releaseYear1 = std::stoi(title1.substr(foundAt + 1));
        releaseEdition1 = std::stoi(title1.substr(foundAt - 2, foundAt));
    }

    if (_title.empty())
        return false;

    string title2 = _title;
    foundAt = title2.find("/");

    if (foundAt > 2)
    {
        releaseYear2 = std::stoi(title2.substr(foundAt + 1));
        releaseEdition2 = std::stoi(title2.substr(foundAt - 2, foundAt));
    }

    if (releaseYear2 > releaseYear1)
        return true;

    if (releaseYear2 == releaseYear1 && releaseEdition2 > releaseEdition1)
        return true;
    return false;
}

bool Item::operator==(const Item &iss) const
{
    if (_title == iss._title)
        return true;
    return false;
}

ostream &operator<<(ostream &out, Item const &iss)
{
    out << iss._title << ',' << iss._contentUrl << ','
        << iss._downloadPath << ',' << iss._localPath << endl;
    return out;
}

/*
string Item::getReleaseDateString() const
{
    char buffer[80];
    strftime(buffer, 80, "%d.%m.%Y", &_releaseDate);
    return buffer;
}
*/

bool Item::getInformation()
{
    //return if download link is in new format
    if (!_downloadPath.empty())
    {
        size_t found = _downloadPath.find("https://medien.zeit.de/restricted/");

        if (found != std::string::npos)
            return true;
    }

    if (!Util::connectToNetwork())
        return false;

    Log::writeLog(_contentUrl);

    std::string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, _contentUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Util::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        //curl_easy_setopt(curl, CURLOPT_COOKIESESSION, true);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, DIEZEIT_COOOKIE_PATH.c_str());
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {

            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            switch (response_code)
            {
            case 200:
            {
                // view-source:https://epaper.zeit.de/abo/diezeit/20.05.2020
                //   <a class="btn btn-default btn-block btn-border btn-md" href="https://premium.zeit.de/system/files/2020-21/epub/die_zeit_2020_22.epub" data-wt-click="{ct: '#download_zeit+magazin', ck: {4: 'undefined', 5: 'content', 6: 'reader', 9: 'epub'}}">
                //      EPUB FÜR E-READER LADEN    </a>

                //    <a class="btn btn-default btn-block btn-border btn-md" href="https://medien.zeit.de/restricted/4fb5c38b-61eb-4378-af6d-02233bff6494.epub" data-wt-click="{ct: '#download_zeit+magazin', ck: {4: 'undefined', 5: 'content', 6: 'reader', 9: 'epub'}}">
                // EPUB FÜR E-READER LADEN    </a>

                size_t found = readBuffer.find("https://medien.zeit.de/restricted/");

                if (found != std::string::npos)
                {
                    readBuffer = readBuffer.substr(found, 200);
                    found = readBuffer.find('"');
                    _downloadPath = readBuffer.substr(0, found);
                    Log::writeLog(_downloadPath);
                    return true;
                }
                else
                {
                    Message(ICON_ERROR, "Fehler", "Fehler beim Parsen der DownloadURL.", 1200);
                    return false;
                }

                break;
            }
            default:
                Message(ICON_ERROR, "Error", ("An unknown error occured. (Curl Response Code " + std::to_string(response_code) + ")").c_str(), 2000);
                break;
            }
        }
        else
        {
            Message(ICON_ERROR, "Fehler", curl_easy_strerror(res), 1200);
        }
    }
    return false;
}

bool Item::download()
{
    if (_downloadPath.empty())
    {
        return false;
    }

    UpdateProgressbar("Versuche eine Verbindung mit dem Internet herzustellen.", 0);

    if (!Util::connectToNetwork())
    {
        return false;
    }

    FILE *fp;
    CURLcode res;

    CURL *curl = curl_easy_init();

    Log::writeLog(_downloadPath);

    if (curl)
    {
        fp = iv_fopen(_localPath.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, _downloadPath.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        //curl_easy_setopt(curl, CURLOPT_COOKIESESSION, true);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, DIEZEIT_COOOKIE_PATH.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Util::writeData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Util::progress_callback);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        iv_fclose(fp);

        if (res == CURLE_OK)
        {

            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            if (response_code == 200)
            {
                _state = FileState::ISYNCED;
                return true;
            }

            else
                Message(ICON_ERROR, "Error", ("An unknown error occured. (Curl Response Code " + std::to_string(response_code) + ")").c_str(), 2000);

            return false;
        }
    }
    return false;
}

void Item::open() const
{
    if (iv_access(_localPath.c_str(), R_OK) == 0)
        OpenBook(_localPath.c_str(), "", 0);
}

bool Item::removeFile()
{
    if (remove(_localPath.c_str()) != 0)
        return false;

    _state = FileState::ICLOUD;

    Util::updatePBLibrary(5);
    return true;
}

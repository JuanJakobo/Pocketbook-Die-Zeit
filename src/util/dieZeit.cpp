//------------------------------------------------------------------
// dieZeit.cpp
//
// Author:           JuanJakobo
// Date:             14.06.2020
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "dieZeit.h"
#include "util.h"
#include "item.h"
#include "log.h"

#include <string>
#include <curl/curl.h>
#include <fstream>
#include <algorithm>

using std::ifstream;
using std::ofstream;
using std::string;

DieZeit::DieZeit()
{
    if (iv_access(DIEZEIT_PATH.c_str(), W_OK) != 0)
        iv_mkdir(DIEZEIT_PATH.c_str(), 0777);

    if (iv_access(DIEZEIT_ISSUE_PATH.c_str(), W_OK) != 0)
        iv_mkdir(DIEZEIT_ISSUE_PATH.c_str(), 0777);
}

void DieZeit::setUsername(const string &Username)
{
    iconfigedit *temp = nullptr;
    iconfig *dieZeitConfig = OpenConfig(DIEZEIT_CONFIG_PATH.c_str(), temp);
    WriteString(dieZeitConfig, "username", Username.c_str());
    CloseConfig(dieZeitConfig);
}

void DieZeit::setPassword(const string &Pass)
{
    iconfigedit *temp = nullptr;
    iconfig *dieZeitConfig = OpenConfig(DIEZEIT_CONFIG_PATH.c_str(), temp);
    WriteSecret(dieZeitConfig, "password", Pass.c_str());
    CloseConfig(dieZeitConfig);
}

bool DieZeit::login()
{
    return login(this->getUsername(), this->getPassword());
}

bool DieZeit::login(const string &Username, const string &Pass)
{
    UpdateProgressbar("Versuche Verbindung mit dem Internet herzustellen.", 10);

    if (!Util::connectToNetwork())
    {
        Message(ICON_WARNING, "Warnung", "Es konnte keine Verbindung zum Internet hergestellt werden.", 200);
        return false;
    }

    if (Username.empty() || Pass.empty())
    {
        Message(ICON_ERROR, "Fehler", "Benutzername und/oder Password nicht gefunden.", 1200);
        return false;
    }

    Log::writeLog("login startet.");

    UpdateProgressbar("Baue eine Verbindung mit Zeit.de auf.", 20);

    string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();

    if (curl)
    {
        string post = "email=" + Username + "&pass=" + Pass;

        curl_easy_setopt(curl, CURLOPT_URL, DIEZEIT_LOGIN_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Util::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_COOKIESESSION, true);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, DIEZEIT_COOOKIE_PATH.c_str());
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, DIEZEIT_COOOKIE_PATH.c_str());

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {
            //check if login was successful
            std::size_t found;
            found = readBuffer.find("notification__header--error");
            if (found != std::string::npos)
            {
                Message(ICON_ERROR, "Fehler", "Kombination aus Benutzernamen und Passwort ist nicht korrekt.", 1200);
                return false;
            }

            this->setUsername(Username);
            this->setPassword(Pass);
            _loggedIn = true;
            UpdateProgressbar("Lade Ausgabeninformationen herunter.", 50);

            if (!getCurrentIssues(readBuffer))
                return false;
            if (!getIssuesInformation())
                return false;
            getLocalFiles();
            updateActualizationDate();
            UpdateProgressbar("Speichere Ausgabeninformationen lokal ab.", 99);
            saveIssuesToFile();
            return true;
        }
    }
    return false;
}

void DieZeit::logout(bool deleteFiles)
{
    if (deleteFiles)
    {
        UpdateProgressbar("Die Ausgaben werden entfernt.", 50);
        string cmd = "rm -rf " + DIEZEIT_ISSUE_PATH + "/";
        system(cmd.c_str());
        Util::updatePBLibrary(5);
    }
    //https://meine.zeit.de/abmelden?url=https%3A//premium.zeit.de/
    remove(DIEZEIT_CONFIG_PATH.c_str());
    remove((DIEZEIT_CONFIG_PATH + ".back.").c_str());
    remove(DIEZEIT_COOOKIE_PATH.c_str());
    remove(DIEZEIT_CSV_PATH.c_str());

    _items.reset();
    _loggedIn = false;
}

bool DieZeit::saveIssuesToFile()
{
    if (_items->size() > 0)
    {
        ofstream outFile(DIEZEIT_CSV_PATH.c_str());

        if (outFile)
        {
            for (size_t i = 0; i < _items->size(); i++)
            {
                outFile << _items->at(i);
            }
            return true;
        }
    }
    return false;
}

bool DieZeit::getIssuesFromFile()
{

    _items = std::make_shared<vector<Item>>(vector<Item>());

    if (_items)
        _items->clear();

    if (iv_access(DIEZEIT_CSV_PATH.c_str(), R_OK) != 0)
        return false;

    ifstream inFile(DIEZEIT_CSV_PATH.c_str());

    auto line = std::string{};

    while (getline(inFile, line))
    {
        istringstream str_strm(line);
        _items->push_back(Item(str_strm));
    }

    if (_items->size() < 1)
        return false;

    _loggedIn = true;
    getLocalFiles();

    return true;
}

string DieZeit::getUsername()
{
    iconfigedit *temp = nullptr;
    iconfig *dieZeitConfig = OpenConfig(DIEZEIT_CONFIG_PATH.c_str(), temp);
    string user = ReadString(dieZeitConfig, "username", "");
    CloseConfigNoSave(dieZeitConfig);
    return user;
}

string DieZeit::getPassword()
{
    iconfigedit *temp = nullptr;
    iconfig *dieZeitConfig = OpenConfig(DIEZEIT_CONFIG_PATH.c_str(), temp);
    string pass = ReadSecret(dieZeitConfig, "password", "");
    CloseConfigNoSave(dieZeitConfig);
    return pass;
}

bool DieZeit::getCurrentIssues(string &htmlpage)
{
    Log::writeLog("get current isseues");
    std::size_t found;
    //tm releaseDate;
    string contentUrl;
    string title;

    if (!_items)
        _items = std::make_shared<vector<Item>>(vector<Item>());

    //epaper-cover
    //gets the last 7 issues
    do
    {
        found = std::string::npos;
        found = htmlpage.find("epaper-cover");

        if (found != std::string::npos)
        {
            htmlpage = htmlpage.substr(found);

            //href="/abo/diezeit/20.05.2020"
            found = htmlpage.find("href=\"");
            htmlpage = htmlpage.substr(found + 6);
            contentUrl = "https://epaper.zeit.de" + htmlpage.substr(0, htmlpage.find("\""));

            //TODO add image
            //src="https://mobile.s4p-iapps.com/pdf/published/company/67/pdfplace/613/pages/926711/pdf-926711-page-0-cover-big.jpg"

            //alt="DIE ZEIT 22/2020"
            found = htmlpage.find("alt=\"");
            htmlpage = htmlpage.substr(found + 5);
            title = htmlpage.substr(0, htmlpage.find("\""));

            //found = htmlpage.find("release-date\">");
            //htmlpage = htmlpage.substr(found + 14);
            //string releasedateString = htmlpage.substr(0, htmlpage.find("<"));

            //sscanf(releasedateString.c_str(), "%2d.%2d.%4d", &releaseDate.tm_mday, &releaseDate.tm_mon, &releaseDate.tm_year);
            //releaseDate.tm_mon = releaseDate.tm_mon - 1;
            //releaseDate.tm_year = releaseDate.tm_year - 1900;

            //Item temp = Item(title, contentUrl, releaseDate);
            Item temp = Item(title, contentUrl);
            if (find(_items->begin(), _items->end(), temp) == _items->end())
                _items->push_back(temp);
        }
    } while (found != std::string::npos);

    sort(_items->begin(), _items->end(), std::greater<Item>());

    return true;
}

bool DieZeit::getIssuesInformation()
{
    int progress = 50;
    for (auto i = 0; i < _items->size(); i++)
    {
        progress = progress + i;
        UpdateProgressbar("Lade die Ausgabeninformationen herunter.", progress);

        if (_items->at(i).getState() == FileState::ICLOUD)
        {
            Log::writeLog("getting udpate" + _items->at(i).getTitle());
            if (!_items->at(i).getInformation())
                return false;
        }
    }
    UpdateProgressbar("Herunterladen der Ausgabeninformationen abgeschlossen.", 95);

    return true;
}

void DieZeit::getLocalFiles()
{
    //get local files, https://stackoverflow.com/questions/306533/how-do-i-get-a-list-of-files-in-a-directory-in-c
    DIR *dir;
    class dirent *ent;
    class stat st;

    dir = opendir(DIEZEIT_ISSUE_PATH.c_str());
    while ((ent = readdir(dir)) != NULL)
    {
        const string file_name = ent->d_name;
        const string full_file_name = DIEZEIT_ISSUE_PATH + file_name;

        if (file_name[0] == '.')
            continue;

        if (stat(full_file_name.c_str(), &st) == -1)
            continue;

        const bool is_directory = (st.st_mode & S_IFDIR) != 0;
        if (is_directory)
            continue;

        bool found = false;
        for (auto i = 0; i < _items->size(); i++)
        {
            if (_items->at(i).getLocalPath().compare(full_file_name) == 0)
            {
                _items->at(i).setState(FileState::ISYNCED);
                found = true;
                break;
            }
        }
        if (!found)
            _items->push_back(Item(full_file_name));
    }
    closedir(dir);
}

void DieZeit::updateActualizationDate()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d/%m/%g %H:%M", timeinfo);
    std::string str(buffer);
    Util::writeStringSetting("LastActualisation", str);
}
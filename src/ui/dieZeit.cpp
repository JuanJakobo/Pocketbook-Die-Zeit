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
#include "issue.h"

#include <string>
#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

DieZeit::DieZeit(irect* ContentRect): contentRect(ContentRect)
{
    loggedIn = false;

    dieZeitFont = OpenFont("LiberationMono",DIEZEIT_FONT_SIZE,1);

    if(iv_access(DIEZEIT_PATH.c_str(), W_OK)!=0)
        iv_mkdir(DIEZEIT_PATH.c_str(),0777);

    if(iv_access(DIEZEIT_ISSUE_PATH.c_str(),W_OK)!=0)
        iv_mkdir(DIEZEIT_ISSUE_PATH.c_str(),0777);
}

DieZeit::~DieZeit()
{
    CloseFont(dieZeitFont);
}

void DieZeit::setUsername(const string& Username)
{
    iconfigedit* temp = nullptr;
    iconfig  *dieZeitConfig = OpenConfig(DIEZEIT_CONFIG_PATH.c_str(),temp);
    WriteString(dieZeitConfig,"username",Username.c_str());
    CloseConfig(dieZeitConfig);
}

void DieZeit::setPassword(const string& Pass)
{
    iconfigedit* temp = nullptr;
    iconfig  *dieZeitConfig = OpenConfig(DIEZEIT_CONFIG_PATH.c_str(),temp);
    WriteSecret(dieZeitConfig,"password",Pass.c_str());
    CloseConfig(dieZeitConfig);
}

bool DieZeit::login()
{
    return login(this->getUsername(),this->getPassword());     
}

bool DieZeit::login(const string& Username, const string& Pass)
{
    if(!Util::connectToNetwork())
        return false;

    if(Username.empty() || Pass.empty())
        return false;
    
    std::string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();

    if(curl)
    {
        string post = "email=" + Username + "&pass=" + Pass;

        curl_easy_setopt(curl, CURLOPT_URL, DIEZEIT_LOGIN_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
        curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Util::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_COOKIESESSION, true);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, DIEZEIT_COOOKIE_PATH.c_str());  
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, DIEZEIT_COOOKIE_PATH.c_str());

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);


        if(res == CURLE_OK)
        {
            //check if login was successful
            std::size_t found;
            found = readBuffer.find("notification__header--error");
            if(found!=std::string::npos)
            {
                Message(ICON_ERROR, "Error", "Failed to login", 600);
                return false;
            }
            this->setUsername(Username);
            this->setPassword(Pass);
            loggedIn = true;
            return getCurrentIssues(readBuffer);
        }
    }
    return false;
}

bool DieZeit::logout()
{
    //https://meine.zeit.de/abmelden?url=https%3A//premium.zeit.de/
    remove(DIEZEIT_CONFIG_PATH.c_str());
    remove(DIEZEIT_COOOKIE_PATH.c_str());
    Dialog(2,"Result","test","Delete issues.","Keep issues.",DieZeit::DialogHandlerStatic);
    issues.clear();
    loggedIn = false;

    return true;
}

bool DieZeit::getCurrentIssues(string htmlpage)
{
    std::size_t found;
    tm releaseDate;
    string contentUrl;
    string title;

    //epaper-cover
    //gets the last 7 issues
    do
    {
        found = std::string::npos;
        found = htmlpage.find("epaper-cover");

        if (found!=std::string::npos)
        {
            htmlpage = htmlpage.substr(found);

            //href="/abo/diezeit/20.05.2020"  
            found = htmlpage.find("href=\""); 
            htmlpage = htmlpage.substr(found+6); 
            contentUrl = "https://epaper.zeit.de" + htmlpage.substr(0, htmlpage.find("\""));

            //TODO add image
            //src="https://mobile.s4p-iapps.com/pdf/published/company/67/pdfplace/613/pages/926711/pdf-926711-page-0-cover-big.jpg" 

            //alt="DIE ZEIT 22/2020" 
            found = htmlpage.find("alt=\"");
            htmlpage = htmlpage.substr(found+5);
            title = htmlpage.substr(0, htmlpage.find("\""));

            found = htmlpage.find("release-date\">");
            htmlpage = htmlpage.substr(found+14);
            string releasedateString = htmlpage.substr(0, htmlpage.find("<"));

	        sscanf(releasedateString.c_str(),"%2d.%2d.%4d",&releaseDate.tm_mday,&releaseDate.tm_mon,&releaseDate.tm_year);
            releaseDate.tm_mon = releaseDate.tm_mon -1;
            releaseDate.tm_year = releaseDate.tm_year -1900;

            Issue temp = Issue(title,contentUrl,releaseDate);

            if(find(issues.begin(), issues.end(), temp) == issues.end())
                this->issues.push_back(temp);
        }
    } while (found!=std::string::npos);

    //sort issues 
    sort(issues.begin(),issues.end(),greater<Issue>());
    
    return true;
}

void DieZeit::drawIssuesScreen()
{
        int entrySize = DIEZEIT_FONT_SIZE*3;
        
        for(unsigned int i = 0; i < issues.size(); i++)
        {
            irect rect = iRect(0,i*entrySize+contentRect->y,ScreenWidth(),entrySize,0);
            issues[i].setRect(rect);
            issues[i].draw(dieZeitFont);
        }
}

int DieZeit::issueClicked(int x, int y)
{
    for(unsigned int i = 0; i < issues.size(); i++)
    {
        if(IsInRect(x,y,issues[i].getRect())==1)
        {
            issues[i].isClicked(x,y,dieZeitFont);
            return 1;
        }
    }
    return 0;
}

void DieZeit::getIssuesInformation()
{

    //only for issues which dont have 
    for(auto& i : issues) 
    {
        i.getInformation();
    }
}

bool DieZeit::saveIssuesToFile()
{
    ofstream outFile(DIEZEIT_CSV_PATH.c_str());

    if(outFile)
    {
        for (const auto &iss : issues)
        {
            outFile << iss;       
        }
        return true;
    }
    return false;    
}

bool DieZeit::getIssuesFromFile()
{
    if(iv_access(DIEZEIT_CSV_PATH.c_str(), R_OK)!=0)
        return false;

    ifstream inFile(DIEZEIT_CSV_PATH.c_str());

    auto line = std::string{};

    while(getline(inFile,line))
    {
        istringstream str_strm(line);
        issues.push_back(Issue(str_strm));
    }

    if(issues.size()<1)
        return false;

    loggedIn = true;

    return true;

}

string DieZeit::getUsername()
{
    iconfigedit* temp = nullptr;
    iconfig  *dieZeitConfig = OpenConfig(DIEZEIT_CONFIG_PATH.c_str(),temp);
    string user = ReadString(dieZeitConfig,"username","");
    CloseConfigNoSave(dieZeitConfig);
    return user;
}

string DieZeit::getPassword()
{
    iconfigedit* temp = nullptr;
    iconfig  *dieZeitConfig = OpenConfig(DIEZEIT_CONFIG_PATH.c_str(),temp);
    string pass = ReadSecret(dieZeitConfig,"password","");
    CloseConfigNoSave(dieZeitConfig);
    return pass;
}

void DieZeit::DialogHandlerStatic(int Button)
{
    if(Button==1)
    {
        remove(DIEZEIT_CSV_PATH.c_str());
        rmdir(DIEZEIT_ISSUE_PATH.c_str());
    }

}
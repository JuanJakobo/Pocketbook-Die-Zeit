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

bool DieZeit::login()
{
    if(!Util::connectToNetwork())
        return false;
    
    std::string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();

    if(curl)
    {
        string post = "email=" + this->getUsername() + "&pass=" + this->getPassword();

        curl_easy_setopt(curl, CURLOPT_URL, DIEZEIT_LOGIN_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
        curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Util::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_COOKIESESSION, true);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, DIEZEIT_COOOKIE_PATH.c_str());  
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, DIEZEIT_COOOKIE_PATH.c_str());
        //curl_easy_setopt(curl, CURLOPT_SSL_CIPHER_LIST,"DEFAULT@SECLEVEL=1");

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);


        if(res == CURLE_OK)
        {
            this->renameCookie();
            loggedIn = true;
            return getCurrentIssues(readBuffer);
        }
    }
    return false;
}

void DieZeit::renameCookie()
{
    ifstream t(DIEZEIT_COOOKIE_PATH);
    string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

    string from = "zeit_sso_session_201501";
    string to = "zeit_sso_201501";

    size_t start_pos = str.find(from);

    if(start_pos != string::npos)
        str.replace(start_pos,from.length(),to);

    ofstream out(DIEZEIT_COOOKIE_PATH);
    out << str;
    out.close();

}

bool DieZeit::getCurrentIssues(string htmlpage)
{
    std::size_t found;

    auto i = 0;
    string contentUrl;
    string title;

    //check if login was successful
    found = htmlpage.find("notification__header--error");
    if(found!=std::string::npos)
        return false;

    //epaper-cover
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

            Issue temp = Issue(title,contentUrl);

            if(find(issues.begin(), issues.end(), temp) == issues.end())
                this->issues.push_back(temp);
        }
        i++;

    } while (found!=std::string::npos);

    return true;
}

void DieZeit::drawIssuesScreen()
{
        int entrySize = DIEZEIT_FONT_SIZE*3;
        SetFont(dieZeitFont, BLACK);
        
        for(unsigned int i = 0; i < issues.size(); i++)
        {
            irect rect = iRect(0,i*entrySize+contentRect->y,ScreenWidth(),entrySize,0);
            issues[i].setRect(&rect);
            issues[i].draw();
        }
}

bool DieZeit::saveIssuesToFile()
{
    if(iv_access(DIEZEIT_CSV_PATH.c_str(), R_OK)!=0)
        return false;

    ofstream outFile(DIEZEIT_CSV_PATH.c_str());

    if(outFile)
    {
        for (const auto &iss : issues)
        {
            outFile << iss.getTitle() << CSV_DELIM << iss.getContentUrl() << CSV_DELIM << iss.getDownloadUrl() << CSV_DELIM << iss.getPath() 
                    << CSV_DELIM << iss.getContent() << CSV_DELIM << iss.isHidden() << CSV_DELIM << iss.isDownloaded() << endl; 
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
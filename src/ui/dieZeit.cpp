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

DieZeit * DieZeit::dieZeitStatic;

DieZeit::DieZeit(irect* ContentRect): contentRect(ContentRect)
{
    dieZeitStatic = this;

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

void DieZeit::setUsername(const string& Username)
{
    iconfigedit* temp = nullptr;
    iconfig  *dieZeitConfig = OpenConfig(DIEZEIT_CONFIG_PATH.c_str(),temp);
    WriteString(dieZeitConfig,"username",Username.c_str());
    CloseConfig(dieZeitConfig);
}

string DieZeit::getPassword()
{
    iconfigedit* temp = nullptr;
    iconfig  *dieZeitConfig = OpenConfig(DIEZEIT_CONFIG_PATH.c_str(),temp);
    string pass = ReadSecret(dieZeitConfig,"password","");
    CloseConfigNoSave(dieZeitConfig);
    return pass;
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

bool DieZeit::logout()
{
    //TODO what to do with existing books? 
    //dialog "want to remove books"

    //TODO
    //https://meine.zeit.de/abmelden?url=https%3A//premium.zeit.de/
    //delete Config
    //return false;
    remove(DIEZEIT_CONFIG_PATH.c_str());
    remove(DIEZEIT_COOOKIE_PATH.c_str());
    remove(DIEZEIT_CSV_PATH.c_str());
    issues.clear();
    loggedIn = false;

    return true;
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
        
        for(unsigned int i = 0; i < issues.size(); i++)
        {
            irect rect = iRect(0,i*entrySize+contentRect->y,ScreenWidth(),entrySize,0);
            issues[i].setRect(rect);
            DrawTextRect2(&rect,"abc");
            issues[i].draw(dieZeitFont);
        }
}

void DieZeit::drawLoginScreen()
{

    usernameButton = iRect(50,200,ScreenWidth()-50,75,ALIGN_CENTER);
    DrawLine(20,275,(ScreenWidth()-20),275,BLACK);
    //DrawBorder(&usernameButton, 5, 1, 1, BLACK);
    //void DrawBorder(const irect *border_rect, int border_size, int style, int radius, int color);
    //DrawPickOutEx(&usernameButton, "test");
    //FillAreaRect(&usernameButton, BLACK);
    SetFont(dieZeitFont,BLACK);
    DrawTextRect2(&usernameButton,"Username");

    passwordButton = iRect(50,400,(ScreenWidth()-50),75,ALIGN_CENTER);
    DrawLine(20,475,(ScreenWidth()-20),475,BLACK);
    //FillArea(21,301,(ScreenWidth()-20),75, BLACK);
    //FillAreaRect(&passwordButton, WHITE);
    SetFont(dieZeitFont,BLACK);
    DrawTextRect2(&passwordButton,"Password");

    loginButton = iRect(ScreenWidth()/2,contentRect->h/2,200,50,ALIGN_CENTER);
    FillAreaRect(&loginButton, BLACK);
    SetFont(dieZeitFont,WHITE);
    DrawTextRect2(&loginButton,"Login"); 
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

//TODO WHERE TO MOVE TO??
void DieZeit::keyboardHandlerStatic(char *text) 
{   
    dieZeitStatic->keyboardHandler(text);
}


void DieZeit::keyboardHandler(char *text)
{
    if(!text)
        return;

    std::string s(text);
    if (s.empty())
        return;

    if(test==1)
    {
        //TODO only save after successful login??
        this->setUsername(s.c_str());
        DrawTextRect2(&usernameButton,s.c_str());
    }
    else
    {
        this->setPassword(s.c_str());
        DrawTextRect2(&passwordButton,s.c_str());

    }    

    //m_cmd_buffer = null after setting it

}

int DieZeit::logginClicked(int x, int y)
{
    //TODO anpassen
    #define MAX_CMD_LEN      256
    char m_cmd_buffer[4 * MAX_CMD_LEN + 1];

    //TODO Move all to event handler?
    //TODO if config not existent --> create  // add  warning
    
    if(IsInRect(x,y,&usernameButton))
    {
        test = 1;
        OpenKeyboard("Username", m_cmd_buffer, MAX_CMD_LEN-1, KBD_NORMAL, &keyboardHandlerStatic);
        return 1;
    }
    else if(IsInRect(x,y,&passwordButton))
    {
        test = 2;
        OpenKeyboard("Password", m_cmd_buffer, MAX_CMD_LEN-1, KBD_NORMAL, &keyboardHandlerStatic);

        return 1;
    }
    else if(IsInRect(x,y,&loginButton))
    {
        login();
        getIssuesInformation();
        //UpdateProgressbar("Done",90);
        FillAreaRect(contentRect,WHITE);
        drawIssuesScreen();
        saveIssuesToFile();
        //TODO missing draw etc; where to place it?
        FullUpdate();
        return 1;
    }

    return 0;
}

void DieZeit::getIssuesInformation()
{
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
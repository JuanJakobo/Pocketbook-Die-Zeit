//------------------------------------------------------------------
// dieZeit.h
//  
// Author:           JuanJakobo          
// Date:             14.06.2020
// Description:      Handles login and download of german newspaper "Die Zeit"
//
//-------------------------------------------------------------------

#ifndef DIEZEIT
#define DIEZEIT

#include "issue.h"

#include <string>

const string DIEZEIT_PATH = "/mnt/ext1/system/config/dieZeit";
const string DIEZEIT_COOOKIE_PATH = DIEZEIT_PATH + "/dieZeit.cookie";
const string DIEZEIT_CONFIG_PATH = DIEZEIT_PATH + "/dieZeit.cfg";
const string DIEZEIT_CSV_PATH = DIEZEIT_PATH + "/dieZeit.csv";
const string CSV_DELIM = ","; 
const string DIEZEIT_ISSUE_PATH = "/mnt/ext1/dieZeit/";
const string DIEZEIT_LOGIN_URL = "https://meine.zeit.de/anmelden?url=https%3A//premium.zeit.de/aktion/das-digitale-abo-der-zeit&entry_service=premium";
const int    DIEZEIT_FONT_SIZE = 40;

class DieZeit {
    public:

        DieZeit(irect* ContentRect);

        ~DieZeit();

        string getUsername();
        string getPassword();
        
        bool isLoggedIn(){return loggedIn;};

        bool login();
        void renameCookie();
        bool getCurrentIssues(string htmlpage);
        void drawIssuesScreen();

        int issueClicked(int x,int y);
        void getIssuesInformation();

        bool saveIssuesToFile();
        bool getIssuesFromFile();

    private:

        ifont* dieZeitFont;  
        vector<Issue>   issues;
        irect*  contentRect;
        bool loggedIn;  
};



#endif
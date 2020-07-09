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
//extern const ifont*    dieZeitFont

class DieZeit {
    public:

        DieZeit(irect* ContentRect);

        ~DieZeit();

        //TODO PRIVATE??
        string getUsername();
        void   setUsername(const string& Username);

        string getPassword();
        void   setPassword(const string& Pass);

        void keyboardHandler(char *text);
        
        bool isLoggedIn(){return loggedIn;};

        bool login();
        bool logout();
        void renameCookie();
        bool getCurrentIssues(string htmlpage);
        void drawIssuesScreen();
        void drawLoginScreen();

        int issueClicked(int x,int y);
        int logginClicked(int x, int y);
        void getIssuesInformation();

        bool saveIssuesToFile();
        bool getIssuesFromFile();

    private:

        static DieZeit *dieZeitStatic;
        int test;

        ifont* dieZeitFont;  
        vector<Issue>   issues;
        irect*  contentRect;
        irect   loginButton;
        irect   usernameButton;
        irect   passwordButton;
        bool    loggedIn;  

        /**
        * Functions needed to call C function, handles the panel
        * 
        * @return void
        */
        static void keyboardHandlerStatic(char *text); 
};



#endif
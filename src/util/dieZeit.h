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

#include "inkview.h"
#include "item.h"

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;

const string DIEZEIT_PATH = "/mnt/ext1/system/config/dieZeit";
const string DIEZEIT_COOOKIE_PATH = DIEZEIT_PATH + "/dieZeit.cookie";
const string DIEZEIT_CONFIG_PATH = DIEZEIT_PATH + "/dieZeit.cfg";
const string DIEZEIT_CSV_PATH = DIEZEIT_PATH + "/dieZeit.csv";
const string DIEZEIT_ISSUE_PATH = "/mnt/ext1/dieZeit/";
const string DIEZEIT_LOGIN_URL = "https://meine.zeit.de/anmelden?url=https%3A//premium.zeit.de/aktion/das-digitale-abo-der-zeit&entry_service=premium";
const int DIEZEIT_FONT_SIZE = 40;

class DieZeit
{
public:
    explicit DieZeit();

    bool login();
    bool login(const string &Username, const string &Pass);

    void logout(bool deleteFiles = false);

    bool saveIssuesToFile();
    bool getIssuesFromFile();

    std::shared_ptr<vector<Item>> getItems() const { return _items; };
    
    bool isLoggedIn() const { return _loggedIn; };
    void setUsername(const string &Username);
    void setPassword(const string &Pass);

private:
    std::shared_ptr<vector<Item>> _items = nullptr;
    bool _loggedIn{false};

    string getUsername();
    string getPassword();

    bool getCurrentIssues(string &htmlpage);
    bool getIssuesInformation();
    void getLocalFiles();
    void updateActualizationDate();
};

#endif
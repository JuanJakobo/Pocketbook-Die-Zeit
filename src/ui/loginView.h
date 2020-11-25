//------------------------------------------------------------------
// loginView.h
//
// Author:           JuanJakobo
// Date:             26.08.2020
// Description:
//-------------------------------------------------------------------

#ifndef LOGIN_SCREEN
#define LOGIN_SCREEN

#include "inkview.h"

#include <string>
#include <memory>

using std::string;

enum KeyboardTarget 
{
    IUSERNAME,
    IPASSWORD
};

const int KEYBOARD_STRING_LENGTH = 40;

class LoginView
{
public:
    LoginView(const irect *contentRect);

    int logginClicked(int x, int y);

    string getUsername() { return _username; };
    string getPassword() { return _password; };
    
private:
    static LoginView *_loginViewStatic;
    std::unique_ptr<ifont> _loginFont;
    const irect *_contentRect;
    irect _loginButton;
    irect _usernameButton;
    irect _passwordButton;
    KeyboardTarget _target;
    string _username;
    string _password;
    string _temp;

    /**
        * Functions needed to call C function, handles the panel
        * 
        * @return void
        */
    static void keyboardHandlerStatic(char *text);

    void keyboardHandler(char *text);
};

#endif
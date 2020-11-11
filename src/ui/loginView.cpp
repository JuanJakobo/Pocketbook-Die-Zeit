//------------------------------------------------------------------
// loginView.cpp
//
// Author:           JuanJakobo
// Date:             26.08.2020
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "loginView.h"
#include "eventHandler.h"

#include <string>
#include <memory>

using std::string;

LoginView *LoginView::_loginViewStatic;

LoginView::LoginView(const irect *contentRect) : _contentRect(contentRect)
{
    _loginViewStatic = this;
    _loginFont = std::unique_ptr<ifont>(OpenFont("LiberationMono", 40, 1));

    SetFont(_loginFont.get(), BLACK);
    FillAreaRect(_contentRect, WHITE);
    
    _usernameButton = iRect(50, 400, ScreenWidth() - 100, 75, ALIGN_CENTER);
    DrawTextRect(_usernameButton.x, _usernameButton.y - 50, _usernameButton.w, _usernameButton.h, "Benutzername:", ALIGN_LEFT);
    DrawRect(_usernameButton.x - 1, _usernameButton.y - 1, _usernameButton.w + 2, _usernameButton.h + 2, BLACK);

    _passwordButton = iRect(50, 600, (ScreenWidth() - 100), 75, ALIGN_CENTER);
    DrawTextRect(_passwordButton.x, _passwordButton.y - 50, _passwordButton.w, _passwordButton.h, "Passwort:", ALIGN_LEFT);
    DrawRect(_passwordButton.x - 1, _passwordButton.y - 1, _passwordButton.w + 2, _passwordButton.h + 2, BLACK);

    _loginButton = iRect(50, 750, (ScreenWidth() - 100), 75, ALIGN_CENTER);

    FillAreaRect(&_loginButton, BLACK);
    SetFont(_loginFont.get(), WHITE);
    DrawTextRect2(&_loginButton, "Login");
}
int LoginView::logginClicked(int x, int y)
{
    _charBuffer = (char *)malloc(_bufferSize);

    if (IsInRect(x, y, &_usernameButton))
    {
        _keyboardValue = 1;
        OpenKeyboard("Benutzername", _charBuffer, MAX_CHAR_BUFF_LENGHT - 1, KBD_NORMAL, &keyboardHandlerStatic);
        return 1;
    }
    else if (IsInRect(x, y, &_passwordButton))
    {
        _keyboardValue = 2;
        OpenKeyboard("Passwort", _charBuffer, MAX_CHAR_BUFF_LENGHT - 1, KBD_PASSWORD, &keyboardHandlerStatic);

        return 1;
    }
    else if (IsInRect(x, y, &_loginButton))
    {
        if (_username.empty() || _password.empty())
        {
            Message(ICON_ERROR, "Fehler", "Bitte tragen Sie Benutzernamen und Passwort ein.", 1200);
            return 1;
        }

        return 2;
    }

    return 0;
}

void LoginView::keyboardHandlerStatic(char *text)
{
    _loginViewStatic->keyboardHandler(text);
}

void LoginView::keyboardHandler(char *text)
{
    if (!text)
        return;

    string s(text);
    if (s.empty())
        return;

    else if (_keyboardValue == 1)
    {
        _username = s.c_str();
        FillAreaRect(&_usernameButton, WHITE);
        DrawTextRect2(&_usernameButton, s.c_str());
    }
    else
    {
        _password = s.c_str();
        FillAreaRect(&_passwordButton, WHITE);

        string pass;
        for (auto i = 0; i < s.length(); i++)
        {
            pass += "*";
        }

        DrawTextRect2(&_passwordButton, pass.c_str());
    }

    free(_charBuffer);
}
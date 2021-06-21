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

using std::string;

LoginView *LoginView::_loginViewStatic;

LoginView::LoginView(const irect *contentRect) : _contentRect(contentRect)
{
    _loginViewStatic = this;
    int contentHeight = contentRect->h / 2;
    int contentWidth = _contentRect->w * 0.9;

    int beginY = 0.4 * contentHeight;
    int beginX = (_contentRect->w - contentWidth) / 2;

    int contents = contentHeight / 7;

    _loginFontHeight = contents / 2;

    _loginFont = OpenFont("LiberationMono", _loginFontHeight, FONT_STD);
    SetFont(_loginFont, BLACK);
    FillAreaRect(_contentRect, WHITE);
    
    _usernameButton = iRect(beginX, beginY + 2 * contents, contentWidth, contents, ALIGN_CENTER);
    DrawTextRect(_usernameButton.x, _usernameButton.y - _loginFontHeight - _loginFontHeight/3, _usernameButton.w, _usernameButton.h, "Username:", ALIGN_LEFT);
    DrawRect(_usernameButton.x - 1, _usernameButton.y - 1, _usernameButton.w + 2, _usernameButton.h + 2, BLACK);

    _passwordButton = iRect(beginX, beginY + 4 * contents, contentWidth, contents, ALIGN_CENTER);
    DrawTextRect(_passwordButton.x, _passwordButton.y - _loginFontHeight - _loginFontHeight/4, _passwordButton.w, _passwordButton.h, "Password:", ALIGN_LEFT);
    DrawRect(_passwordButton.x - 1, _passwordButton.y - 1, _passwordButton.w + 2, _passwordButton.h + 2, BLACK);

    _loginButton = iRect(beginX, beginY + 6 * contents, contentWidth, contents, ALIGN_CENTER);

    FillAreaRect(&_loginButton, BLACK);
    SetFont(_loginFont, WHITE);
    DrawTextRect2(&_loginButton, "Login");
}

LoginView::~LoginView()
{
    CloseFont(_loginFont);
}

int LoginView::logginClicked(int x, int y)
{
    _temp = "";

    if (IsInRect(x, y, &_usernameButton))
    {
        _target = KeyboardTarget::IUSERNAME;
        if (!_username.empty())
            _temp = _username;
        _temp.resize(KEYBOARD_STRING_LENGTH);
        OpenKeyboard("Benutzername", &_temp[0],KEYBOARD_STRING_LENGTH, KBD_NORMAL, &keyboardHandlerStatic);
        return 1;
    }
    else if (IsInRect(x, y, &_passwordButton))
    {
        _target = KeyboardTarget::IPASSWORD;
        _temp.resize(KEYBOARD_STRING_LENGTH);
        OpenKeyboard("Passwort", &_temp[0], KEYBOARD_STRING_LENGTH, KBD_PASSWORD, &keyboardHandlerStatic);
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
    {
        return;
    }
    else if (_target == KeyboardTarget::IUSERNAME)
    {
        _username = s.c_str();
        FillAreaRect(&_usernameButton, WHITE);
        DrawTextRect2(&_usernameButton, s.c_str());
    }
    else if (_target == KeyboardTarget::IPASSWORD)
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
    else
    {
        Message(ICON_ERROR,"Fehler","Fehler bei der Eingabe. Bitte versuchen Sie es erneut.",1200);
    }
    
}
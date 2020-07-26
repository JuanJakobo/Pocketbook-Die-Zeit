//------------------------------------------------------------------
// loginScreenHandler.cpp
//
// Author:           JuanJakobo          
// Date:             26.07.2020
//   
//-------------------------------------------------------------------

#include "inkview.h"
#include "loginScreenHandler.h"
#include "eventHandler.h"

#include <string>

LoginScreenHandler * LoginScreenHandler::loginScreenHandlerStatic;

LoginScreenHandler::LoginScreenHandler()
{
    loginScreenHandlerStatic = this;
    loginFont = OpenFont("LiberationMono",40,1);

}

LoginScreenHandler::~LoginScreenHandler()
{
    CloseFont(loginFont);
}

void LoginScreenHandler::drawLoginScreen()
{

    usernameButton = iRect(50,200,ScreenWidth()-50,75,ALIGN_CENTER);
    DrawLine(20,275,(ScreenWidth()-20),275,BLACK);
    SetFont(loginFont,BLACK);
    DrawTextRect2(&usernameButton,"Username");

    passwordButton = iRect(50,400,(ScreenWidth()-50),75,ALIGN_CENTER);
    DrawLine(20,475,(ScreenWidth()-20),475,BLACK);
    SetFont(loginFont,BLACK);
    DrawTextRect2(&passwordButton,"Password");

    //loginButton = iRect(ScreenWidth()/2,contentRect->h/2,200,50,ALIGN_CENTER);
    loginButton = iRect(ScreenWidth()/2,500,200,50,ALIGN_CENTER);

    FillAreaRect(&loginButton, BLACK);
    SetFont(loginFont,WHITE);
    DrawTextRect2(&loginButton,"Login"); 
}

void LoginScreenHandler::keyboardHandlerStatic(char *text) 
{   
    loginScreenHandlerStatic->keyboardHandler(text);
}


void LoginScreenHandler::keyboardHandler(char *text)
{
    if(!text)
        return;

    std::string s(text);
    if (s.empty())
        return;

    if(test==1)
    {
        username = s.c_str();
        FillAreaRect(&usernameButton,WHITE);
        DrawTextRect2(&usernameButton,s.c_str());
    }
    else
    {
        password = s.c_str();
        FillAreaRect(&passwordButton,WHITE);
        DrawTextRect2(&passwordButton,s.c_str());

    }    

    //m_cmd_buffer = null after setting it

}


int LoginScreenHandler::logginClicked(int x, int y)
{
    //TODO anpassen
    #define MAX_CMD_LEN      256
    char m_cmd_buffer[4 * MAX_CMD_LEN + 1];

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
        if(username.empty() || password.empty())
        {
            Message(ICON_ERROR, "Error", "Please set username and password.", 600);
            return 1;
        }

        return 2;
    }

    return 0;
}
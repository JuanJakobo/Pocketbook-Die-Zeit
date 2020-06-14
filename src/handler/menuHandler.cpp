//------------------------------------------------------------------
// menuHandler.cpp
//
// Author:           JuanJakobo          
// Date:             14.06.2020
//   
//-------------------------------------------------------------------

#include "inkview.h"
#include "menuHandler.h"

#include <string>

MenuHandler::MenuHandler(const std::string &name)
{


    //Define panel size
    panelMenuHeight = ScreenHeight() / 18;
    panelMenuBeginY = 0;
    mainMenuWidth = ScreenWidth()/6;
    panelMenuBeginX = ScreenWidth() - mainMenuWidth;

    menuButtonRect = iRect(panelMenuBeginX,panelMenuBeginY,mainMenuWidth,panelMenuHeight,0);

    //set menu font
    menuFont = OpenFont("LiberationMono-Bold",panelMenuHeight/2,1);
    SetFont(menuFont, BLACK);

    DrawTextRect(0,panelMenuBeginY,ScreenWidth(),panelMenuHeight,name.c_str(),ALIGN_CENTER);
    DrawTextRect2(&menuButtonRect,"Menu");
    DrawLine(0,panelMenuHeight,ScreenWidth(),panelMenuHeight,BLACK);

    contentRect = iRect(0,panelMenuHeight,ScreenWidth(),(ScreenHeight() - PanelHeight() - panelMenuHeight),0);

    SetHardTimer("PANELUPDATE", panelHandlerStatic, 110000);
	DrawPanel(NULL, "", NULL, -1);
}

MenuHandler::~MenuHandler()
{
    CloseFont(menuFont);
}

irect* MenuHandler::getContentRect()
{
    return &contentRect;
}

irect* MenuHandler::getMenuButtonRect()
{
    return &menuButtonRect;
}

void MenuHandler::panelHandlerStatic() 
{
    DrawPanel(NULL, "", NULL, -1);
    SetHardTimer("PANELUPDATE", panelHandlerStatic, 110000);
}

void MenuHandler::createMenu(bool loggedIn, iv_menuhandler handler)
{
    imenu mainMenu[]=
    {
        { ITEM_HEADER, 0, "Menu", NULL},
        //show logged out
        { loggedIn ? ITEM_HIDDEN : ITEM_ACTIVE, 101, "Login", NULL}, 
        //show logged in 
        { loggedIn ? ITEM_ACTIVE : ITEM_HIDDEN, 102, "Actualize", NULL}, 
        { loggedIn ? ITEM_ACTIVE : ITEM_HIDDEN, 103, "Logout", NULL}, 
        //show always
        { ITEM_ACTIVE, 104, "Settings", NULL},
        { ITEM_ACTIVE, 105, "Exit"},
        { 0, 0, NULL, NULL}
    };

    if(loggedIn)
    {
        mainMenu[3].type=ITEM_ACTIVE;
    }
    else
    {
        mainMenu[5].type=ITEM_ACTIVE;
    }   

    OpenMenu(mainMenu,0,panelMenuBeginX,panelMenuBeginY,handler);
}
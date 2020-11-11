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

using namespace std;

MenuHandler::MenuHandler(const string &name)
{
    //Define panel size
    _panelMenuHeight = ScreenHeight() / 18;
    _panelMenuBeginY = 0;
    _mainMenuWidth = ScreenWidth() / 3;
    _panelMenuBeginX = ScreenWidth() - _mainMenuWidth;

    _menuButtonRect = iRect(_mainMenuWidth * 2, _panelMenuBeginY, _mainMenuWidth, _panelMenuHeight, ALIGN_RIGHT);
    _menuFont = std::unique_ptr<ifont>(OpenFont("LiberationMono-Bold", _panelMenuHeight / 2, 1));

    SetFont(_menuFont.get(), BLACK);
    DrawTextRect(0, _panelMenuBeginY, ScreenWidth(), _panelMenuHeight, name.c_str(), ALIGN_CENTER);
    DrawTextRect2(&_menuButtonRect, "Menü");
    DrawLine(0, _panelMenuHeight - 1, ScreenWidth(), _panelMenuHeight - 1, BLACK);

    _contentRect = iRect(0, _panelMenuHeight, ScreenWidth(), (ScreenHeight() - PanelHeight() - _panelMenuHeight), 0);

    _loadingScreenRect = iRect(_contentRect.w / 2 - 125, _contentRect.h / 2 - 50, 250, 100, ALIGN_CENTER);
    _loadingFont = std::unique_ptr<ifont>(OpenFont("LiberationMono", _loadingScreenRect.h / 4, 1));

    SetHardTimer("PANELUPDATE", panelHandlerStatic, 110000);
    DrawPanel(NULL, "", NULL, -1);
}

void MenuHandler::panelHandlerStatic() 
{
    DrawPanel(NULL, "", NULL, -1);
    SetHardTimer("PANELUPDATE", panelHandlerStatic, 110000);
}


int MenuHandler::createMenu(bool loggedIn, iv_menuhandler handler)
{
    imenu mainMenu[]=
    {
        { ITEM_HEADER, 0, "Menü", NULL},
        //show logged in 
        { loggedIn ? ITEM_ACTIVE : ITEM_HIDDEN, 102, "Aktualiseren", NULL}, 
        { loggedIn ? ITEM_ACTIVE : ITEM_HIDDEN, 103, "Logout", NULL}, 
        //show always
        { ITEM_ACTIVE, 104, "Schließen"},
        { 0, 0, NULL, NULL}
    };

    if(loggedIn)
    {
        mainMenu[3].type=ITEM_ACTIVE;
    }
    else
    {
        mainMenu[1].type=ITEM_ACTIVE;
    }   

    OpenMenu(mainMenu, 0, _panelMenuBeginX, _panelMenuBeginY, handler);
    
    return 1;
    
}
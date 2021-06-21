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
    _menuFont = OpenFont("LiberationMono-Bold", _panelMenuHeight / 2, 1);

    SetFont(_menuFont, BLACK);
    DrawTextRect(0, _panelMenuBeginY, ScreenWidth(), _panelMenuHeight, name.c_str(), ALIGN_CENTER);
    DrawTextRect2(&_menuButtonRect, "Menü");
    DrawLine(0, _panelMenuHeight - 1, ScreenWidth(), _panelMenuHeight - 1, BLACK);

    _contentRect = iRect(0, _panelMenuHeight, ScreenWidth(), (ScreenHeight() - PanelHeight() - _panelMenuHeight), 0);

    SetHardTimer("PANELUPDATE", panelHandlerStatic, 110000);
    DrawPanel(NULL, "", NULL, -1);
}

MenuHandler::~MenuHandler()
{
    CloseFont(_menuFont);
    free(_menu);
    free(_actualize);
    free(_logout);
    free(_exit);
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
        { ITEM_HEADER, 0, _menu, NULL},
        //show logged in 
        { loggedIn ? (short)ITEM_ACTIVE : (short)ITEM_HIDDEN, 102, _actualize, NULL}, 
        { loggedIn ? (short)ITEM_ACTIVE : (short)ITEM_HIDDEN, 103, _logout, NULL}, 
        //show always
        { ITEM_ACTIVE, 104, _exit},
        { 0, 0, NULL, NULL}
    };

    OpenMenu(mainMenu, 0, _panelMenuBeginX, _panelMenuBeginY, handler);
    
    return 1;
    
}
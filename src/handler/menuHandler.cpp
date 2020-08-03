//------------------------------------------------------------------
// menuHandler.cpp
//
// Author:           JuanJakobo          
// Date:             14.06.2020
//   
//-------------------------------------------------------------------

#include "inkview.h"
#include "menuHandler.h"
#include "util.h"

#include <string>
#include <ctime>

using namespace std;

MenuHandler::MenuHandler(const string &name)
{
    //Define panel size
    panelMenuHeight = ScreenHeight() / 18;
    panelMenuBeginY = 0;
    mainMenuWidth = ScreenWidth()/3;
    panelMenuBeginX = ScreenWidth() - mainMenuWidth;

    menuButtonRect = iRect(mainMenuWidth*2,panelMenuBeginY,mainMenuWidth,panelMenuHeight,ALIGN_RIGHT);


    //last actualisation date
    accFont = OpenFont("LiberationMono-Bold",panelMenuHeight/3,0);
    SetFont(accFont,BLACK);
    DrawTextRect(0,panelMenuBeginY,mainMenuWidth,panelMenuHeight,"Last Actualisation:",ALIGN_LEFT);
    DrawTextRect(0,panelMenuHeight/2,mainMenuWidth,panelMenuHeight,Util::readStringSetting("LastActualisation").c_str(),ALIGN_LEFT);

    menuFont = OpenFont("LiberationMono-Bold",panelMenuHeight/2,1);
    SetFont(menuFont, BLACK);

    DrawTextRect(0,panelMenuBeginY,ScreenWidth(),panelMenuHeight,name.c_str(),ALIGN_CENTER);
    DrawTextRect2(&menuButtonRect,"Menu");
    DrawLine(0,panelMenuHeight-1,ScreenWidth(),panelMenuHeight-1,BLACK);

    contentRect = iRect(0,panelMenuHeight,ScreenWidth(),(ScreenHeight() - PanelHeight() - panelMenuHeight),0);

    SetHardTimer("PANELUPDATE", panelHandlerStatic, 110000);
	DrawPanel(NULL, "", NULL, -1);
}

MenuHandler::~MenuHandler()
{
    CloseFont(menuFont);
    CloseFont(accFont);
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
        { ITEM_HEADER, 0, "Menu", NULL},
        //show logged in 
        { loggedIn ? ITEM_ACTIVE : ITEM_HIDDEN, 102, "Actualize", NULL}, 
        //show always
        { ITEM_ACTIVE, 103, "Settings", NULL},
        { loggedIn ? ITEM_ACTIVE : ITEM_HIDDEN, 104, "Logout", NULL}, 
        //show always
        { ITEM_ACTIVE, 105, "Exit"},
        { 0, 0, NULL, NULL}
    };

    if(loggedIn)
    {
        mainMenu[2].type=ITEM_ACTIVE;
    }
    else
    {
        mainMenu[4].type=ITEM_ACTIVE;
    }   

    OpenMenu(mainMenu,0,panelMenuBeginX,panelMenuBeginY,handler);
    
    return 1;
    
}

void MenuHandler::updateActualizationDate()
{
    SetFont(accFont, BLACK);

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d/%m/%g %H:%M",timeinfo);
    std::string str(buffer);

    FillArea(0,panelMenuHeight/2, mainMenuWidth, (panelMenuHeight/3)-1,WHITE);
    DrawTextRect(0,panelMenuHeight/2,mainMenuWidth,(panelMenuHeight/3),str.c_str(),ALIGN_LEFT);

    Util::writeStringSetting("LastActualisation",str);

}
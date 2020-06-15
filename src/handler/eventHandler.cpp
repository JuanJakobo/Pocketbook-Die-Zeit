//------------------------------------------------------------------
// eventHandler.cpp
//
// Author:           JuanJakobo          
// Date:             14.06.2020
//   
//-------------------------------------------------------------------

#include "inkview.h"
#include "eventHandler.h"
#include "menuHandler.h"
#include "dieZeit.h"

#include <string>

EventHandler * EventHandler::eventHandlerStatic;

EventHandler::EventHandler()
{
    //create a event to create handlers
    eventHandlerStatic = this;

    menu = new MenuHandler("Die Zeit");

    zeit = new DieZeit(menu->getContentRect());

    if(zeit->getIssuesFromFile())
    {
        zeit->drawIssuesScreen();
    }
    else
    {
        if(zeit->login())
        {
            zeit->getIssuesInformation();
            zeit->drawIssuesScreen();
            zeit->saveIssuesToFile();
        }
        else
        {
            Message(ICON_ERROR, "Error", "Failed to login", 600);
        }
    }
    
    FullUpdate();
}

EventHandler::~EventHandler()
{
    delete menu;
    delete zeit;
}

int EventHandler::eventDistributor(int type, int par1, int par2)
{
    if (ISPOINTEREVENT(type))
	    return EventHandler::pointerHandler(type,par1,par2);

    return 0;
}

void EventHandler::mainMenuHandlerStatic(int index)
{
    eventHandlerStatic->mainMenuHandler(index);
}

void EventHandler::mainMenuHandler(int index)
{
    switch(index)
 	{
        //Login
        case 101:
            break; 	 
        //Actualize
        case 102:
            break; 
        //Logout	 
        case 103:
            break; 	
        //Settings
        case 104:
            break; 
        //Exit	 
        case 105:
            CloseApp();
            break;
        default:
            break;
 	}
}

int EventHandler::pointerHandler(int type, int par1, int par2)
{
    if(type==EVT_POINTERDOWN)
    {
        if(IsInRect(par1,par2,menu->getMenuButtonRect())==1)
        {
            return menu->createMenu(zeit->isLoggedIn(),EventHandler::mainMenuHandlerStatic);
        }
        else if(IsInRect(par1,par2,menu->getContentRect())==1)
        {
            return zeit->issueClicked(par1,par2);
        }
        else
        {
            DrawTextRect(par1,par2,100,100,"*",ALIGN_CENTER);
            PartialUpdate(par1,par2,100,100);
            return 1;
        }
    }

    return 0;
}
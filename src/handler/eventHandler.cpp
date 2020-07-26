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
#include "loginScreenHandler.h"
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
        loginScreen = new LoginScreenHandler();

        loginScreen->drawLoginScreen();
    }
    
    FullUpdate();
}

EventHandler::~EventHandler()
{
    //TODO test ifworks
    zeit->saveIssuesToFile();

    delete menu;
    delete zeit;
}

int EventHandler::eventDistributor(int type, int par1, int par2)
{
    if (ISPOINTEREVENT(type))
	    return EventHandler::pointerHandler(type,par1,par2);

    return 0;
}

void EventHandler::DialogHandlerStatic(int Button)
{
    //TODO stop download? howto?
    CloseProgressbar();
}

void EventHandler::mainMenuHandlerStatic(int index)
{
    eventHandlerStatic->mainMenuHandler(index);
}


void EventHandler::loginAndDraw()
{
    if(iv_access(DIEZEIT_CONFIG_PATH.c_str(), R_OK)==0)
    {
        OpenProgressbar(1,"Login to Zeit","Loggin in",10,DialogHandlerStatic);

        if(zeit->isLoggedIn())
        {
            if(zeit->login())
            {
                CloseProgressbar();
                Message(ICON_ERROR, "Error", "Failed to login", 600);
                return;
            }
        }
        else
        {
            if(!zeit->login(loginScreen->getUsername(),loginScreen->getPassword()))
            {
                CloseProgressbar();
                Message(ICON_ERROR, "Error", "Failed to login", 600);
                return; 
            }
            delete loginScreen;
            zeit->getIssuesFromFile();
        }
        
        UpdateProgressbar("Getting current issues",70);
        zeit->getIssuesInformation();
        UpdateProgressbar("Done",90);
        FillAreaRect(menu->getContentRect(),WHITE);
        zeit->drawIssuesScreen();
        zeit->saveIssuesToFile();
        CloseProgressbar();
        PartialUpdate(1,menu->getContentRect()->y,menu->getContentRect()->w,menu->getContentRect()->h);
    }
    else
    {
        Message(ICON_ERROR, "Error", "Cannot access config file.", 600);
    }
    
}

void EventHandler::mainMenuHandler(int index)
{
    switch(index)
 	{
        //Actualize
        case 102:
            loginAndDraw();    
            break; 
        //Logout	 
        case 103:
            zeit->logout();
            FillAreaRect(menu->getContentRect(),WHITE);
            loginScreen->drawLoginScreen();
            PartialUpdate(1,menu->getContentRect()->y,menu->getContentRect()->w,menu->getContentRect()->h);
            break; 	
        //Settings
        case 104:
            break; 
        //Exit	 
        case 105:
            zeit->saveIssuesToFile();
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
            if(zeit->isLoggedIn())
            {
                return zeit->issueClicked(par1,par2);
            }
            else
            {
                if(loginScreen->logginClicked(par1,par2)==2)
                    loginAndDraw();
                
                return 1;
            }
            
        }
    }

    return 0;
}
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

        zeit->drawLoginScreen();
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

void EventHandler::mainMenuHandler(int index)
{
    switch(index)
 	{
        //Login
        case 101:
            //TODO clear content area and than draw login screen
            if(iv_access(DIEZEIT_CONFIG_PATH.c_str(), R_OK)==0)
            {
                //TODO OpenProgressbar(1,"Login to Zeit","Loggin in",10,DialogHandlerStatic);

                if(zeit->login())
                {        
                    //UpdateProgressbar("Getting current issues",70);
                    zeit->getIssuesInformation();
                    //UpdateProgressbar("Done",90);
                    FillAreaRect(menu->getContentRect(),WHITE);
                    zeit->drawIssuesScreen();
                    zeit->saveIssuesToFile();
                }
                else
                {
                    Message(ICON_ERROR, "Error", "Failed to login", 600);
                }
                //CloseProgressbar();
                PartialUpdate(1,menu->getContentRect()->y,menu->getContentRect()->w,menu->getContentRect()->h);

            }
            break; 	 
        //Actualize
        case 102:
            //Todo in method 
                OpenProgressbar(1,"Login to Zeit","Loggin in",10,DialogHandlerStatic);

                if(zeit->login())
                {        
                    UpdateProgressbar("Getting current issues",70);
                    zeit->getIssuesInformation();
                    UpdateProgressbar("Done",90);
                    //FillAreaRect(menu->getContentRect(),WHITE);
                    zeit->drawIssuesScreen();
                    zeit->saveIssuesToFile();
                    CloseProgressbar();

                }
                else
                {
                    CloseProgressbar();
                    Message(ICON_ERROR, "Error", "Failed to login", 600);
                }
                PartialUpdate(1,menu->getContentRect()->y,menu->getContentRect()->w,menu->getContentRect()->h);     
            break; 
        //Logout	 
        case 103:
            zeit->logout();
            FillAreaRect(menu->getContentRect(),WHITE);
            zeit->drawLoginScreen();
            PartialUpdate(1,menu->getContentRect()->y,menu->getContentRect()->w,menu->getContentRect()->h);
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
            if(zeit->isLoggedIn())
            {
                return zeit->issueClicked(par1,par2);
            }
            else
            {
                return zeit->logginClicked(par1,par2);
            }
            
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
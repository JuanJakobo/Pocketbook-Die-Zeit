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

    //test for login data

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

    OpenProgressbar(1,"Login to Zeit","Loggin in",10,DialogHandlerStatic);

    if(zeit->isLoggedIn())
    {
        if(!zeit->login())
        {
            CloseProgressbar();
            return;
        }
    }
    else
    {
        if(!zeit->login(loginScreen->getUsername(),loginScreen->getPassword()))
        {
            CloseProgressbar();
            //Message(ICON_ERROR, "Error", "Failed to login", 600);
            //and then the user needs the possiblity to login again...
            return; 
        }
        delete loginScreen;
    }
        
    UpdateProgressbar("Getting current issues",70);
    zeit->getIssuesInformation();
    UpdateProgressbar("Done",90);
    zeit->saveIssuesToFile();
    CloseProgressbar();

    FillAreaRect(menu->getContentRect(),WHITE);
    zeit->drawIssuesScreen();
    menu->updateActualizationDate();    
    FullUpdate();
}

void EventHandler::mainMenuHandler(int index)
{
    switch(index)
 	{
        //Actualize
        case 102:
            loginAndDraw();
            break; 
        //Settings	 
        case 103:

            break; 	
        //Logout
        case 104:
            zeit->logout(EventHandler::logoutDialogHandlerStatic);

            break; 
        //Exit	 
        case 105:
            if(zeit->isLoggedIn())
                zeit->saveIssuesToFile();
            //CloseApp();
            break;
        default:
            break;
 	}
}

void EventHandler::logoutDialogHandlerStatic(int button)
{
    eventHandlerStatic->logoutDialogHandler(button);
}

void EventHandler::logoutDialogHandler(int button)
{
    if(button==1)
    {
        remove(DIEZEIT_CSV_PATH.c_str());
        rmdir(DIEZEIT_ISSUE_PATH.c_str());
    } 

    FillAreaRect(menu->getContentRect(),WHITE);
    loginScreen = new LoginScreenHandler();
    loginScreen->drawLoginScreen();
    FullUpdate();
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
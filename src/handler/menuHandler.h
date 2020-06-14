//------------------------------------------------------------------
// eventHandler.h
//
// Author:           JuanJakobo          
// Date:             14.06.2020
// Description:      Handles the menubar and the menu   
//-------------------------------------------------------------------

#ifndef MENU_HANDLER
#define MENU_HANDLER

#include <string>

class MenuHandler {
    public:
        /**
        * Defines fonds, sets global Event Handler and starts new content 
        */
        MenuHandler(const std::string &name);

        /**
        * Destructor 
        */
        ~MenuHandler();
        
        irect*   getContentRect(){return &contentRect;};
        irect*   getMenuButtonRect(){return &menuButtonRect;};

        int    createMenu(bool loggedIn, iv_menuhandler handler);

    private:

        ifont*  menuFont;

        int     panelMenuBeginX;
        int     panelMenuBeginY;
        int     panelMenuHeight;

        int     mainMenuWidth;
        irect   menuButtonRect;

        imenu   mainMenu;

        irect   contentRect;

        /**
        * Functions needed to call C function, handles the panel
        * 
        * @return void
        */
        static void panelHandlerStatic(); 

};

#endif
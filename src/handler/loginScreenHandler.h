//------------------------------------------------------------------
// loginScreenHandler.h
//
// Author:           JuanJakobo          
// Date:             26.07.2020
// Description:        
//-------------------------------------------------------------------

#ifndef LOGIN_SCREEN_HANDLER
#define LOGIN_SCREEN_HANDLER

#include "inkview.h"

#include <string>

using namespace std;

const int MAX_CHAR_BUFF_LENGHT = 256;

class LoginScreenHandler 
{
    public:  

        LoginScreenHandler();
        ~LoginScreenHandler();
        
        void drawLoginScreen(); 
        int logginClicked(int x, int y);   

        string getUsername(){return username;};
        string getPassword(){return password;}; 
         
    private:

        static LoginScreenHandler *loginScreenHandlerStatic;
        ifont*  loginFont;  
        irect   loginButton;
        irect   usernameButton;
        irect   passwordButton;
        int     test;
        string  username;
        string  password;

        char*   charBuffer;

        /**
        * Functions needed to call C function, handles the panel
        * 
        * @return void
        */
        static void keyboardHandlerStatic(char *text);         
        
        void keyboardHandler(char *text);



};

#endif
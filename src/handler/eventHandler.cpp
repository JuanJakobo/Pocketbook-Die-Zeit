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
#include "util.h"
#include "log.h"

std::unique_ptr<EventHandler> EventHandler::_eventHandlerStatic;

EventHandler::EventHandler()
{
    //create a event to create handlers
    _eventHandlerStatic = std::unique_ptr<EventHandler>(this);

    _listView = nullptr;
    _loginView = nullptr;

    if (_dieZeit.getIssuesFromFile())
    {
        _listView = std::unique_ptr<ListView>(new ListView(_menu.getContentRect(), _dieZeit.getItems()));
    }
    else if ((iv_access(DIEZEIT_CONFIG_PATH.c_str(), W_OK) == 0))
    {
        if (_dieZeit.login())
            _listView = std::unique_ptr<ListView>(new ListView(_menu.getContentRect(), _dieZeit.getItems()));
    }
    else
    {
        _dieZeit.logout();
        _loginView = std::unique_ptr<LoginView>(new LoginView(_menu.getContentRect()));
    }

    FullUpdate();
}

int EventHandler::eventDistributor(int type, int par1, int par2)
{
    if (ISPOINTEREVENT(type))
        return EventHandler::pointerHandler(type, par1, par2);

    return 0;
}

void EventHandler::DialogHandlerStatic(const int clicked)
{
    //TODO cannot interact with it
    CloseProgressbar();
}

void EventHandler::mainMenuHandlerStatic(int index)
{
    _eventHandlerStatic->mainMenuHandler(index);
}

void EventHandler::mainMenuHandler(int index)
{
    switch (index)
    {
    //Actualize
    case 102:
        OpenProgressbar(ICON_INFORMATION, "Aktualiserung", "Start der Aktualisierung", 0, EventHandler::DialogHandlerStatic);
        if (_dieZeit.login())
        {
            FillAreaRect(_menu.getContentRect(), WHITE);
            _listView.reset(new ListView(_menu.getContentRect(), _dieZeit.getItems()));
            FullUpdate();
        }
        CloseProgressbar();
        break;
    //Logout
    case 103:
    {
        int dialogResult = DialogSynchro(ICON_QUESTION, "Frage", "Möchten Sie die lokal gespeicherten Ausgaben löschen?", "Ja", "Nein", "Abbrechen");
        switch (dialogResult)
        {
        case 1:
            _dieZeit.logout(true);
            break;
        case 3:
            return;
        default:
            _dieZeit.logout();
            break;
        }
        _listView.reset();
        _loginView = std::unique_ptr<LoginView>(new LoginView(_menu.getContentRect()));
        FullUpdate();
        break;
    }
    //Exit
    case 104:
        CloseApp();
        break;
    default:
        break;
    }
}

int EventHandler::pointerHandler(int type, int par1, int par2)
{
    if (type == EVT_POINTERDOWN)
    {
        if (IsInRect(par1, par2, _menu.getMenuButtonRect()) == 1)
        {
            return _menu.createMenu(_dieZeit.isLoggedIn(), EventHandler::mainMenuHandlerStatic);
        }
        else if (_listView != nullptr)
        {
            int itemID = _listView->listClicked(par1, par2);
            if (itemID != -1)
            {
                if (_dieZeit.getItems()->at(itemID).getState() == FileState::ICLOUD)
                {
                    OpenProgressbar(1, "Herunterladen der Ausgabe", "Beginnen des Herunterladens", 0, EventHandler::DialogHandlerStatic);
                    if (!_dieZeit.getItems()->at(itemID).download())
                    {
                        Message(ICON_WARNING, "Warnung", "Die Ausgabe konnte nicht heruntergeladen werden. Bitte versuchen Sie es erneut.", 1200);
                    }
                    else
                    {
                        Util::updatePBLibrary(5);
                        Log::writeLog("updating PB lib");
                    }

                    CloseProgressbar();
                }
                else
                {
                    int dialogResult = 3;

                    dialogResult = DialogSynchro(ICON_QUESTION, "Frage", "Was möchten Sie tun?", "Ausgabe lesen", "Ausgabe löschen", "Abbrechen");

                    switch (dialogResult)
                    {
                    case 1:
                        _dieZeit.getItems()->at(itemID).open();
                        break;
                    case 2:
                        OpenProgressbar(ICON_INFORMATION, "Information", "Die Ausgabe wird entfernt.", 0, EventHandler::DialogHandlerStatic);
                        if (!_dieZeit.getItems()->at(itemID).removeFile())
                            Message(ICON_WARNING, "Warnung", "Die Ausgabe konnte nicht gelöscht werden. Bitte versuchen Sie es erneut.", 1200);
                        if (_dieZeit.getItems()->at(itemID).getDownloadPath().empty())
                        {
                            _dieZeit.getItems()->erase(_dieZeit.getItems()->begin() + itemID);
                            _listView.reset(new ListView(_menu.getContentRect(), _dieZeit.getItems()));
                            _dieZeit.saveIssuesToFile();
                            PartialUpdate(_menu.getContentRect()->x, _menu.getContentRect()->y, _menu.getContentRect()->w, _menu.getContentRect()->h);
                            return 1;
                        }
                        CloseProgressbar();
                        break;
                    default:
                        return 1;
                    }
                }
                _listView->drawEntry(itemID);
            }

            PartialUpdate(_menu.getContentRect()->x, _menu.getContentRect()->y, _menu.getContentRect()->w, _menu.getContentRect()->h);

            return 1;
        }
        else if (_loginView != nullptr)
        {
            if (_loginView->logginClicked(par1, par2) == 2)
            {
                OpenProgressbar(ICON_INFORMATION, "Login", "Start der Logins", 0, EventHandler::DialogHandlerStatic);

                if (_dieZeit.login(_loginView->getUsername(), _loginView->getPassword()))
                {
                    CloseProgressbar();
                    _listView = std::unique_ptr<ListView>(new ListView(_menu.getContentRect(), _dieZeit.getItems()));
                    _loginView.reset();
                    PartialUpdate(_menu.getContentRect()->x, _menu.getContentRect()->y, _menu.getContentRect()->w, _menu.getContentRect()->h);
                    return 1;
                }
                CloseProgressbar();

                return 1;
            }
            return 1;
        }
    }
    return 0;
}
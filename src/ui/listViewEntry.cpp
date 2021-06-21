//------------------------------------------------------------------
// listViewEntry.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "item.h"
#include "listViewEntry.h"
#include "util.h"

#include <memory>

ListViewEntry::ListViewEntry(int page, irect rect) : _page(page), _position(rect)
{
}

void ListViewEntry::draw(const Item &item, ifont *entryFont, ifont *entryFontBold, int fontHeight)
{
    SetFont(entryFontBold, BLACK);
    DrawTextRect(_position.x, _position.y, _position.w, fontHeight, item.getTitle().c_str(), ALIGN_LEFT);

    SetFont(entryFont, BLACK);

    switch (item.getState())
    {
    case FileState::ICLOUD:
        DrawTextRect(_position.x, _position.y + 3 * fontHeight, _position.w, fontHeight, "Klicken um Ausgabe herunterzuladen", ALIGN_RIGHT);
        break;
    default:
        DrawTextRect(_position.x, _position.y + 3 * fontHeight, _position.w, fontHeight, "Ausgabe ist heruntergeladen", ALIGN_RIGHT);
        break;
    }
    int line = (_position.y + _position.h) - 1;
    DrawLine(0, line, ScreenWidth(), line, BLACK);
}

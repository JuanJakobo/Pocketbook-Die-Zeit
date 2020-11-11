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

void ListViewEntry::draw(const Item &item)
{
    SetFont(_entryFontBold.get(), BLACK);
    DrawTextRect(_position.x, _position.y, _position.w, _fontHeight, item.getTitle().c_str(), ALIGN_LEFT);

    SetFont(_entryFont.get(), BLACK);

    switch (item.getState())
    {
    case FileState::ICLOUD:
        DrawTextRect(_position.x, _position.y + 3 * _fontHeight, _position.w, _fontHeight, "Klicken um Ausgabe herunterzuladen", ALIGN_RIGHT);
        break;
    default:
        DrawTextRect(_position.x, _position.y + 3 * _fontHeight, _position.w, _fontHeight, "heruntergeladen", ALIGN_RIGHT);
        break;
    }
    int line = (_position.y + _position.h) - 1;
    DrawLine(0, line, ScreenWidth(), line, BLACK);
}

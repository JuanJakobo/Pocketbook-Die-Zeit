//------------------------------------------------------------------
// listView.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      An UI class to display items in a listview
//-------------------------------------------------------------------

#ifndef LISTVIEW
#define LISTVIEW

#include "inkview.h"
#include "item.h"
#include "listViewEntry.h"

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;

class ListView
{
public:
    /**
        * Displays a list view 
        * 
        * @param ContentRect area of the screen where the list view is placed
        * @param Items items that shall be shown in the listview
        */
    ListView(const irect *contentRect, const std::shared_ptr<vector<Item>> items);

    ~ListView();

    /**
        * Navigates to the next page
        */
    void nextPage() { this->actualizePage(_shownPage + 1); };

    /**
        * Navigates to the prev page
        */
    void prevPage() { this->actualizePage(_shownPage - 1); };

    /**
        * Navigates to first page
        */
    void firstPage() { this->actualizePage(1); };

    /**
        * draws the header including an item to navigate a page up
        * 
        */
    void drawHeader(string headerText);

    /**
        * draws the footer including a page changer 
        * 
        */
    void drawFooter();

    void drawEntry(int itemID);

    /**
        * iterates through the items and sends them to the listViewEntry Class for drawing
        * 
        */
    void drawEntries();

    /**
        * Checkes if the listview has been clicked and either changes the page or returns item ID
        * 
        * @param x x-coordinate
        * @param y y-coordinate
        * @return int Item ID that has been clicked, -1 if no Item was clicked
        */
    int listClicked(int x, int y);

private:
    int _footerHeight;
    int _headerHeight;
    int _headerFontHeight;
    int _footerFontHeight;
    int _entryFontHeight;
    const irect *_contentRect;
    const std::shared_ptr<vector<Item>> _items =  nullptr;
    vector<ListViewEntry> _entries;
    std::unique_ptr<ifont> _titleFont;
    std::unique_ptr<ifont> _footerFont;
    int _page;
    int _shownPage;
    irect _pageIcon;
    irect _nextPageButton;
    irect _prevPageButton;
    irect _firstPageButton;
    irect _lastPageButton;
    int _itemCount = 7;

    void actualizePage(int pageToShow);
};
#endif
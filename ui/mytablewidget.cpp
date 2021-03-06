//Copyright 2015 Ryan Wick

//This file is part of Bandage

//Bandage is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//Bandage is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with Bandage.  If not, see <http://www.gnu.org/licenses/>.


#include "mytablewidget.h"

#include <vector>
#include <QHeaderView>
#include <QScrollBar>

MyTableWidget::MyTableWidget(QWidget * parent) :
    QTableWidget(parent)
{
    verticalHeader ()->hide();
}




void MyTableWidget::resizeEvent(QResizeEvent * event)
{
    resizeColumns();
    QTableWidget::resizeEvent(event);
}


void MyTableWidget::resizeColumns()
{
    resizeColumnsToContents();

    //Set the first column to a minimum size, as it will hold the colour
    //for both queries and hits.
    int minColumnWidth = horizontalHeader()->minimumSectionSize();
    horizontalHeader()->resizeSection(0, minColumnWidth);

    std::vector<int> columnWidths;
    int oldTotalColumnWidth = 0;
    for (int i = 0; i < columnCount(); ++i)
    {
        int columnWidth = horizontalHeader()->sectionSize(i);
        columnWidths.push_back(columnWidth);
        oldTotalColumnWidth += columnWidth;
    }

    //If the total column width exceeds the size of the table, just leave things alone.
    int tableWidth = viewport()->width();
    if (oldTotalColumnWidth > tableWidth)
        return;

    //If the code got here, then there is width to spare in the table.  Resize each column
    //(except for the first) to take up the whole width, keeping their relative size.

    int newTotalColumnWidth = minColumnWidth;
    for (int i = 1; i < columnCount() - 1; ++i)
    {
        int oldColumnWidth = columnWidths[i];
        double fraction = double(oldColumnWidth) / oldTotalColumnWidth;
        int newColumnWidth = fraction * tableWidth;
        newTotalColumnWidth += newColumnWidth;
        horizontalHeader()->resizeSection(i, newColumnWidth);
    }
    int lastColumnWidth = tableWidth - newTotalColumnWidth;
    horizontalHeader()->resizeSection(columnCount() - 1, lastColumnWidth);
}

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


#include "globals.h"
#include <QLocale>
#include <QDir>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QProcess>

QSharedPointer<Settings> g_settings;
MyGraphicsView * g_graphicsView;
double g_absoluteZoom;
QSharedPointer<BlastSearch> g_blastSearch;
QString m_tempDirectory;
QSharedPointer<AssemblyGraph> g_assemblyGraph;


QString formatIntForDisplay(int num)
{
    QLocale locale;
    return locale.toString(num);
}

QString formatIntForDisplay(long long num)
{
    QLocale locale;
    return locale.toString(num);
}

QString formatDoubleForDisplay(double num, int decimalPlacesToDisplay)
{
    QLocale locale;
    QString withCommas = locale.toString(num, 'f');

    QString final;
    bool pastDecimalPoint = false;
    int numbersPastDecimalPoint = 0;
    for (int i = 0; i < withCommas.length(); ++i)
    {
        final += withCommas[i];

        if (pastDecimalPoint)
            ++numbersPastDecimalPoint;

        if (numbersPastDecimalPoint >= decimalPlacesToDisplay)
            return final;

        if (withCommas[i] == locale.decimalPoint())
            pastDecimalPoint = true;
    }
    return final;
}


void readFastaFile(QString filename, std::vector<QString> * names, std::vector<QString> * sequences)
{
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QString name = "";
        QString sequence = "";

        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QApplication::processEvents();

            QString line = in.readLine();

            if (line.length() == 0)
                continue;

            if (line.at(0) == '>')
            {
                //If there is a current sequence, add it to the vectors now.
                if (name.length() > 0)
                {
                    names->push_back(name);
                    sequences->push_back(sequence);
                }

                line.remove(0, 1); //Remove '>' from start
                name = line;
                sequence = "";
            }

            else //It's a sequence line
                sequence += line.simplified();
        }

        //Add the last target to the results now.
        if (name.length() > 0)
        {
            names->push_back(name);
            sequences->push_back(sequence);
        }

        inputFile.close();
    }
}


//The function returns a node name, replacing "+" at the end with "-" or
//vice-versa.
QString getOppositeNodeName(QString nodeName)
{
    QChar lastChar = nodeName.at(nodeName.size() - 1);
    nodeName.chop(1);

    if (lastChar == '-')
        return nodeName + "+";
    else
        return nodeName + "-";
}


QString convertGraphFileTypeToString(GraphFileType graphFileType)
{
    QString graphFileTypeString;
    switch (graphFileType)
    {
    case LAST_GRAPH: graphFileTypeString = "LastGraph"; break;
    case FASTG: graphFileTypeString = "FASTG"; break;
    case GFA: graphFileTypeString = "GFA"; break;
    case TRINITY: graphFileTypeString = "Trinity.fasta"; break;
    case ANY_FILE_TYPE: graphFileTypeString = "any"; break;
    case UNKNOWN_FILE_TYPE: graphFileTypeString = "unknown"; break;
    }
    return graphFileTypeString;
}


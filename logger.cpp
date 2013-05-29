/****************************************************************************
**
** mysms - Send & receive all your SMS on your Notebook, PC or tablet – like on your smartphone
** Copyright (C) 2013 Up To Eleven
** All rights reserved.
**
**
** This library is free software.  either version 2.1 of the License, or (at your option) any later version.
** you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation;
** either version 2.1 of the License, or (at your option) any later version.
**
**
** This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
** Without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License along with this library;
** if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
**
****************************************************************************/

#include "globalsettings.h"
#include "logger.h"

static QString logFileName = LOGFILE_PATH;
bool Logger::mfileOpen = false;
QFile Logger::mfile(logFileName);

Logger::Logger()
{}

void Logger::open_file()
{
#ifdef QT_DEBUG
    if(!mfile.open(QIODevice::WriteOnly | QIODevice::Text))
        mfileOpen = false;
    else
        mfileOpen = true;    
#endif
}

void Logger::log_message(QString msg)
{
#ifdef QT_DEBUG
    if (mfileOpen)
    {
        QTextStream out(&mfile);                      
        out << msg << "\n";
        out.flush();
    }
#endif
}

void Logger::close_file()
{
#ifdef QT_DEBUG
    if (mfileOpen)
    {
        if (mfile.isOpen())
        {
            mfile.close();
            mfileOpen = false;
        }
    }
#endif
}

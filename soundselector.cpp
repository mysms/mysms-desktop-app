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

#include "soundselector.h"

#include <QSound>
#include <QApplication>

SoundSelector* SoundSelector::mSoundSelector = NULL;

const QString* playList[PLAYLISTSIZE] = {
                                            new QString("mysms"),
                                            new QString("bottle-open"), new QString("can-open"),        new QString("coffee-cup"),
                                            new QString("coin-drop"),   new QString("glass-clink"),
                                            new QString("page-flip"),   new QString("snap"),            new QString("squeeze-toy"),
                                            new QString("stamp"),       new QString("zippo-close")
                                        };

SoundSelector::SoundSelector()
{}

SoundSelector* SoundSelector::getInstance()
{
    if (mSoundSelector == NULL )
    {
        mSoundSelector = new SoundSelector();
    }

    return mSoundSelector;
}

void SoundSelector::playSound(int index)
{
    QString myPath = QApplication::applicationDirPath();

    if (index < PLAYLISTSIZE)
    {
        myPath += "/audio/" + (*playList[index]) + ".wav";
        QSound::play(myPath);  // QSound has problems with resource dir, therefore absolute path is necessary
    }
}

QString SoundSelector::getSoundTitle(int index) const
{
    if (index < PLAYLISTSIZE)
    {
        return *playList[index];
    }

    return "";
}

/******************************************************************************
** KVNShuffle 0.2
** Copyright (C) 2011 Valery Kharitonov <kharvd@gmail.com>
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
** If you have questions regarding the use of this file, please contact me at
** kharvd@gmail.com.
**
******************************************************************************/

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QLibraryInfo>
#include <QTextCodec>
#include <QTranslator>
#include <QVariant>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_ru",
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QCoreApplication::setOrganizationName("Валерий Харитонов");
    QCoreApplication::setApplicationName("KVNShuffle");
    QCoreApplication::setApplicationVersion("0.2");

    a.setProperty("iniFile", QVariant("settings.ini"));

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));

    QFile style("style.css");

    if (style.open(QIODevice::ReadOnly | QIODevice::Text)) {
        a.setStyleSheet(style.readAll());
    } else {
        qDebug() << "CSS file not found";
    }

    QIcon appIcon;
    appIcon.addPixmap(QPixmap(":/icons/icon128.png"));
    appIcon.addPixmap(QPixmap(":/icons/icon64.png"));
    appIcon.addPixmap(QPixmap(":/icons/icon48.png"));
    appIcon.addPixmap(QPixmap(":/icons/icon32.png"));
    a.setWindowIcon(appIcon);
    MainWindow w;
    w.show();

    return a.exec();
}

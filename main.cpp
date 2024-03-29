/****************************************************************************
**
** Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QCoreApplication>
#include <QtCore>

#include <stdlib.h>

#include "WislandService.hpp"
//#include "ChatClient.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    qsrand(rand());
    QCoreApplication app(argc, argv);
    Server server;
    //QScopedPointer<ChatClient> chatClient(new ChatClient(QLatin1String("_wisland_chat._tcp"), 0));
//    server.show();
    //qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
//    QString pin = QCryptographicHash::hash(QHostInfo::localHostName(), QCryptographicHash::Md5).toHex();
//    serviceName = QString("Wisland-DESKTOP:%1").arg(pin.toUpper());
//    WService * chatClient = new WService(QLatin1String(serviceName.toLatin1()),
//                QLatin1String("_wisland_chat._tcp"), app);
    //chatClient->setParent(&app);
    return app.exec();
}


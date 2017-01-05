/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
** Author: Peter Buelow
** Created: April 24, 2016
****************************************************************************/

#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include "qtservice.h"
#include "Program.h"

// As a strict requirement, Iris will NOT support a GUI. Hence, in subclassing 
// the QtService template, templatize on QCoreApplication instead of QApplication.
class AppService : public QtService<QCoreApplication>
{
public:
    AppService(int argc, char **argv);
    virtual ~AppService();

protected:
	// Virtual methods inherited from the base class. Must be implemented!
	
	// This method--start()--is intended to perform the service's work. 
	// Within it, it is recommended that one create some main object on 
	// the heap which is the heart of the 'customized' service. Note that
	// the method is only called when no service specific arguments are 
	// passed to the service's constructor. Also, it is only called by 
	// exec() after the executeApplication() method has been invoked.
    void start();
    void stop();
//    void pause() { mp_theServerDaemon->pause(); }
//    void resume() {	mp_theServerDaemon->resume(); }

private:
    Program m_Program;
};

#endif // HTTP_SERVICE_H

/*========================================================================*
 |                                                                        |
 |  This file is part of the live coding environment BassLive.            |
 |  Copyright (C) 2017 Bass Jansson.                                      |
 |                                                                        |
 |  This program is free software: you can redistribute it and/or modify  |
 |  it under the terms of the GNU General Public License as published by  |
 |  the Free Software Foundation, either version 3 of the License, or     |
 |  (at your option) any later version.                                   |
 |                                                                        |
 |  This program is distributed in the hope that it will be useful,       |
 |  but WITHOUT ANY WARRANTY; without even the implied warranty of        |
 |  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          |
 |  GNU General Public License for more details.                          |
 |                                                                        |
 |  You should have received a copy of the GNU General Public License     |
 |  along with this program. If not, see <http://www.gnu.org/licenses/>.  |
 |                                                                        |
 *========================================================================*/


#ifndef MAIN_APPLICATION_H_INCLUDED
#define MAIN_APPLICATION_H_INCLUDED

#include "MainWindow.h"
#include "MainGUIComponent.h"
#include "MainAudioProcessor.h"


class MainApplication : public JUCEApplication
{
public:
    /*====================================================================*/
    MainApplication() {}

    /*====================================================================*/
    const String getApplicationName()    override { return ProjectInfo::projectName; }
    const String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed()    override { return false; }

    /*====================================================================*/
    void initialise(const String& commandLine) override;
    void shutdown() override;

    /*====================================================================*/
    void systemRequestedQuit() override;
    void anotherInstanceStarted(const String& commandLine) override;

private:
    /*====================================================================*/
    ScopedPointer<MainWindow>         mainWindow;
    ScopedPointer<MainGUIComponent>   mainGUIComponent;
    ScopedPointer<MainAudioProcessor> mainAudioProcessor;

    /*====================================================================*/
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainApplication)
};


#endif /* MAIN_APPLICATION_H_INCLUDED */
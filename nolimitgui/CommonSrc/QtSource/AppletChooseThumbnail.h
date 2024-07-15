#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletChooseThumbnailUi;
}
QT_END_NAMESPACE

class ThumbnailEditWidget;

class AppletChooseThumbnail : public AppletBase
{
	Q_OBJECT
public:
    AppletChooseThumbnail( AppCommon& app, QWidget* parent );
	virtual ~AppletChooseThumbnail();

signals:
    void						signalThumbSelected( AppletBase * baseApplet, ThumbnailEditWidget * widget );

private slots:
    void						slotImageChanged();

protected:
    //=== vars ===//
    Ui::AppletChooseThumbnailUi&	ui;
    std::string					m_strSavedCwd;
    std::string					m_strStoryBoardDir;
    std::string					m_strStoryBoardFile;
};



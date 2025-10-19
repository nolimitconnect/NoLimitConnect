#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include <Plugins/FileInfo.h>
#include "ToGuiFileXferInterface.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletAboutFileUi;
}
QT_END_NAMESPACE

class FileInfo;
class AssetBaseInfo;
class QPlainTextEdit;

class AppletAboutFile : public AppletBase
{
	Q_OBJECT
public:
	AppletAboutFile( AppCommon& app, QWidget* parent, QString launchParam = "" );
	virtual ~AppletAboutFile();

    void						setFileInfo( FileInfo& fileInfo );

protected slots:
    void                        slotBrowseFolderButClick( void );

protected:
    QPlainTextEdit*             getInfoEdit( void );
    void                        infoMsg( const QString& text );

    bool                        updateFromFileInfo( FileInfo& fileInfo );
    bool                        updateFromAssetInfo( AssetBaseInfo& assetInfo );

    void                        showThumbnail( bool visible );

    //=== vars ===//
    Ui::AppletAboutFileUi&	    ui;

    FileInfo                    m_FileInfo;
};



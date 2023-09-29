#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include <ptop_src/ptop_engine_src/Plugins/FileInfo.h>
#include "ToGuiFileXferInterface.h"

#include "ui_AppletFileOfferSelect.h"

#include <PktLib/VxCommon.h>

class FileShareItemWidget;
class VxNetIdent;
class VxSha1Hash;
class FileItemInfo;
class FileListReplySession;
class FileInfo;

class AppletFileOfferSelect : public AppletBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:
	AppletFileOfferSelect( AppCommon& app, QWidget* parent, QString launchParam = "" );
	virtual ~AppletFileOfferSelect();

    virtual void				setPluginType( EPluginType pluginType ) override;
    virtual void				setUser( GuiUser* hisIdent ) override;

    void						addFile( FileInfo& fileInfo );
    void						setFileFilter( EFileFilterType eFileFilter );

protected slots:
    void						slotHomeButtonClicked( void ) override;
    void						statusMsg( QString strMsg );
    void						slotRequestFileList( void );
    void						slotApplyFileFilter( unsigned char fileTypeMask );

    void						slotListItemClicked( QListWidgetItem* item );
    void						slotListItemDoubleClicked( QListWidgetItem* item );

    void						slotListFileIconClicked( QListWidgetItem* item );
    void						slotListPlayIconClicked( QListWidgetItem* item );
    void						slotListLibraryIconClicked( QListWidgetItem* item );
    void						slotListShareFileIconClicked( QListWidgetItem* item );
    void						slotListShredIconClicked( QListWidgetItem* item );

    void                        slotBrowseForFileButClick( void );
    void                        slotBrowseFolderButClick( void );

protected:
    virtual void				showEvent( QShowEvent* ev ) override;
    virtual void				hideEvent( QHideEvent* ev ) override;

    virtual void				callbackToGuiFileList( FileInfo& fileInfo ) override;
    virtual void				callbackToGuiFileListCompleted( void ) override;

    FileShareItemWidget*		fileToWidget( FileInfo& fileInfo );
    FileItemInfo*				widgetToFileItemInfo( FileShareItemWidget* item );

    FileShareItemWidget*		findListEntryWidget( FileInfo& fileInfo );
    void						clearFileList( void );

    FileShareItemWidget*        findItemByFileName( QString fileName );

    void                        onFileSelected( FileInfo& fileInfo );
    void                        onFileRemoved( FileInfo& fileInfo );

    bool                        verifyFile( void );

    //=== vars ===//
    Ui::AppletFileOfferSelectUi	ui;
    bool						m_IsSelectAFileMode{ false };
    bool						m_FileWasSelected{ false };
    FileInfo                    m_FileInfo;
    EFileFilterType				m_eFileFilterType{ eFileFilterAll };
    uint8_t						m_FileFilterMask{ 0 };
};



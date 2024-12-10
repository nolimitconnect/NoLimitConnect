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
    class AppletFileOfferSelectUi;
}
QT_END_NAMESPACE

class FileShareItemWidget;
class VxNetIdent;
class VxSha1Hash;
class FileItemInfo;
class FileListReplySession;
class FileInfo;
class QListWidgetItem;

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
    void						slotApplyFileFilter( EFileFilterType fileFilter );

    void						slotListItemClicked( QListWidgetItem* item );
    void						slotListItemDoubleClicked( QListWidgetItem* item );

    void						slotListFileIconClicked( QListWidgetItem* item );
    void						slotListPlayIconClicked( QListWidgetItem* item );
    void						slotListLibraryIconClicked( QListWidgetItem* item );
    void                        slotListPlayExternIconClicked( QListWidgetItem* item );
    void						slotListShareFileIconClicked( QListWidgetItem* item );
    void						slotListShredIconClicked( QListWidgetItem* item );

    void                        slotBrowseForFileButClick( void );
    void                        slotBrowseFolderButClick( void );

protected:
    virtual void				showEvent( QShowEvent* ev ) override;
    virtual void				hideEvent( QHideEvent* ev ) override;

    virtual void				callbackToGuiFileList( VxGUID& appInstId, FileInfo& fileInfo ) override;
    virtual void				callbackToGuiFileListCompleted( VxGUID& appInstId ) override;

    FileShareItemWidget*		fileToWidget( FileInfo& fileInfo );
    FileItemInfo*				widgetToFileItemInfo( FileShareItemWidget* item );

    FileShareItemWidget*		findListEntryWidget( FileInfo& fileInfo );
    void						clearFileList( void );

    FileShareItemWidget*        findItemByFileName( QString fileName );

    void                        onFileSelected( FileInfo& fileInfo );
    void                        onFileRemoved( FileInfo& fileInfo );

    bool                        verifyFile( void );

    void                        wantFileXferCallbacks( bool enable );

    //=== vars ===//
    Ui::AppletFileOfferSelectUi&	ui;
    bool						m_IsSelectAFileMode{ false };
    bool						m_FileWasSelected{ false };
    FileInfo                    m_FileInfo;
    EFileFilterType				m_eFileFilterType{ eFileFilterAll };
    bool                        m_FileXferCallbacksRequested{ false };
};



#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPeerBase.h"
#include "ui_AppletPeerViewSharedFiles.h"
#include "ToGuiFileXferInterface.h"

#include <PktLib/VxCommon.h>

#include <QString>
#include <QDialog>

class FileXferWidget;
class GuiFileXferSession;
class FileListReplySession;
class GuiUser;
class VxSha1Hash;
class VxMyFileInfo;

class AppletPeerViewSharedFiles : public AppletPeerBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:
	AppletPeerViewSharedFiles( AppCommon& myApp, QWidget * parent = nullptr );
	virtual ~AppletPeerViewSharedFiles() override = default;

public:
    void						addFile( GuiUser * netIdent, VxMyFileInfo& fileInfo  );

private slots:
    void						slotHomeButtonClicked( void ) override;
	void						statusMsg( QString strMsg );
	void						slotApplyFileFilter( unsigned char fileTypeMask );
	void						slotDownloadFileSelected( int iMenuId, QWidget * popupMenu );

	void						slotItemClicked( QListWidgetItem * item );
	void						slotCancelButtonClicked( QListWidgetItem * item );
	void						slotPlayButtonClicked( QListWidgetItem * item );
	void						slotLibraryButtonClicked( QListWidgetItem * item );
	void						slotFileShareButtonClicked( QListWidgetItem * item );
	void						slotShredButtonClicked( QListWidgetItem * item );

protected:
    virtual void				showEvent( QShowEvent * ev ) override;
    virtual void				hideEvent( QHideEvent * ev ) override;

    virtual void				toGuiFileListReply(	FileListReplySession * replySession ) override;
    virtual void				toGuiFileXferState( EPluginType pluginType, VxGUID& lclSession, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 ) override;
    virtual void				toGuiStartDownload( GuiFileXferSession * xferSession ) override;
    virtual void				toGuiFileDownloadComplete( EPluginType pluginType, VxGUID& lclSession, QString newFileName, EXferError xferError ) override;

	FileXferWidget *			fileToWidget( GuiUser * netIdent, VxMyFileInfo& fileInfo );
	void						updateListEntryWidget( FileXferWidget * item, GuiFileXferSession * xferSession );
	GuiFileXferSession *		widgetToFileItemInfo( FileXferWidget * item );

	FileXferWidget *			findListEntryWidget( VxGUID fileInstance );
	GuiFileXferSession *		findSession( VxGUID lclSessionId );

	void						clearFileList( void );
	void						moveUpOneFolder( void );
	void						promptForDownload( GuiFileXferSession * poInfo );
	void						beginDownload( GuiFileXferSession * xferSession, QListWidgetItem * item );
	void						cancelDownload( GuiFileXferSession * xferSession, QListWidgetItem * item );
	void						cancelUpload( GuiFileXferSession * xferSession, QListWidgetItem * item );
	void						removeDownload( GuiFileXferSession * xferSession, QListWidgetItem * item );
	bool						confirmDeleteFile( bool shredFile );

	//=== vars ===//
	Ui::AppletPeerViewSharedFilesUi	ui;
    GuiUser *				    m_Friend{ nullptr };
	int							m_iMenuItemHeight{ 34 };
	std::string					m_strCurrentDirectory;
	GuiFileXferSession *		m_SelectedFileInfo{ nullptr };
	uint8_t						m_u8FileFilter{ 0 };
	VxGUID						m_LclSessionId;
};

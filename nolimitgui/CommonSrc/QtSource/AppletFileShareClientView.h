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

#include "ToGuiFileXferInterface.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletFileShareClientViewUi;
}
QT_END_NAMESPACE

class FileXferWidget;
class GuiFileXferSession;
class FileListReplySession;
class GuiUser;
class VxSha1Hash;
class FileInfo;
class QListWidgetItem;

class AppletFileShareClientView : public AppletPeerBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:
	AppletFileShareClientView( AppCommon& myApp, QWidget* parent = nullptr );
	virtual ~AppletFileShareClientView();

	void                        setIdentity( GuiUser* guiUser );

    void						addFile( GuiUser* guiUser, EPluginType pluginType, FileInfo& fileInfo  );

private slots:
    void						slotHomeButtonClicked( void ) override;
	void						statusMsg( QString strMsg );
	void						slotApplyFileFilter( EFileFilterType fileFilter );
	void						slotDownloadFileSelected( int iMenuId, QWidget* popupMenu );

	void						slotItemClicked( QListWidgetItem* item );
	void						slotAcceptButtonClicked( QListWidgetItem* item );
	void						slotCancelButtonClicked( QListWidgetItem* item );
	void						slotStreamButtonClicked( QListWidgetItem* item );

	void						slotPlayButtonClicked( QListWidgetItem* item );
	void						slotPlayExternButtonClicked( QListWidgetItem* item );
	void						slotLibraryButtonClicked( QListWidgetItem* item );
	void						slotFileShareButtonClicked( QListWidgetItem* item );
	void						slotShredButtonClicked( QListWidgetItem* item );

protected:
    virtual void				showEvent( QShowEvent* ev ) override;
    virtual void				hideEvent( QHideEvent* ev ) override;

    virtual void				toGuiFileListReply(	FileListReplySession* replySession ) override;
    virtual void				toGuiFileXferState( EPluginType pluginType, VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 ) override;
    virtual void				toGuiFileDownloadStart( GuiFileXferSession* xferSession ) override;
    virtual void				toGuiFileDownloadComplete( EPluginType pluginType, VxGUID& lclSessionId, QString newFileName, EXferError xferError ) override;

	virtual void				toGuiSearchResultFileSearch( GuiUser* guiUser, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo ) override;

	FileXferWidget*				fileToWidget( GuiUser* guiUser, EPluginType pluginType, FileInfo& fileInfo );
	void						updateListEntryWidget( FileXferWidget* item, GuiFileXferSession* xferSession );
	GuiFileXferSession*			widgetToFileItemInfo( FileXferWidget* item );

	FileXferWidget*				findListEntryWidget( VxGUID fileInstance );
	GuiFileXferSession*			findSession( VxGUID lclSessionId );

	void						clearFileList( void );
	void						moveUpOneFolder( void );
	void						promptForDownload( GuiFileXferSession* poInfo );
	void						beginDownload( GuiFileXferSession* xferSession, QListWidgetItem* item );
	void						cancelDownload( GuiFileXferSession* xferSession, QListWidgetItem* item );
	void						cancelUpload( GuiFileXferSession* xferSession, QListWidgetItem* item );
	void						removeDownload( GuiFileXferSession* xferSession, QListWidgetItem* item );
	bool						confirmDeleteFile( bool shredFile );

	void						wantFileXferCallbacks( bool enable );

	//=== vars ===//
	Ui::AppletFileShareClientViewUi&	ui;
    GuiUser*				    m_Friend{ nullptr };
	int							m_iMenuItemHeight{ 34 };
	std::string					m_strCurrentDirectory;
	GuiFileXferSession*			m_SelectedFileInfo{ nullptr };
	EFileFilterType				m_eFileFilterType{ eFileFilterAll };
	VxGUID						m_LclSessionId;
	VxGUID                      m_HisOnlineId;
	bool						m_FileXferCallbackRequested{ false };
};

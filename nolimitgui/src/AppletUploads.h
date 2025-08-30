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
    class AppletUploadsUi;
}
QT_END_NAMESPACE

class GuiFileXferSession;
class FileXferWidget;
class QListWidgetItem;

class AppletUploads : public AppletPeerBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:
	AppletUploads(	AppCommon&	app, 
						QWidget*		parent = nullptr );
	virtual ~AppletUploads() override;

public:
	FileXferWidget*				addDownload( GuiFileXferSession* poFileInfo );

	bool						isUploadInProgress( VxGUID fileInstance );

	GuiFileXferSession*			findSession( VxGUID fileInstance );
	FileXferWidget*				findListEntryWidget( VxGUID fileInstance );
	FileXferWidget*				addUpload( GuiFileXferSession* poSession );

private slots:
	void						slotFileXferItemClicked( QListWidgetItem* item );

	void						slotFileIconButtonClicked( QListWidgetItem* item );
	void						slotAcceptButtonClicked( QListWidgetItem* item );
	void						slotCancelButtonClicked( QListWidgetItem* item );
	void						slotPlayButtonClicked( QListWidgetItem* item );
	void						slotPlayExternButtonClicked( QListWidgetItem* item );
	void						slotLibraryButtonClicked( QListWidgetItem* item );
	void						slotFileShareButtonClicked( QListWidgetItem* item );
	void						slotShredButtonClicked( QListWidgetItem* item );

protected:

    virtual void				toGuiFileUploadStart( GuiFileXferSession* xferSession ) override;
    virtual void				toGuiFileXferState( EPluginType plguinType, VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 ) override;
    virtual void				toGuiFileUploadComplete( EPluginType plguinType, VxGUID& lclSessionId, EXferError xferError ) override;

	FileXferWidget*				sessionToWidget( GuiFileXferSession* poSession );
	void						updateListEntryWidget( FileXferWidget* item, GuiFileXferSession* poSession );
	GuiFileXferSession*			widgetToSession( FileXferWidget* item );
	bool						confirmDeleteFile( bool shredFile );

	void						populateList( std::vector<GuiFileXferSession*>& sessionList );
	void						updateSession( GuiFileXferSession* xferSession, bool resetState = false );

	//=== vars ===//
	Ui::AppletUploadsUi&		ui;
};


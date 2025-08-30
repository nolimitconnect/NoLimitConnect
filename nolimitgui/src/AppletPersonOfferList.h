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
    class AppletPersonOfferListUi;
}
QT_END_NAMESPACE

class FileXferWidget;
class P2PEngine;
class GuiFileXferSession;

class AppletPersonOfferList : public AppletPeerBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:
	AppletPersonOfferList(	AppCommon&	app, 
							QWidget*		parent = nullptr );
	virtual ~AppletPersonOfferList() override;

	//bool						isXferInProgress( VxGUID fileInstance );
	//FileXferWidget*			addDownload( GuiFileXferSession* poSession );

private slots:
    /*

	void						slotFileXferItemClicked( QListWidgetItem* item );

	void						slotFileIconButtonClicked( QListWidgetItem* item );
	void						slotCancelButtonClicked( QListWidgetItem* item );
	void						slotPlayButtonClicked( QListWidgetItem* item );
	void						slotLibraryButtonClicked( QListWidgetItem* item );
	void						slotFileShareButtonClicked( QListWidgetItem* item );
	void						slotShredButtonClicked( QListWidgetItem* item );
    */

protected:
    virtual void				showEvent( QShowEvent* ev ) override;
    virtual void				hideEvent( QHideEvent* ev ) override;

    /*
    // override default behavior of closing dialog when back button is clicked
    void                        onBackButtonClicked( void ) override;

    virtual void				toGuiFileDownloadStart(  GuiFileXferSession* xferSession ) override;
    virtual void				toGuiFileXferState( VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 ) override;
    virtual void				toGuiFileDownloadComplete(  VxGUID& lclSessionId, QString newFileName, EXferError xferError ) override;

	FileXferWidget*			sessionToWidget( GuiFileXferSession* poSession );
	void						updateListEntryWidget( FileXferWidget* item, GuiFileXferSession* poSession );
	GuiFileXferSession*		widgetToSession( FileXferWidget* item );
	GuiFileXferSession*		findSession( VxGUID lclSessionId );
	FileXferWidget*			findListEntryWidget( VxGUID lclSessionId );
	bool						confirmDeleteFile( bool shredFile );
    */

	//=== vars ===//
	Ui::AppletPersonOfferListUi&	ui;
};

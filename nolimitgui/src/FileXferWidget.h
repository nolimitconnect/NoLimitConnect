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

#include "GuiFileXferSession.h"
#include "MyIconsDefs.h"

#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
    class FileXferWidgetClass;
}
QT_END_NAMESPACE

class AppCommon;
class MyIcons;

class FileXferWidget : public QWidget, public QListWidgetItem
{
	Q_OBJECT

public:
	FileXferWidget(QWidget* parent=nullptr);
	virtual ~FileXferWidget();

	AppCommon&					getMyApp( void ) { return m_MyApp; }
	MyIcons&					getMyIcons( void );

    void						setFileItemInfo( GuiFileXferSession* fileItemInfo );
    GuiFileXferSession*         getFileItemInfo( void );

    void						setXferState( EXferState xferState, EXferError xferErr, int param1 );
	void						resetXferState( void );

	void						setIsSharedFile( bool isShared );
	bool						getIsShared( void );
	void						setIsInLibrary( bool isInLibrary );
	bool						getIsInLibrary( void );

	void						setFileIconButtonEnabled( bool enable );
	void						setFileProgressBarValue( int val );

	void						updateWidgetFromInfo( void );

signals:
	void						signalFileXferItemClicked( QListWidgetItem* poItemWidget );
	void						signalFileIconButtonClicked( QListWidgetItem* poItemWidget );

	void						signalAcceptButtonClicked( QListWidgetItem* poItemWidget );
	void						signalCancelButtonClicked( QListWidgetItem* poItemWidget );
	void						signalStreamButtonClicked( QListWidgetItem* poItemWidget );

	void						signalPlayButtonClicked( QListWidgetItem* poItemWidget );
	void						signalPlayExternButtonClicked( QListWidgetItem* poItemWidget );
	void						signalLibraryButtonClicked( QListWidgetItem* poItemWidget );
	void						signalFileShareButtonClicked( QListWidgetItem* poItemWidget );
	void						signalShredButtonClicked( QListWidgetItem* poItemWidget );

protected slots:
	void						slotFileIconButtonClicked( void );
	void						slotAcceptButtonClicked( void );
	void						slotCancelButtonClicked( void );
	void						slotStreamButtonClicked( void );

	void						slotPlayButtonClicked( void );
	void						slotPlayExternButtonClicked( void );
	void						slotLibraryButtonClicked( void );
	void						slotFileShareButtonClicked( void );
	void						slotShredButtonClicked( void );

protected:
    virtual void				mousePressEvent( QMouseEvent * event ) override;
    virtual void				resizeEvent( QResizeEvent* resizeEvent ) override;
	virtual void				updateXferInfo( void );
	void						setCancelIcon( EMyIcons cancelIcon );

	Ui::FileXferWidgetClass&	ui;
	AppCommon&					m_MyApp;
};

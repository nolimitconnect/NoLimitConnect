#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
    class FileShareItemWidget;
}
QT_END_NAMESPACE

class FileItemInfo;
class AppCommon;
class MyIcons;

class FileShareItemWidget : public QWidget, public QListWidgetItem
{
	Q_OBJECT

public:
	FileShareItemWidget(QWidget* parent=nullptr );
	virtual ~FileShareItemWidget();

	AppCommon&					getMyApp( void ) { return m_MyApp; }
	MyIcons&					getMyIcons( void );

	void						setIsSharedFile( bool isShared );
	bool						getIsShared( void );
	void						setIsInLibrary( bool isInLibrary );
	bool						getIsInLibrary( void );

	bool						getIsMediaFile( void );

	void						setSelectAFileMode( bool selectAFile );
	bool						getSelectAFileMode( void ) { return m_SelectAFileMode; }

	void						setFileItemInfo( FileItemInfo* fileItemInfo );
	FileItemInfo*				getFileItemInfo( void );

	void						updateWidgetFromInfo( void );

signals:
	void						signalFileShareItemClicked( QListWidgetItem* poItemWidget );
	void						signalFileIconClicked( QListWidgetItem* poItemWidget );

	void						signalPlayButtonClicked( QListWidgetItem* poItemWidget );
	void						signalPlayExternButtonClicked( QListWidgetItem* poItemWidget );
	void						signalLibraryButtonClicked( QListWidgetItem* poItemWidget );
	void						signalFileShareButtonClicked( QListWidgetItem* poItemWidget );
	void						signalShredButtonClicked( QListWidgetItem* poItemWidget );

protected slots:
	void						slotFileIconButtonClicked( void );
	void						slotPlayButtonClicked( void );
	void						slotPlayExternButtonClicked( void );
	void						slotLibraryButtonClicked( void );
	void						slotFileShareButtonClicked( void );
	void						slotShredButtonClicked( void );

protected:
	virtual void				mousePressEvent(QMouseEvent * event);
	virtual void				resizeEvent(QResizeEvent* resizeEvent);

	Ui::FileShareItemWidget&	ui;
	AppCommon&					m_MyApp;
	bool						m_SelectAFileMode{ false };
};

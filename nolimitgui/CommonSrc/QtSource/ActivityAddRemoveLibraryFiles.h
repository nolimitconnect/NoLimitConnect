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

#include "ActivityBase.h"
#include "ToGuiFileXferInterface.h"
#include "ui_ActivityAddRemoveLibraryFiles.h"

#include <CoreLib/VxDefs.h>
#include <CoreLib/VxGUID.h>

class FileShareItemWidget;
class FileItemInfo;
class FromGuiInterface;
class FromEngineInterface;
class P2PEngine;
class QTimer;

class ActivityAddRemoveLibraryFiles : public ActivityBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:

    ActivityAddRemoveLibraryFiles( AppCommon& app, QWidget* parent = nullptr );
    virtual ~ActivityAddRemoveLibraryFiles();

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

	void						setTitle( QString strTitle );

	FileShareItemWidget*		fileToWidget( FileInfo& fileInfo );
	void						updateListEntryWidget( FileShareItemWidget* item );

protected slots:
    void						slotHomeButtonClicked( void ) override;
	void						slotUpDirectoryClicked( void );
	void						slotBrowseButtonClicked( void );

	void						slotListItemClicked( QListWidgetItem* item );
	void						slotListFileIconClicked( QListWidgetItem* item );
	void						slotListLockIconClicked( QListWidgetItem* item );
	void						slotRequestFileList( void );
	
protected:
	virtual void				callbackToGuiFileList( FileInfo& fileInfo ) override;
	virtual void				callbackToGuiFileListCompleted( void ) override;

	void						fromListWidgetRequestFileList( void );
	void						setCurrentBrowseDir( QString browseDir );
	void						setActionEnable( bool enable );
	void						addFile( FileInfo& fileInfo );

	void						updateListEntryWidget( FileShareItemWidget* item, FileItemInfo* poSession );
	void						clearFileList( void );



	//=== vars ===//
	Ui::ShareFilesDialog		ui;

	std::string					m_strCurBrowseDirectory;
	bool						m_bFetchInProgress;
	QTimer *					m_WidgetClickEventFixTimer;
};



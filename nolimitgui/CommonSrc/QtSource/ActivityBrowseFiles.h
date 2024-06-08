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
#include "ui_ActivityBrowseFiles.h"
#include "FileItemInfo.h"
#include "ToGuiFileXferInterface.h"

#include <CoreLib/VxTimer.h>

class FileShareItemWidget;
class FromGuiInterface;
class FromEngineInterface;
class P2PEngine;
class QTimer;

class ActivityBrowseFiles : public ActivityBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:

	ActivityBrowseFiles( AppCommon& app, EFileFilterType fileFilter, QWidget* parent = nullptr, bool isSelectAFileMode = false );
	virtual ~ActivityBrowseFiles();

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

public:
	void						setFileFilter( EFileFilterType eFileFilter );
	void						setCurrentBrowseDir( QString browseDir );

	FileShareItemWidget*		fileToWidget( FileInfo& fileInfo );

	bool						getWasFileSelected( void )						{ return m_FileWasSelected; }
	FileInfo&					getSelectedFileInfo( void )						{ return m_SelectedFileInfo; }

protected slots:
    void						slotHomeButtonClicked( void ) override;
	void						slotUpDirectoryClicked( void );
	void						slotBrowseButtonClicked( void );

	void						slotRequestFileList( void );
	void						slotApplyFileFilter( unsigned char fileMask );

	void						slotListItemClicked( QListWidgetItem* item );
	void						slotListItemDoubleClicked( QListWidgetItem* item );
	void						slotListFileIconClicked( QListWidgetItem* item );
	void						slotListShareFileIconClicked( QListWidgetItem* item );
	void						slotListLibraryIconClicked( QListWidgetItem* item );
	void						slotListPlayIconClicked( QListWidgetItem* item );
	void						slotListPlayExternIconClicked( QListWidgetItem* item );
	void						slotListShredIconClicked( QListWidgetItem* item );
	void						slotAddAllButtonClicked( void );
	
protected:
    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;

	void						callbackToGuiFileList( FileInfo& fileInfo ) override;
	void						callbackToGuiFileListCompleted( void ) override;
	void						toGuiFileDeleted( QString& fileName ) override;

	void						fromListWidgetRequestFileList( void );
	
	void						setActionEnable( bool enable );
	void						addFile( FileInfo& fileInfo );

	void						clearFileList( void );
	void						setDefaultCurrentDir( EFileFilterType eFileFilterType );
	std::string					getDefaultDir( int eFileFilterType );

	void						showAddAllToLibrary( bool visible );

	void                        updateStorageSpace( std::string fileName );

	Ui::BrowseFilesWidget		ui;

	std::string					m_CurBrowseDirectory;
	std::string					m_LastBrowseDir;

	QTimer *					m_WidgetClickEventFixTimer;
	VxTimer						m_ClickToFastTimer;

	bool						m_bFetchInProgress{ false };
	bool						m_IsSelectAFileMode{ false };
	bool						m_FileWasSelected{ false };
	FileInfo					m_SelectedFileInfo;

	EFileFilterType				m_eFileFilterType{ eFileFilterAll };
	uint8_t						m_FileFilterMask{ 0 };
	uint8_t						m_LastFileFilterMask{ 0 };
};



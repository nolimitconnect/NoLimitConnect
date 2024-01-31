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

#include "AppletBase.h"
#include "ui_AppletBrowseFiles.h"
#include "FileItemInfo.h"
#include "ToGuiFileXferInterface.h"

#include <CoreLib/VxTimer.h>

class FileShareItemWidget;
class FromGuiInterface;
class FromEngineInterface;
class P2PEngine;
class QTimer;

class AppletBrowseFiles : public AppletBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:

	AppletBrowseFiles( AppCommon& app, QWidget* parent, QString launchParam );
	virtual ~AppletBrowseFiles() override;

public:
	void						setFileFilter( EFileFilterType eFileFilter );
	void						setCurrentDirectory( QString browseDir );

	FileShareItemWidget*		fileToWidget( FileInfo& fileInfo );
	void						updateListEntryWidget( FileShareItemWidget* item );
	bool						getWasFileSelected( void )						{ return m_FileWasSelected; }
	uint8_t						getSelectedFileType( void )						{ return m_SelectedFileType; }
	QString						getSelectedFileName( void )						{ return m_SelectedFileName; }
	uint64_t					getSelectedFileLen( void )						{ return m_SelectedFileLen; }
	bool						getSelectedFileIsShared( void )					{ return m_SelectedFileIsShared; }
	bool						getSelectedFileIsInLibrary( void )				{ return m_SelectedFileIsInLibrary; }

protected slots:
    void						slotHomeButtonClicked( void ) override;
	void						slotUpDirectoryClicked( void );
	void						slotBrowseButtonClicked( void );
    void                        slotAddAllButtonClicked( void );

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
	
protected:
    virtual void				showEvent( QShowEvent* ev ) override;
    virtual void				hideEvent( QHideEvent* ev ) override;

    virtual void				callbackToGuiFileList( FileInfo& fileInfo ) override;
	virtual void				callbackToGuiFileListCompleted( void ) override;

	void						fromListWidgetRequestFileList( void );
	void						setActionEnable( bool enable );
	void						addFile( FileInfo&	fileInfo );

	void						clearFileList( void );
	void						setDefaultCurrentDir( EFileFilterType eFileFilterType );
	std::string					getDefaultDir( int eFileFilterType );
	void						setCurrentBrowseDir( QString browseDir );

	void						showAddAllToLibrary( bool visible );

	Ui::AppletBrowseFilesUi		ui;

	std::string					m_CurBrowseDirectory;
	bool						m_bFetchInProgress;
	QTimer *					m_WidgetClickEventFixTimer;
	bool						m_IsSelectAFileMode;
	bool						m_FileWasSelected;
	uint8_t						m_SelectedFileType;
	QString						m_SelectedFileName;
	uint64_t					m_SelectedFileLen;
	bool						m_SelectedFileIsShared;
	bool						m_SelectedFileIsInLibrary;

	EFileFilterType				m_eFileFilterType;
	uint8_t						m_FileFilterMask;
	VxTimer						m_ClickToFastTimer;
};



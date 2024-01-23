#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBase.h"
#include <Plugins/FileInfo.h>
#include "ToGuiFileXferInterface.h"

#include "ui_ActivityViewLibraryFiles.h"

#include <PktLib/VxCommon.h>

class FileShareItemWidget;
class VxNetIdent;
class VxSha1Hash;
class FileItemInfo;
class FileListReplySession;
class FileInfo;

class ActivityViewLibraryFiles : public ActivityBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:
	ActivityViewLibraryFiles( AppCommon& app, QWidget* parent = nullptr, bool isSelectAFileMode = false );
	virtual ~ActivityViewLibraryFiles() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

public:
	void						addFile( FileInfo& fileInfo );
	void						setFileFilter( EFileFilterType eFileFilter );
	bool						getWasFileSelected( void )						{ return m_FileWasSelected; }
	uint8_t						getSelectedFileType( void )						{ return m_SelectedFileType; }
	QString						getSelectedFileName( void )						{ return m_SelectedFileName; }
	uint64_t					getSelectedFileLen( void )						{ return m_SelectedFileLen; }
	bool						getSelectedFileIsShared( void )					{ return m_SelectedFileIsShared; }
	bool						getSelectedFileIsInLibrary( void )				{ return m_SelectedFileIsInLibrary; }

private slots:
    void						slotHomeButtonClicked( void ) override;
	void						statusMsg( QString strMsg );
	void						slotRequestFileList( void );
	void						slotApplyFileFilter( unsigned char fileTypeMask );
	void						slotAddFilesButtonClicked( void );

	void						slotListItemClicked( QListWidgetItem* item );
	void						slotListItemDoubleClicked( QListWidgetItem* item );

	void						slotListFileIconClicked( QListWidgetItem* item );
	void						slotListPlayIconClicked( QListWidgetItem* item );
	void						slotListPlayExternIconClicked( QListWidgetItem* item );
	void						slotListLibraryIconClicked( QListWidgetItem* item );
	void						slotListShareFileIconClicked( QListWidgetItem* item );
	void						slotListShredIconClicked( QListWidgetItem* item );

protected:
    virtual void				showEvent( QShowEvent* ev ) override;
    virtual void				hideEvent( QHideEvent* ev ) override;
	virtual void				callbackToGuiFileList( FileInfo& fileInfo ) override;
	virtual void				callbackToGuiFileListCompleted( void ) override;

	FileShareItemWidget*		fileToWidget( FileInfo& fileInfo );
	FileItemInfo*				widgetToFileItemInfo( FileShareItemWidget* item );

	FileShareItemWidget*		findListEntryWidget( FileInfo& fileInfo );
	void						clearFileList( void );
	bool						confirmDeleteFile( QString fileName, bool shredFile );

	//=== vars ===//
	Ui::ViewLibraryFilesDialog	ui;
    EPluginType					m_ePluginType;
	bool						m_IsSelectAFileMode;
	bool						m_FileWasSelected;
	uint8_t						m_SelectedFileType;
	QString						m_SelectedFileName;
	uint64_t					m_SelectedFileLen;
	bool						m_SelectedFileIsShared;
	bool						m_SelectedFileIsInLibrary;
	EFileFilterType				m_eFileFilterType;
	uint8_t						m_FileFilterMask;
};

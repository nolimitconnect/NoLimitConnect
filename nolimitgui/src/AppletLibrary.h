#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"

#include "GuiHelpers.h"
#include "ToGuiFileXferInterface.h"
#include <PktLib/VxCommon.h>
#include <Plugins/FileInfo.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletLibraryUi;
}
QT_END_NAMESPACE

class FileShareItemWidget;
class VxNetIdent;
class VxSha1Hash;
class FileItemInfo;
class FileListReplySession;
class FileInfo;
class QListWidgetItem;

class AppletLibrary : public AppletBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:
	AppletLibrary( AppCommon& app, QWidget* parent, QString launchParam = "" );
	virtual ~AppletLibrary();

    void						addFile( FileInfo& fileInfo );
    void						setFileFilter( EFileFilterType eFileFilter );
    bool						getWasFileSelected( void )						{ return m_FileWasSelected; }
    uint8_t						getSelectedFileType( void )						{ return m_SelectedFileType; }
    QString						getSelectedFileName( void )						{ return m_SelectedFileName; }
    uint64_t					getSelectedFileLen( void )						{ return m_SelectedFileLen; }
    bool						getSelectedFileIsShared( void )					{ return m_SelectedFileIsShared; }
    bool						getSelectedFileIsInLibrary( void )				{ return m_SelectedFileIsInLibrary; }

private slots:

    void						slotRequestFileList( void );
    void						slotApplyFileFilter( EFileFilterType eFileFilter );
    void						slotFileMediaSelected( EMediaFileType mediaFileType );

    void						slotAddFileButtonClicked( void );
    void						slotAddFileLabelClicked( void );
    void						slotBrowseButtonClicked( void );
    void						slotBrowseLabelClicked( void );

    void						slotListItemClicked( QListWidgetItem* item );
    void						slotListItemDoubleClicked( QListWidgetItem* item );

    void						slotListFileIconClicked( QListWidgetItem* item );
    void						slotListPlayIconClicked( QListWidgetItem* item );
    void                        slotListPlayExternIconClicked( QListWidgetItem* item );
    void						slotListLibraryIconClicked( QListWidgetItem* item );
    void						slotListShareFileIconClicked( QListWidgetItem* item );
    void                        slotListAboutFileClicked( QListWidgetItem* item );
    void						slotListShredIconClicked( QListWidgetItem* item );

protected:
    void				        showEvent( QShowEvent* ev ) override;
    void				        hideEvent( QHideEvent* ev ) override;

    void				        callbackToGuiFileList( VxGUID& appInstId, FileInfo& fileInfo ) override;
    void				        callbackToGuiFileListCompleted( VxGUID& appInstId ) override;
    void				        toGuiFileDeleted( QString& fileName ) override;

    FileShareItemWidget*		fileToWidget( FileInfo& fileInfo );
    FileItemInfo*				widgetToFileItemInfo( FileShareItemWidget* item );

    FileShareItemWidget*		findListEntryWidget( FileInfo& fileInfo );
    void						clearFileList( void );

    FileShareItemWidget*        findItemByFileName( QString fileName );

    void                        updateStorageSpace( std::string fileName );

    void                        statusMsg( QString strMsg );

    void                        browseForFile( EMediaFileType mediaFileType );

    void                        wantFileXferCallbacks( bool enable );

    //=== vars ===//
    Ui::AppletLibraryUi&	    ui;
    EPluginType					m_ePluginType;
    bool						m_IsSelectAFileMode{ false };
    bool						m_FileWasSelected{ false };
    uint8_t						m_SelectedFileType{ 0 };
    QString						m_SelectedFileName;
    uint64_t					m_SelectedFileLen{ 0 };
    bool						m_SelectedFileIsShared{ false };
    bool						m_SelectedFileIsInLibrary{ false };
    EFileFilterType				m_eFileFilterType{ eFileFilterAll };
    bool                        m_FileXferCallbacksRequested{ false };
};



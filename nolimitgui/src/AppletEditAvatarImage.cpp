//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletEditAvatarImage.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "AccountMgr.h"
#include "DialogConfirmRemoveMessage.h"
#include "GuiHelpers.h"
#include "MyIconsDefs.h"

#include <P2PEngine/EngineSettings.h>
#include <P2PEngine/P2PEngine.h>
#include <AssetMgr/AssetMgr.h>
#include <ThumbMgr/ThumbMgr.h>
#include <ThumbMgr/ThumbInfo.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <QMessageBox>
#include <QFileDialog>

#include "ui_AppletEditAvatarImage.h"

//============================================================================
AppletEditAvatarImage::AppletEditAvatarImage( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_EDIT_AVATAR_IMAGE, app, parent )
, ui(*(new Ui::AppletEditAvatarImageUi))
, m_ThumbMgr( app.getEngine().getThumbMgr() )
{
    setAppletType( eAppletEditAvatarImage );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
    ui.m_ServiceSettingsWidget->setPluginType( ePluginTypeHostPeerUser );
    ui.m_ServiceSettingsWidget->setViewServiceVisible( false );
    ui.m_ThumbnailEditWidget->setThumnailIsCircular( true );
    ui.m_StatusLabel->setVisible( false );
    //ui.m_DeleteFrame->setVisible( false );

    ui.m_ShredButton->setFixedSize( eButtonSizeSmall );
    ui.m_ShredButton->setIcon( eMyIconShredderNormal );

    m_MyIdent = m_MyApp.getAppGlobals().getMyNetIdent();
    m_strOrigOnlineName = m_MyIdent->getOnlineName();
    m_strOrigMoodMessage = m_MyIdent->getOnlineDescription();
    if( m_MyIdent->getAvatarThumbGuid().isValid() )
    {
        ThumbInfo* thumbAsset = dynamic_cast<ThumbInfo*>(m_ThumbMgr.findAsset( m_MyIdent->getAvatarThumbGuid() ));
        if( thumbAsset )
        {
            ui.m_ThumbnailEditWidget->loadFromAsset( thumbAsset );
        }
    }

    connect( ui.m_ApplyAvatarButton,    SIGNAL(clicked()), this, SLOT(onApplyButClick()) );
    connect( ui.m_RemoveAvatarButton,   SIGNAL(clicked()), this, SLOT(onRemoveButClick()) );
    connect( ui.m_ShredButton,		    SIGNAL(clicked()), this, SLOT(slotShredAsset()) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================    
void AppletEditAvatarImage::onApplyButClick( void )
{
    bool assetExists = ui.m_ThumbnailEditWidget->isAssetIdValid();
    if( assetExists )
    {
        ThumbInfo* existingAsset = dynamic_cast<ThumbInfo*>( m_ThumbMgr.findAsset( ui.m_ThumbnailEditWidget->getAssetId() ) );
        if( existingAsset )
        {
            if( ui.m_ThumbnailEditWidget->updateThumbAsset( *existingAsset ) )
            {
                // setup identity with updated avatar image
                m_MyIdent->setAvatarGuid( existingAsset->getAssetUniqueId(), existingAsset->getModifiedTime() );

                m_Engine.getThumbMgr().fromGuiThumbUpdated( *existingAsset );

                // technically this should have resource lock but because there is no damage if read wrong by another thread during write it is ok to set directly
                m_Engine.getMyNetIdent()->setAvatarGuid( existingAsset->getAssetUniqueId(), existingAsset->getModifiedTime() );

                // notify others of change to identity
                m_MyApp.updateMyIdent( m_MyIdent );
                emit signalAvatarImageChanged( existingAsset );

                QString msgText = QObject::tr( "Applied Avatar Image Changes " );
                QMessageBox::information( this, QObject::tr( "Applied Avatar Image Success" ), msgText );
            }
        }
        else
        {
            assetExists = false;
        }
    }
    
    if( !assetExists && ui.m_ThumbnailEditWidget->getIsUserPickedImage()  )
    {
        ThumbInfo thumbInfo;
        if( ui.m_ThumbnailEditWidget->generateThumbAsset( thumbInfo ) )
        {
            // setup identity with new avatar image
            m_MyIdent->setAvatarGuid( thumbInfo.getAssetUniqueId(), thumbInfo.getModifiedTime() );

            m_Engine.getThumbMgr().fromGuiThumbCreated( thumbInfo );

            // technically this should have resource lock but because there is no damage if read wrong by another thread during write it is ok to set directly
            m_Engine.getMyNetIdent()->setAvatarGuid( thumbInfo.getAssetUniqueId(), thumbInfo.getModifiedTime() );

            // notify others of change to identity
            m_MyApp.updateMyIdent( m_MyIdent );
            emit signalAvatarImageChanged( &thumbInfo );

            QString msgText = QObject::tr( "Applied Avatar Image Changes " );
            QMessageBox::information( this, QObject::tr( "Applied Avatar Image Success" ), msgText );
        }
    }

    closeApplet();
}

//============================================================================    
void AppletEditAvatarImage::onRemoveButClick( void )
{
    ThumbInfo* assetInfo{ nullptr };
    bool assetExists = ui.m_ThumbnailEditWidget->isAssetIdValid();
    if( assetExists )
    {
        assetInfo = dynamic_cast<ThumbInfo*>(m_ThumbMgr.findAsset( ui.m_ThumbnailEditWidget->getAssetId() ));
    }

    if( assetExists && assetInfo->isChatFaceAsset() )
    {
        QString msgText = QObject::tr( "Cannot Delete Avatar Image" );
        QMessageBox::information( this, QObject::tr( "Cannot remove image that is a emoticon" ), msgText );  
        return;
    }

    QString removeConfirmmsgText = QObject::tr( "Are you sure you want to remove your avatar image?" );
    if( QMessageBox::Yes == QMessageBox::question( this, QObject::tr( "Remove Avatar Image" ), removeConfirmmsgText ) )
    {
        removeAvatarImage();

        QString msgText = QObject::tr( "Remove Avatar Image Success" );
        QMessageBox::information( this, QObject::tr( "Remove Avatar Image" ), msgText );  
        closeApplet();
    }
    else
    {
        QString msgText = QObject::tr( "Remove Avatar Image canceled " );
        QMessageBox::information( this, msgText, msgText );
    }
}

//============================================================================
void AppletEditAvatarImage::removeAvatarImage( void )
{
    // setup identity with null avatar image
    VxGUID nullGuid;
    m_MyIdent->setAvatarGuid( nullGuid, 0 );

    // technically this should have resource lock but because there is no damage if read wrong by another thread during write it is ok to set directly
    m_Engine.getMyNetIdent()->setAvatarGuid( nullGuid, 0 );

    // notify others of change to identity
    m_MyApp.updateMyIdent( m_MyIdent );
    emit signalAvatarImageRemoved();
}

//============================================================================
void AppletEditAvatarImage::slotShredAsset( void )
{
    ThumbInfo* assetInfo{ nullptr };
    bool assetExists = ui.m_ThumbnailEditWidget->isAssetIdValid();
    if( assetExists )
    {
        assetInfo = dynamic_cast<ThumbInfo*>(m_ThumbMgr.findAsset( ui.m_ThumbnailEditWidget->getAssetId() ));
    }

    if( assetInfo && assetInfo->isFileAsset() )
    {
	    DialogConfirmRemoveMessage dlg( *assetInfo, true, this );
	    dlg.exec();
	    EAssetAction removeAssetAction = dlg.getAssetActionResult();
	    if( eAssetActionShreadFile == removeAssetAction )
	    {
            removeAvatarImage();
;
            ui.m_ThumbnailEditWidget->clearAssetId();
		    m_MyApp.playSound( eSndDefPaperShredder );
		    m_Engine.fromGuiAssetAction(  eAssetActionShreadFile, *assetInfo, 0 );
            m_ThumbMgr.removeAsset( assetInfo->getThumbId() );
	    }
    }
    else
    {
        QString msgText = QObject::tr( "Cannot shred image " );
        QMessageBox::information( this, msgText, msgText );
    }
}
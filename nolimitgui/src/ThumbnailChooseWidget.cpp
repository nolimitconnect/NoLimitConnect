//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "ThumbnailChooseWidget.h"
#include "AppletChooseThumbnail.h"
#include "AppletMgr.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "ThumbnailEditWidget.h"

#include <P2PEngine/P2PEngine.h>
#include <AssetMgr/AssetMgr.h>
#include <AssetMgr/AssetInfo.h>
#include <ThumbMgr/ThumbMgr.h>
#include <ThumbMgr/ThumbInfo.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>

#include <QMessageBox>
#include <QUuid>
#include <QFileDialog>
#include <QResizeEvent>
#include <QMessageBox>
#include <QPainter>

#include "ui_ThumbnailChooseWidget.h"

ThumbnailViewWidget*        ThumbnailChooseWidget::getThumbnailViewWidget()                        { return ui.m_ThumbnailViewWidget; }

void                        ThumbnailChooseWidget::setIsUserPickedImage( bool userPicked )         { ui.m_ThumbnailViewWidget->setIsUserPickedImage( userPicked ); }
bool                        ThumbnailChooseWidget::getIsUserPickedImage( void )                    { return ui.m_ThumbnailViewWidget->getIsUserPickedImage(); }
bool                        ThumbnailChooseWidget::saveToPngFile( QString& fileName )              { return ui.m_ThumbnailViewWidget->saveToPngFile( fileName ); }


//============================================================================
ThumbnailChooseWidget::ThumbnailChooseWidget( QWidget* parent )
    : QLabel( parent )
    , ui(*(new Ui::ThumnailChooseWidgetUi))
    , m_MyApp( GetAppInstance() )
    , m_ThumbMgr( m_MyApp.getEngine().getThumbMgr() )
{
    m_ParentApplet = GuiHelpers::findParentApplet( parent );
    ui.setupUi( this );
    QSize frameSize( GuiParams::getThumbnailSize().width() + 20, GuiParams::getThumbnailSize().height() + 20 );
    ui.m_ThumbnailFrame->setFixedSize( frameSize );
    setFixedHeight( GuiParams::getThumbnailSize().height() + 30 );

    connect( ui.m_ChooseThumbButton, SIGNAL(clicked()), this, SLOT(slotChooseThumb() ) );
}

//============================================================================
void ThumbnailChooseWidget::slotChooseThumb()
{
    AppletChooseThumbnail* galleryThumb = dynamic_cast< AppletChooseThumbnail * >( m_MyApp.getAppletMgr().launchApplet( eAppletChooseThumbnail, m_ParentApplet ) );
    if( galleryThumb )
    {
        connect( galleryThumb, SIGNAL(signalThumbSelected( AppletBase*, ThumbnailEditWidget* ) ), this, SLOT(slotThumbSelected( AppletBase*, ThumbnailEditWidget* ) ) );
    }
}

//============================================================================
bool ThumbnailChooseWidget::loadFromAsset( ThumbInfo* thumbAsset )
{
    bool loadOk = false;
    if( thumbAsset )
    {
        ui.m_ThumbnailViewWidget->setThumnailIsCircular( getThumbnailIsCircular() );
        loadOk = ui.m_ThumbnailViewWidget->loadFromFile( thumbAsset->getAssetNameAndPath().c_str() );
        if( loadOk )
        {
            setAssetId( thumbAsset->getAssetUniqueId(), getThumbnailIsCircular() );
        }
    }

    return loadOk;
}

//============================================================================
void ThumbnailChooseWidget::slotThumbSelected( AppletBase * thumbGallery, ThumbnailEditWidget * thumb )
{
    if( thumbGallery && thumb )
    {
        VxGUID assetGuid = thumb->updateAndGetThumbnailId();
        ThumbInfo* thumbAsset = dynamic_cast<ThumbInfo*>(m_MyApp.getEngine().getThumbMgr().findAsset( assetGuid ));
        if( thumbAsset )
        {
            if( loadFromAsset( thumbAsset ) )
            {
                setAssetId( assetGuid, thumb->getThumbnailIsCircular() );
                emit signalThumbnailAssetChanged( thumbAsset );
            }
        }

        /*
        disconnect( thumbGallery, SIGNAL(signalThumbSelected( AppletBase *, ThumbnailViewWidget * ) ), this, SLOT(slotThumbSelected( AppletBase *, ThumbnailViewWidget * ) ) );
        thumbGallery->closeApplet();
        */
    }
}

//============================================================================
bool ThumbnailChooseWidget::loadThumbnail( VxGUID& assetId, bool isCircle )
{
    bool result = false;
    setThumnailIsCircular( isCircle );
    if( assetId.isVxGUIDValid() )
    {
        AssetBaseInfo* thumbAsset = m_ThumbMgr.findAsset( assetId );
        if( thumbAsset )
        {
            if( loadFromAsset( dynamic_cast<ThumbInfo*>(thumbAsset) ) )
            {
                setAssetId( assetId, isCircle );
                result = true;
            }
        }
    }

    return result;
}

//============================================================================
VxGUID ThumbnailChooseWidget::updateAndGetThumbnailId( void )
{
    bool assetExists = isAssetIdValid();
    if( assetExists )
    {
        AssetBaseInfo* existingAsset = m_ThumbMgr.findAsset( getAssetId() );
        if( existingAsset )
        {
            return existingAsset->getAssetUniqueId();
        }
        else
        {
            assetExists = false;
        }
    }

    VxGUID nullGuid;
    return nullGuid;
}

//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ThumbnailViewWidget.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>
#include <ThumbMgr/ThumbMgr.h>
#include <ThumbMgr/ThumbInfo.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QResizeEvent>
#include <QUuid>

//============================================================================
ThumbnailViewWidget::ThumbnailViewWidget( QWidget* parent )
    : QLabel( parent )
    , m_MyApp( GetAppInstance() )
{
}

//============================================================================
void ThumbnailViewWidget::mousePressEvent( QMouseEvent * event )
{
    QWidget::mousePressEvent( event );
    emit clicked();
    emit signalImageClicked( this );
}

//============================================================================
bool ThumbnailViewWidget::loadFromAsset( ThumbInfo* asset )
{
    if( asset && ( asset->isPhotoAsset() || asset->isThumbAsset() ) )
    {
        setThumbnailId( asset->getAssetUniqueId() );
        return loadFromFile( asset->getAssetNameAndPath().c_str() );
    }

    return false;
}

//============================================================================
bool ThumbnailViewWidget::loadFromThumbId( VxGUID& thumbId )
{
    if( !thumbId.isVxGUIDValid() )
    {
        return false;
    }

    QString fileName = GuiHelpers::generateThumbFileName( thumbId );
    if( !fileName.isEmpty() && VxFileUtil::fileExists( fileName.toUtf8().constData() ) )
    {
        return loadFromFile( fileName );
    }

    return false;
}

//============================================================================
void ThumbnailViewWidget::updateAssetImage( ThumbInfo* thumbAsset )
{
    if( !loadFromAsset( thumbAsset ) )
    {
        LogMsg( LOG_ERROR, "ThumbnailViewWidget::updateAssetImage failed load %s", thumbAsset->getAssetNameAndPath().c_str() );
    }
}

//============================================================================
bool ThumbnailViewWidget::loadFromFile( QString fileName )
{
    QPixmap pixmap;
    bool result = pixmap.load( fileName );
    if( result )
    {
        cropAndUpdateImage( pixmap );
    }
    else
    {
        QString msgText = QObject::tr( "Failed to create pixmap " ) + fileName;
        QMessageBox::warning( this, QObject::tr( "Error loading from file" ), msgText );
    }

    return result;
}

//============================================================================
void ThumbnailViewWidget::slotJpgSnapshot( uint8_t* pu8JpgData, uint32_t u32DataLen, int iWidth, int iHeight )
{
    QPixmap bitmap;
    if( bitmap.loadFromData( pu8JpgData, u32DataLen, "JPG" ) )
    {
        cropAndUpdateImage( bitmap );
        setIsUserPickedImage( true );
    }
    else
    {
        QString msgText = QObject::tr( "Failed to read snapshot " );
        QMessageBox::warning( this, QObject::tr( "Error Reading snapshot" ), msgText );
    }
}

//============================================================================
QPixmap ThumbnailViewWidget::makeCircleImage( QPixmap& pixmap )
{
    QPixmap target( pixmap.width(), pixmap.height() );
    target.fill( Qt::transparent );

    QPainter painter( &target );

    // Set clipped region (circle) in the center of the target image
    QRegion clipRegion( QRect( 0, 0, pixmap.width(), pixmap.height() ), QRegion::Ellipse );
    painter.setClipRegion( clipRegion );

    painter.drawPixmap( 0, 0, pixmap );  
    return target;
}

//============================================================================
void ThumbnailViewWidget::cropAndUpdateImage( QPixmap& pixmap )
{
    QSize thumbSize = GuiParams::getThumbnailSize();
    QSize origSize = pixmap.size();

    if( thumbSize == origSize )
    {
        // no need to scale or crop image
        m_ThumbPixmap = pixmap;
        if( m_ThumbnailIsCircular )
        {
            setPixmap( makeCircleImage( m_ThumbPixmap ) );
        }
        else
        {
            setPixmap( m_ThumbPixmap );
        }
    }
    else if( !pixmap.isNull() )
    {
        int minSize = origSize.width() < origSize.height() ? origSize.width() : origSize.height();
        int leftMargin = ( origSize.width() - minSize ) / 2;
        int topMargin = ( origSize.height() - minSize ) / 2;
        QRect rect( leftMargin, topMargin, origSize.width() - leftMargin, origSize.height() - topMargin );
        QPixmap cropped = pixmap.copy( rect );
        QPixmap scaledPixmap = cropped.scaled( GuiParams::getThumbnailSize() );
        if( !scaledPixmap.isNull() )
        {
            m_ThumbPixmap = pixmap;
            if( m_ThumbnailIsCircular )
            {
                setPixmap( makeCircleImage( scaledPixmap ) );
            }
            else
            {
                setPixmap( scaledPixmap );
            }
        }
        else
        {
            QString msgText = QObject::tr( "Failed to scale and crop image " );
            QMessageBox::warning( this, QObject::tr( "Error scaling image." ), msgText );
        }
    }
    else
    {
        QString msgText = QObject::tr( "Null image " );
        QMessageBox::warning( this, QObject::tr( "Null image." ), msgText );
    }
}

//============================================================================
void ThumbnailViewWidget::browseForImage( void )
{
    QString startPath = QDir::current().path();
    std::string lastGalleryPath;
    m_MyApp.getAppSettings().getLastGalleryDir( lastGalleryPath );
    if( ( 0 != lastGalleryPath.length() )
        && ( VxFileUtil::directoryExists( lastGalleryPath.c_str() ) ) )
    {
        startPath = lastGalleryPath.c_str();
    }

    // Get a filename from the file dialog.
    QString filename = QFileDialog::getOpenFileName( this,
                                                     QObject::tr( "Open Image" ),
                                                     startPath,
                                                     SUPPORTED_IMAGE_FILES );
    if( filename.length() > 0 )
    {
        QPixmap oBitmap;
        if( false == oBitmap.load( filename ) )
        {
            QString msgText = QObject::tr( "Failed To Read Image File " ) + filename;
            QMessageBox::critical( this, QObject::tr( "Error Reading Image" ), msgText );
        }
        else
        {
            std::string justFileName;
            VxFileUtil::seperatePathAndFile( filename.toUtf8().constData(), lastGalleryPath, justFileName );
            if( ( 0 != lastGalleryPath.length() )
                && ( VxFileUtil::directoryExists( lastGalleryPath.c_str() ) ) )
            {
                m_MyApp.getAppSettings().setLastGalleryDir( lastGalleryPath );
            }

            cropAndUpdateImage( oBitmap );
            setIsUserPickedImage( true );
        }
    }
}

//============================================================================
bool ThumbnailViewWidget::saveToPngFile( QString pngFileName )
{
    return GuiHelpers::saveToPngFile( m_ThumbPixmap, pngFileName );
}

//============================================================================
void ThumbnailViewWidget::setImage( VxGUID& thumbId, QImage& thumbImage )
{
    if( thumbImage.isNull() || !thumbId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ThumbnailViewWidget::%s invalid param", __func__ );
        return;
    }
    
    m_ThumbPixmap = QPixmap::fromImage( thumbImage );
    if( m_ThumbPixmap.isNull() )
    {
        LogMsg( LOG_ERROR, "ThumbnailViewWidget::%s m_ThumbPixmap isNull", __func__ );
    }
    else
    {
        m_ThumbnailId = thumbId;
        cropAndUpdateImage( m_ThumbPixmap );
    }
}
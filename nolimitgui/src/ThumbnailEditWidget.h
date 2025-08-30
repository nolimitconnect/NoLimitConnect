//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include <QWidget>

#include <CoreLib/VxGUID.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ThumnailEditWidgetUi;
}
QT_END_NAMESPACE

class AppCommon;
class AppletBase;
class IVxVidCap;
class ThumbInfo;  
class ThumbMgr;
class ThumbnailViewWidget;

class ThumbnailEditWidget : public QWidget
{
    Q_OBJECT
public:
    ThumbnailEditWidget( QWidget* parent );
    ~ThumbnailEditWidget() override = default;

    void                        setIsUserPickedImage( bool userPicked );
    bool                        getIsUserPickedImage( void );

    void                        setThumnailIsCircular( bool isCircle );
    bool                        getThumbnailIsCircular( void );

    bool                        loadFromAsset( ThumbInfo* thumbAsset );
    bool                        saveToPngFile( QString& fileName );

    void                        setAssetId( VxGUID& assetGuid )                 { m_AsssetId = assetGuid; }
    VxGUID&                     getAssetId( void )                              { return m_AsssetId; }
    void                        clearAssetId( void );
    bool                        isAssetIdValid( void )                          { return m_AsssetId.isVxGUIDValid(); }

    bool                        generateThumbAsset( ThumbInfo& assetInfo );
    bool                        updateThumbAsset( ThumbInfo& assetInfo );

    bool                        loadThumbnail( VxGUID& assetGuid );
    VxGUID                      updateAndGetThumbnailId( void );

signals:
    void                        signalImageChanged();

protected slots:
    void                        slotEmoticonGalleryClick( void );
    void                        slotThumbGalleryClick( void );
    void                        slotSnapShotButClick( void );
    void                        slotBrowseButClick( void );
//    void                        slotMakeCircleButClick( void );
//    void                        slotUndoCircleClick( void );
    void                        slotThumbSelected( AppletBase * thumbGallery, ThumbnailViewWidget * thumb );
    void                        slotJpgSnapshot( uint8_t* pu8JpgData, uint32_t u32DataLen, int iWidth, int iHeight );
    void                        slotImageSnapshot( QImage snapshotImage );

protected:
    QPixmap                     makeCircleImage( QPixmap& pixmap );

    Ui::ThumnailEditWidgetUi&	ui;
    AppCommon&					m_MyApp;
    ThumbMgr&                   m_ThumbMgr;
    AppletBase*                 m_ParentApplet{ nullptr };
    bool 					    m_CameraSourceAvail{ false };
    bool 					    m_ThumbnailIsCircular = false;

    VxGUID                      m_AsssetId;
    bool                        m_WasModifiedByUser = false;
    uint8_t *					m_pu8BitmapData = nullptr;	// snapshot bitmap
    uint32_t					m_u32BitmapLen = 0;		// bitmap length
};

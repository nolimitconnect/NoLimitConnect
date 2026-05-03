//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include <QLabel>

#include <CoreLib/VxGUID.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ThumnailChooseWidgetUi;
}
QT_END_NAMESPACE

class AppCommon;
class AppletBase;
class QPixmap;
class ThumbInfo;
class ThumbMgr;
class ThumbnailEditWidget;
class ThumbnailViewWidget;

class ThumbnailChooseWidget : public QLabel
{
    Q_OBJECT
public:
    ThumbnailChooseWidget( QWidget* parent );
    virtual ~ThumbnailChooseWidget() override = default;

    ThumbnailViewWidget*        getThumbnailViewWidget();

    void                        setIsUserPickedImage( bool userPicked );
    bool                        getIsUserPickedImage( void );

    void                        setThumnailIsCircular( bool isCircle )          { m_ThumbnailIsCircular = isCircle; }
    bool                        getThumbnailIsCircular( void )                  { return m_ThumbnailIsCircular; }

    bool                        loadFromAsset( ThumbInfo* thumbAsset );
    bool                        saveToPngFile( QString& fileName );

    void                        setAssetId( VxGUID& assetGuid, bool isCircle )  { m_AsssetId = assetGuid; m_ThumbnailIsCircular = isCircle; }
    VxGUID&                     getAssetId( void )                              { return m_AsssetId; }
    void                        clearAssetId( void )                            { m_AsssetId.clearVxGUID(); }
    bool                        isAssetIdValid( void )                          { return m_AsssetId.isValid(); }

    VxGUID&                     getThumbnailId( void )                          { return m_ThumbnailId; }
    VxGUID                      updateAndGetThumbnailId( void );

    bool                        loadThumbnail( VxGUID& assetGuid, bool isCircle );

signals:
    void                        signalThumbnailAssetChanged( ThumbInfo* thumbAsset );

protected slots:
    void                        slotChooseThumb();
    void                        slotThumbSelected( AppletBase * thumbGallery, ThumbnailEditWidget * thumb );

protected:
    Ui::ThumnailChooseWidgetUi&	ui;
    AppCommon&					m_MyApp;
    ThumbMgr&                   m_ThumbMgr;
    AppletBase*                 m_ParentApplet{ nullptr };
    VxGUID                      m_AsssetId;
    bool                        m_ThumbnailIsCircular{ false };
    VxGUID                      m_ThumbnailId;

    bool                        m_bUserPickedImage{ false };
};

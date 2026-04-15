//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InputFaceWidget.h"
#include "AppCommon.h"
#include "GuiParams.h"
#include "InputClientBaseCallback.h"
#include "VxLabel.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include "ui_InputFaceWidget.h"

#include <QResizeEvent>
#include <QShowEvent>
#include <cmath>

namespace
{
    constexpr int kFaceCount        = 50;
    constexpr int kFaceTileMinSize  = 24;
    constexpr int kFaceTileMaxSize  = 80;

    // Returns the largest tile size for which at least N tiles fit in WxH.
    double optimal_size( double W, double H, int N )
    {
        int i_min = 1, j_min = 1;
        for( int i = std::max( 1, (int)std::round( std::sqrt( (double)N * W / H ) ) ); ; i++ )
        {
            if( i * (int)std::floor( H * (double)i / W ) >= N ) { i_min = i; break; }
        }
        for( int j = std::max( 1, (int)std::round( std::sqrt( (double)N * H / W ) ) ); ; j++ )
        {
            if( (int)std::floor( W * (double)j / H ) * j >= N ) { j_min = j; break; }
        }
        return std::max( W / i_min, H / j_min );
    }
}

//============================================================================
InputFaceWidget::InputFaceWidget( QWidget* parent )
: InputBaseWidget( GetAppInstance(), parent )
, ui( *(new Ui::InputFaceWidgetClass) )
{
    m_AssetInfo.setAssetType( eAssetTypeChatFace );
    ui.setupUi( this );
    ui.m_CancelFaceButton->setSquareButtonSize( eButtonSizeTiny );
    ui.m_CancelFaceButton->setIcons( eMyIconCancelNormal );
    connect( ui.m_CancelFaceButton, SIGNAL(clicked()), this, SLOT(slotCancelFaceButtonClicked()) );

    // Unlock the fixed-height m_MainFrame so it can expand to fill available space.
    ui.m_MainFrame->setMinimumHeight( 0 );
    ui.m_MainFrame->setMaximumHeight( QWIDGETSIZE_MAX );
    ui.m_MainFrame->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // Hide the legacy static face selector (contains the 32 hardcoded VxLabel entries).
    ui.m_SelectFaceFrame->setVisible( false );

    // Create the tile canvas.  Children are positioned absolutely by repositionFaceTiles().
    m_FaceContentWidget = new QWidget( ui.m_MainFrame );
    m_FaceContentWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // Zero out the margin/spacing so the tile canvas fills the whole frame.
    ui.verticalLayout->setContentsMargins( 0, 0, 0, 0 );
    ui.verticalLayout->setSpacing( 0 );

    // Insert canvas before the cancel button row.  Stretch = 1 gives it all the remaining height.
    ui.verticalLayout->insertWidget( 0, m_FaceContentWidget, 1 );

    // Create all 50 face labels in one unified loop — same code path for every face.
    for( int i = 1; i <= kFaceCount; i++ )
    {
        QString resPath = ( i > 9 )
            ? QString( ":/AppRes/Resources/emoj%1.svg" ).arg( i )
            : QString( ":/AppRes/Resources/emoj0%1.svg" ).arg( i );

        VxLabel* faceLabel = new VxLabel( m_FaceContentWidget );
        faceLabel->setAlignment( Qt::AlignCenter );
        connect( faceLabel, &VxLabel::clicked, this, [this, i]() { faceLabelClicked( i ); } );
        m_FaceList.append( faceLabel );
        m_FaceResourcePaths.append( resPath );

        QPixmap facePixmap( resPath );
        if( facePixmap.isNull() )
        {
            LogMsg( LOG_ERROR, "InputFaceWidget::InputFaceWidget invalid pixmap for %s",
                    resPath.toUtf8().constData() );
        }

        m_FacePixmaps.append( facePixmap );
        m_ScaledFacePixmaps.append( QPixmap() );
    }
}

//============================================================================
void InputFaceWidget::resizeEvent( QResizeEvent* ev )
{
    QWidget::resizeEvent( ev );
    repositionFaceTiles();
}

//============================================================================
void InputFaceWidget::showEvent( QShowEvent* ev )
{
    QWidget::showEvent( ev );
    repositionFaceTiles( true );
}

//============================================================================
void InputFaceWidget::refreshScaledPixmaps( int tileSize )
{
    if( tileSize == m_LastTileSize )
        return;

    const QSize pixSize( std::max( 1, tileSize - 2 ), std::max( 1, tileSize - 2 ) );
    for( int i = 0; i < m_FacePixmaps.size(); i++ )
    {
        const QPixmap& pix = m_FacePixmaps[i];
        if( pix.isNull() )
        {
            m_ScaledFacePixmaps[i] = QPixmap();
            continue;
        }

        m_ScaledFacePixmaps[i] = pix.scaled( pixSize, Qt::KeepAspectRatio, Qt::FastTransformation );
    }

    m_LastTileSize = tileSize;
}

//============================================================================
void InputFaceWidget::repositionFaceTiles( bool force )
{
    const int n = m_FaceList.size();
    if( n == 0 || !m_FaceContentWidget )
        return;

    const QSize areaSize = m_FaceContentWidget->size();
    if( !force && areaSize == m_LastFaceAreaSize )
        return;

    const int W = areaSize.width();
    const int H = areaSize.height();
    if( W <= 0 || H <= 0 )
        return;

    m_LastFaceAreaSize = areaSize;

    int tileSize = (int)optimal_size( (double)W, (double)H, n );
    tileSize = std::max( kFaceTileMinSize, std::min( kFaceTileMaxSize, tileSize ) );

    int cols = W / tileSize;
    if( cols <= 0 )
        cols = 1;

    refreshScaledPixmaps( tileSize );

    for( int i = 0; i < n; i++ )
    {
        VxLabel* label = m_FaceList[i];
        label->setFixedSize( tileSize, tileSize );
        label->move( ( i % cols ) * tileSize, ( i / cols ) * tileSize );
        label->show();

        const QPixmap& pix = m_ScaledFacePixmaps[i];
        if( !pix.isNull() )
            label->setPixmap( pix );
        else
            LogMsg( LOG_ERROR, "InputFaceWidget::repositionFaceTiles invalid pixmap for %s",
                    m_FaceResourcePaths[i].toUtf8().constData() );
    }
}

//============================================================================
void InputFaceWidget::slotCancelFaceButtonClicked( void )
{
    emit signalInputCompleted();
}

//============================================================================
void InputFaceWidget::faceLabelClicked( int faceNum )
{
    char assetBuf[128];
    if( faceNum > 9 )
        sprintf( assetBuf, "emoj%d", faceNum );
    else
        sprintf( assetBuf, "emoj0%d", faceNum );

    if( fillAssetBaseInfo( true ) )
    {
        m_AssetInfo.setAssetName( assetBuf );
        m_AssetInfo.setAssetLength( 0 );

        if( addOptionalComment() )
        {
            m_ClientCallback->handleAssetAction(
                m_IsPersonalRecorder ? eAssetActionAddToAssetMgr : eAssetActionAddAssetAndSend,
                m_AssetInfo );
        }
    }

    emit signalInputCompleted();
}

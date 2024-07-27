//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "RenderGlWidget.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "MyIcons.h"

#include "RenderGlOffScreenSurface.h"

#include <GuiInterface/NlcRenderFrame.h>

#include <CoreLib/VxTimeUtil.h>

#include <QDebug>
#include <QTimer>
#include <QOpenGLFramebufferObjectFormat>
#include <QKeyEvent>
#include <QtGui/qopenglfunctions.h>
#include <QPainter>

#include <time.h>

# if defined(TARGET_OS_APPLE)
#  include <OpenGLES/ES2/gl.h>
# elif defined(TARGET_OS_ANDROID)
#  include <GLES2/gl2.h>
#  include <GLES2/gl2ext.h>
#  include <GLES3/gl3.h>
# elif defined(TARGET_OS_LINUX)
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glext.h>
# elif defined(TARGET_OS_WINDOWS)
#  include <GL/gl.h>
#  include <GL/glu.h>
# endif // defined(TARGET_OS_ANDROID)


const int RESIZE_WINDOW_COMPLETED_TIMEOUT = 500;

//============================================================================
RenderGlWidget::RenderGlWidget(QWidget* parent)
: RenderGlLogic( *this, parent)
, m_MyApp( GetAppInstance() )
, m_QtToPlayerNlc( m_MyApp )
, m_ScreenSize( 1, 1 )
, m_ResizingWindowSize( 1, 1 )
, m_ResizingTimer( new QTimer( this ) )
{
    memset( m_viewPort, 0, sizeof( m_viewPort ) );
    memset( m_TextureIds, 0, sizeof( m_TextureIds ) );
    memset( m_TexSize, 0, sizeof( m_TexSize ) );

	connect( m_ResizingTimer, SIGNAL( timeout() ), this, SLOT( slotResizeWindowTimeout() ) );
    connect( this, SIGNAL(signalFrameRendered()), this, SLOT(slotOnFrameRendered()) );
    initRenderGlContext();
}

//============================================================================
RenderGlWidget::~RenderGlWidget()
{
    m_QtToPlayerNlc.fromGuiCloseEvent();
    aboutToDestroy();
}

//============================================================================
void RenderGlWidget::takeSnapshot()
{
    if( getIsRenderInitialized() )
    {
        lockRenderer();
        QImage frameImage = getLastRenderedImage();
        if( !frameImage.isNull() )
        {
#ifdef TARGET_OS_WINDOWS
            frameImage.save( QString( "F:\\NoLimitConnect_Image.png" ) );
#else
            frameImage.save( QString( "~/NoLimitConnect_Image.png" ) );
#endif
        }

         unlockRenderer();
    }
}

//============================================================================
void RenderGlWidget::slotOnFrameRendered()
{
   update();
}

//============================================================================
void RenderGlWidget::paintEvent( QPaintEvent * ev )
{
    QWidget::paintEvent( ev );

    if( getIsRenderInitialized() )
    {
        QImage frameImage = getLastRenderedImage();
        if( !frameImage.isNull() )
        {
            QPainter painter;
            painter.begin( this );
            painter.setRenderHint( QPainter::Antialiasing );
            painter.drawImage( 0, 0, frameImage );
            painter.end();
        }
    }
}


//============================================================================
void RenderGlWidget::handleGlResize( int width, int height )
{
    LogMsg( LOG_DEBUG, "handleGlResize x(%d) y(%d)", width, height );

    QSize screenSize = QSize( width, height );
    if( screenSize != m_ScreenSize )
    {
        m_ScreenSize = QSize( width, height );
        m_ResizingWindowSize = m_ScreenSize;

        setSurfaceSize( m_ScreenSize );
        if( !m_IsResizing )
        {
            m_IsResizing = true;
            onResizeBegin( m_ResizingWindowSize );
        }

        onResizeEvent( m_ResizingWindowSize );
        m_ResizingTimer->stop();
        m_ResizingTimer->setSingleShot( true );
        m_ResizingTimer->start( RESIZE_WINDOW_COMPLETED_TIMEOUT );
    }
}

//============================================================================
void RenderGlWidget::showEvent( QShowEvent* ev )
{
    QWidget::showEvent( ev );
    setRenderWindowVisible( true );
}

//============================================================================
void RenderGlWidget::hideEvent( QHideEvent* ev )
{
    setRenderWindowVisible( false );
    QWidget::hideEvent( ev );
}

//============================================================================
void RenderGlWidget::closeEvent( QCloseEvent * ev )
{
    m_QtToPlayerNlc.fromGuiCloseEvent();
    setRenderPlayerNlcThreadShouldRun(false);
    aboutToDestroy();
    QWidget::closeEvent( ev );
}

//============================================================================
void RenderGlWidget::resizeEvent( QResizeEvent* ev )
{
    QWidget::resizeEvent( ev );

    handleGlResize( ev->size().width(), ev->size().height() );
}

//============================================================================
void RenderGlWidget::onResizeBegin( QSize& newSize )
{
	m_QtToPlayerNlc.fromGuiResizeBegin( newSize.width(), newSize.height() );
}

//============================================================================
void RenderGlWidget::onResizeEvent( QSize& newSize )
{
	m_QtToPlayerNlc.fromGuiResizeEvent( newSize.width(), newSize.height() );
}

//============================================================================
void RenderGlWidget::onResizeEnd( QSize& newSize )
{
	m_QtToPlayerNlc.fromGuiResizeEnd( newSize.width(), newSize.height() );
}

//============================================================================
void RenderGlWidget::onModuleState( EAppModule moduleNum, EModuleState moduleState )
{
	if( ( moduleNum == eAppModulePlayerNlc ) && ( moduleState == eModuleStateInitialized ) )
	{
		// send a resize message so kodi will resize to fit window
		m_QtToPlayerNlc.fromGuiResizeEnd( m_ScreenSize.width(), m_ScreenSize.height() );
	}
}

//============================================================================
void RenderGlWidget::keyPressEvent( QKeyEvent * ev )
{
    if( ev->isAutoRepeat() )
        return;

    //if( ! m_QtToPlayerNlc.fromGuiKeyPressEvent( ev->key() ) )
    //{
    //    QWidget::keyPressEvent( ev );
    //}
}

//============================================================================
void RenderGlWidget::keyReleaseEvent( QKeyEvent * ev )
{
    if( ev->isAutoRepeat() )
        return;

    //if( !m_QtToPlayerNlc.fromGuiKeyReleaseEvent( ev->key() ) )
    //{
    //    QWidget::keyReleaseEvent( ev );
    //}
}

//============================================================================
void RenderGlWidget::mousePressEvent( QMouseEvent * ev )
{
    // takeSnapshot();
    //if( !m_QtToPlayerNlc.fromGuiMousePressEvent( ev->position().x(), ev->position().y(), ev->button() ) )
    //{
    //    QWidget::mousePressEvent( ev );
    //}

    if( ev->button() == Qt::LeftButton )
    {
        emit signalLeftMouseButtonClick();
    }
}

//============================================================================
void RenderGlWidget::mouseReleaseEvent( QMouseEvent * ev )
{
    //if( !m_QtToPlayerNlc.fromGuiMouseReleaseEvent( ev->position().x(), ev->position().y(), ev->button() ) )
    //{
    //    QWidget::mouseReleaseEvent( ev );
    //}
}

//============================================================================
void RenderGlWidget::mouseMoveEvent( QMouseEvent * ev )
{
    //if( !m_QtToPlayerNlc.fromGuiMouseMoveEvent( ev->position().x(), ev->position().y() ) )
    //{
    //    QWidget::mouseMoveEvent( ev );
    //}
}

//============================================================================
void RenderGlWidget::slotResizeWindowTimeout()
{
	if( m_IsResizing )
	{
		m_IsResizing = false;
		onResizeEnd( m_ResizingWindowSize );
	}
}

//============================================================================
void RenderGlWidget::showAppIcon( void )
{
    QPixmap pixmap = m_MyApp.getMyIcons().getIconPixmap( eMyIconApp, geometry().size() );
    QImage image = pixmap.toImage();
    setLastRenderedImage( image );
    update();
}

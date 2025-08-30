//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxSpinProgress.h"

#include "GuiParams.h"

#include <CoreLib/VxDebug.h>

#include <QTimer>
#include <QPixmap>

//============================================================================
VxSpinProgress::VxSpinProgress(QWidget* parent, Qt::WindowFlags f) 
: QLabel(parent,f) 
, m_AnimationTimer( new QTimer(this) )
{
	initQSpinProgress();
}

//============================================================================
VxSpinProgress::VxSpinProgress(const QString &text, QWidget* parent, Qt::WindowFlags f) 
: QLabel(text,parent,f) 
, m_AnimationTimer( new QTimer(this) )
{
	initQSpinProgress();
}

//============================================================================
void VxSpinProgress::initQSpinProgress( void )
{
    setObjectName( "VxSpinProgress" );

	setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
	setSquareButtonSize( eButtonSizeSmall );

	m_AnimationTimer->setInterval( 200 );
	connect( m_AnimationTimer, SIGNAL(timeout()), this, SLOT(slotAnimationTickTock()) );
}

//============================================================================
void VxSpinProgress::setSquareButtonSize( EButtonSize buttonSize )
{
	m_SpinnerSize = GuiParams::getButtonSize( buttonSize );
	setFixedSize( m_SpinnerSize );
}

//============================================================================
QSize VxSpinProgress::sizeHint() const
{
	if( m_SpinnerSize.width() )
	{
		return m_SpinnerSize;
	}

	return QSize(32,32);
}

//============================================================================
void VxSpinProgress::setValue( int progressPercent )
{
	m_PercentText = QString("%1").arg(progressPercent);
	//setText( m_PercentText );
	if( progressPercent >= 100 )
	{
		m_AnimationTimer->stop();
	}
}

//============================================================================
void VxSpinProgress::setImage( QString resourceUrl )
{
	QImage picBitmap( resourceUrl ); 
	m_ProgressPixmap = picBitmap.scaled( m_SpinnerSize, Qt::KeepAspectRatio );
	m_AnimationTimer->start();
}

//============================================================================
void VxSpinProgress::mousePressEvent(QMouseEvent * event)
{
	QWidget::mousePressEvent(event);
	emit clicked();
}

//============================================================================
void VxSpinProgress::showEvent(QShowEvent* showEvent)
{
	m_AnimationTimer->start();
}

//============================================================================
void VxSpinProgress::hideEvent(QHideEvent* hideEvent)
{
	m_AnimationTimer->stop();
}

//============================================================================
void VxSpinProgress::slotAnimationTickTock( void )
{
	if( m_ProgressPixmap.isNull() 
		|| ( ( this->width() != m_ProgressPixmap.width() ) && ( this->height() != m_ProgressPixmap.height() ) ) )
	{
		updateImage();
	}

	if( m_ProgressPixmap.isNull() 
		|| ( ( this->width() != m_ProgressPixmap.width() ) && ( this->height() != m_ProgressPixmap.height() ) ) )
	{
		return;
	}

	m_Angle = ( m_Angle + 6 ) % 360;

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	QTransform rm;
#else
	QMatrix rm;
#endif // QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	rm.rotate( m_Angle );
	QPixmap pixmap = QPixmap::fromImage( m_ProgressPixmap ).transformed(rm);
	setPixmap( pixmap );
	update();
}

//============================================================================
void VxSpinProgress::updateImage( void )
{
	QSize screenSize( width(), height() );
	QImage picBitmap( ":/AppRes/Resources/spinner-clockwise.svg" ); 
	m_ProgressPixmap = picBitmap.scaled(screenSize, Qt::KeepAspectRatio);
}

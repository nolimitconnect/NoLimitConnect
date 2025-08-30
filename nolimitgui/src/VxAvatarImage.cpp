//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxAvatarImage.h"

#include "AppCommon.h"
#include "GuiParams.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

#include <QTimer>
#include <QPainter>
#include <QPixmap>
#include <QImage>

//============================================================================
VxAvatarImage::VxAvatarImage(QWidget* parent, Qt::WindowFlags f) 
: QLabel(parent,f) 
{
	initQAvatarImage();
}

//============================================================================
VxAvatarImage::VxAvatarImage(const QString &text, QWidget* parent, Qt::WindowFlags f) 
: QLabel(text,parent,f) 
{
    setObjectName( "VxAvatarImage" );
	initQAvatarImage();
}

//============================================================================
void VxAvatarImage::initQAvatarImage( void )
{
	setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
	setIcon( eMyIconAvatarImage );
}

//============================================================================
void VxAvatarImage::setIcon( enum EMyIcons myIcon )
{
	if( !m_HasThumbImage )
	{
		QColor iconColor = GetAppInstance().getAppTheme().getColor( eButtonForegroundNormal );
		QPixmap avatarPixmap = GetAppInstance().getMyIcons().getIconPixmap( myIcon, GuiParams::getButtonSize( eButtonSizeLarge ), iconColor );
		if( !avatarPixmap.isNull() )
		{
			m_AvatarImage = avatarPixmap.toImage();
			update();
		}
	}
}

//============================================================================
void VxAvatarImage::setImage( const char* resourceUrl )
{
	if( !m_HasThumbImage )
	{
		m_AvatarImage = QImage( resourceUrl );
		update();
	}
}

//============================================================================
void VxAvatarImage::setImage( QImage& avatarImage )
{
	if( !avatarImage.isNull() )
	{
		m_AvatarImage = avatarImage;
		update();
	}
}

//============================================================================
QSize VxAvatarImage::sizeHint() const
{
	return QSize(32,32);
}

//============================================================================
void VxAvatarImage::mousePressEvent( QMouseEvent * ev )
{
	QWidget::mousePressEvent( ev );
	emit clicked();
}

//============================================================================
void VxAvatarImage::paintEvent( QPaintEvent* ev )
{
	if( m_AvatarImage.isNull() )
	{
		return;
	}

	QPainter painter( this );

	const int IMAGE_PADDING = 1;
	QRect drawRect( IMAGE_PADDING, IMAGE_PADDING, this->width() - ( IMAGE_PADDING * 2 ), this->height() - ( IMAGE_PADDING * 2 ) );
	if( drawRect.width() < 8 )
	{
		LogMsg( LOG_ERROR, "VxAvatarImage::paintEvent invalid drawRect.width()  %d ", drawRect.width() );
		return;
	}

	QImage resizedImage = m_AvatarImage.scaled( drawRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
	QPixmap pixmap = QPixmap::fromImage( resizedImage );

	painter.drawPixmap( drawRect, QPixmap::fromImage( resizedImage ) );
}
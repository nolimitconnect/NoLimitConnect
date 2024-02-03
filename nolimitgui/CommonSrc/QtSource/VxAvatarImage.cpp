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

#include <CoreLib/VxDebug.h>

#include <QTimer>
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
	setImage( ":/AppRes/Resources/avatar.svg" );
}

//============================================================================
void VxAvatarImage::setImage( const char* resourceUrl )
{
	if( !m_HasThumbImage )
	{
		setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
		QImage picImage( resourceUrl );
		QPixmap avatarPixmap = QPixmap::fromImage( picImage ).scaled(geometry().size());
		if( !avatarPixmap.isNull() )
		{
			setPixmap( avatarPixmap );
		}
	}
}

//============================================================================
void VxAvatarImage::setImage( QImage& avatarImage )
{
	if( !avatarImage.isNull() )
	{
		QPixmap avatarPixmap = QPixmap::fromImage( avatarImage ).scaled(geometry().size());
		if( !avatarPixmap.isNull() )
		{
			m_HasThumbImage = true;
			setPixmap( avatarPixmap );
		}
	}
}

//============================================================================
QSize VxAvatarImage::sizeHint() const
{
	return QSize(32,32);
}

//============================================================================
void VxAvatarImage::mousePressEvent(QMouseEvent * event)
{
	QWidget::mousePressEvent(event);
	emit clicked();
}

////============================================================================
//void VxAvatarImage::updateImage( void )
//{
//	QSize screenSize( width(), height() );
//	QImage picBitmap( ":/AppRes/Resources/spinner-clockwise.svg" ); 
//	m_ProgressPixmap = picBitmap.scaled(screenSize, Qt::KeepAspectRatio);
//}

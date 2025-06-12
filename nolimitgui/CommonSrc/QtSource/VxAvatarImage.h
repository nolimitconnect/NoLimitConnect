#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "MyIconsDefs.h"

#include <QImage>
#include <QLabel>

class QTimer;

class VxAvatarImage : public QLabel 
{
	Q_OBJECT
public:
	VxAvatarImage(QWidget* parent=nullptr, Qt::WindowFlags f= Qt::Widget);
	VxAvatarImage(const QString &text, QWidget* parent=nullptr, Qt::WindowFlags f= Qt::Widget);
	virtual ~VxAvatarImage() = default;

	void						setIcon( enum EMyIcons myIcon );
	void						setImage( const char* resourceUrl );
	void						setImage( QImage& avatarImage );

signals:
	void						clicked();

protected:
	QSize						sizeHint() const;
	void						mousePressEvent( QMouseEvent * event ) override;
	void				        paintEvent( QPaintEvent* ev ) override;

	void						initQAvatarImage( void );
	bool						m_HasThumbImage{ false };
	QImage						m_AvatarImage;
};


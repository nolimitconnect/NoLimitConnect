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

#include <QLabel>

class QTimer;
class QImage;

class VxAvatarImage : public QLabel 
{
	Q_OBJECT
public:
	VxAvatarImage(QWidget* parent=nullptr, Qt::WindowFlags f= Qt::Widget);
	VxAvatarImage(const QString &text, QWidget* parent=nullptr, Qt::WindowFlags f= Qt::Widget);
	virtual ~VxAvatarImage() = default;

	QSize						sizeHint() const;
	void						setImage( const char* resourceUrl );
	void						setImage( QImage& avatarImage );

signals:
	void						clicked();

protected:
	virtual void				mousePressEvent(QMouseEvent * event);

	void						initQAvatarImage( void );
	bool						m_HasThumbImage{ false };
};


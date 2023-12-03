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
#include "GuiParams.h"

class QTimer;

class VxSpinProgress : public QLabel 
{
	Q_OBJECT
public:
	VxSpinProgress(QWidget* parent=nullptr, Qt::WindowFlags f=Qt::Widget);
	VxSpinProgress(const QString &text, QWidget* parent=nullptr, Qt::WindowFlags f=Qt::Widget);
	virtual ~VxSpinProgress() = default;

	void						setSquareButtonSize( EButtonSize buttonSize );

	QSize						sizeHint() const;
	void						setValue( int progressPercent );
	void						setImage( QString resourceUrl );

signals:
	void						clicked();

protected slots:
	void						slotAnimationTickTock( void );

protected:
	virtual void				mousePressEvent(QMouseEvent * event);
	void						hideEvent( QHideEvent* hideEvent );
	void						showEvent( QShowEvent* showEvent );

	void						initQSpinProgress( void );
	void						updateImage( void );

	QTimer *					m_AnimationTimer;
	QImage						m_ProgressPixmap;
	int							m_Angle{ 0 };
	QString						m_PercentText{ "" };
	QSize						m_SpinnerSize;
};


#pragma once
//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QFrame>

class AppCommon;

class VxFrame : public QFrame
{
	Q_OBJECT
public:
	explicit VxFrame( AppCommon& appCommon, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags() );

	void						setFrameId( int frameId )		{ m_FrameId = frameId; }
	int							getFrameId( void )				{ return m_FrameId; }

signals:
	void						signalFrameResized();

protected slots:
    void                        slotAppSystemMenuSelected( int menuId, QWidget* popupMenu );

protected:
	void						resizeEvent( QResizeEvent* ev );

    //=== vars ===//
    AppCommon&					m_MyApp;
    int							m_FrameId;
};

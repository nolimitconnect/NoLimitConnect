#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QBitmap>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
    class TodGameWidgetUi;
}
QT_END_NAMESPACE

class QImage;
class QLabel;
class VidWidget;
class VxGUID;
class VxLabel;
class VxPushButton;

class TodGameWidget : public QWidget
{
	Q_OBJECT

public:
	TodGameWidget( QWidget* parent = nullptr );

	VidWidget *					getVidWidget( void );

	void						callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 );
	void						enableGameButtons( bool bEnable );

	VxPushButton*				getTruthButton( void );
	VxPushButton*				getDareButton( void );
	VxLabel*					getChallengeImageLabel( void );
	QLabel*						getChallengeTextLabel( void );

	QLabel*						getMyDaresLabel( void );
	QLabel*						getMyTruthsLabel( void );
	QLabel*						getFriendDaresLabel( void );
	QLabel*						getFriendTruthsLabel( void );

	QLabel*						getTodStatusLabel( void );

signals:
	void						truthButtonClicked();
	void						dareButtonClicked();

protected:
	Ui::TodGameWidgetUi&		ui;

};

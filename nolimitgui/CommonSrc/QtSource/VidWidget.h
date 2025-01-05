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

#include <QWidget>

#include "GuiPlayerCallback.h"

#include <CoreLib/VxGUID.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class VidWidgetClass;
}
QT_END_NAMESPACE

class AppCommon;
class AppSettings;
class VxLabel;
class MyIcons;
class P2PEngine;

class VidWidget : public QWidget, public GuiPlayerCallback
{
	Q_OBJECT

public:
	VidWidget( QWidget* parent = nullptr );
	virtual ~VidWidget();

	AppCommon&					getMyApp( void ) { return m_MyApp; }
	MyIcons&					getMyIcons( void );

	void						setAppModule( EAppModule appModule )			{ m_AppModule = appModule; }
	EAppModule					getAppModule( void )							{ return  m_AppModule; }

	void						setVideoFeedId( VxGUID& feedOnlineId, EAppModule appModule );
	VxGUID&						getFeedId( void )								{ return m_VideoFeedId; }

	void						setRecordFriendName( QString friendName )		{ m_RecFriendName = friendName; }
	QString&					getRecordFriendName( void )						{ return m_RecFriendName; }
	void						setRecordFilePath( QString filePath )			{ m_RecFilePath = filePath; }
	QString&					getRecordFilePath( void )						{ return m_RecFilePath; }
	void						setRecordFileName( QString fileName )			{ m_RecFileName = fileName; }
	QString&					getRecordFileName( void )						{ return m_RecFileName; }

	void						setVidImageRotation( int imageRotation );
	int							getVidImageRotation( void );

	void						showOfflineImage( void );
	bool						setImageFromFile( QString fileName );
	VxLabel *					getVideoScreen( void );

    void                        showUserMsgLabel( bool showCtrls );
	void						showAllControls( bool showCtrls );
	void						showFeedControls( bool showCtrls );
	bool						isFeedControlsVisible( void );
	void						showRecordControls( bool showCtrls );
	bool						isRecordControlsVisible( void );
	void						showMotionSensitivityControls( bool showCtrls );
	bool						isMotionSensitivityControlsVisible( void );
	void						disablePreview( bool disable );
	void						disableRecordControls( bool disable )					{ m_DisableRecordControls = disable; }
	void						enableCamSourceControls( bool enable );
	void						enableCamFeedControls( bool enable );

	virtual void				callbackGuiPlayMotionVideoFrame( VxGUID& onlineId, QImage& vidFrame, int motion0To100000 ) override;
	virtual void				callbackGuiPlayVideoFrame( VxGUID& onlineId, QImage& vidFrame ) override;
	void						updateVidFeedMotion( int motion0To100000 );

    void                        setAspectRatio( float aspectRatio );

signals:			
	void						clicked();
	void						recordBegin();
	void						recordEnd();
	void						playBegin();
	void						playEnd();
	void						signalFeedRotationChanged( int feedRotation );
	void						signalCamRotationChanged( int feedRotation );

protected slots:
	void						slotStatusMsg( QString userMsg );
	void						slotWidgetClicked( void );
	void						slotCamPreviewButtonClicked( void );
	void						slotCamRotateButtonClicked( void );
	void						slotFeedRotateButtonClicked( void );

	void						slotVidFilesButtonClicked( void );

	void						slotPictureSnapshotButton( void );
	void						slotMotionAlarmButtonClicked( void );
	void						slotRecMotionButtonClicked( void );
	void						slotRecNormalButtonClicked( void );
	void						slotCamSourceButtonClicked( void );
	void						slotCamEnableButtonClicked( void );

	void						slotIconToggleTimeout( void );
	void						slotMotionAlarmTimeout( void );
	void						slotMotionRecordTimeout( void );

protected:
    virtual void				showEvent( QShowEvent* ev ) override;
    virtual void				hideEvent( QHideEvent* ev ) override;
	void						updatePreviewVisibility( void );
	void						updateVidFeedImageRotation( void );
	void						updateCamEnable( void );

	void						playMotionAlarm( void );
	void						updateMotionBarColor( void );
	void						enableVidFilesButton( bool enable );

	//=== vars ===//
	Ui::VidWidgetClass&			ui;
	AppCommon&				    m_MyApp;
	P2PEngine&					m_Engine;
	AppSettings&				m_AppSettings;
	VxGUID&						m_MyOnlineId; 
	VxLabel *					m_ThumbnailPreview;
	VxGUID						m_VideoFeedId; 
	bool						m_DisablePreview;
	bool						m_DisableRecordControls;
	QString						m_RecFriendName;
	QString						m_RecFilePath;
	QString						m_RecFileName;
	bool						m_IconToggle;
	QTimer *					m_IconToggleTimer;
	bool						m_MotionAlarmOn;
	bool						m_MotionAlarmDetected;
	QTimer *					m_MotionAlarmExpireTimer;
	bool						m_MotionRecordOn;
	bool						m_MotionRecordDetected;
	QTimer *					m_MotionRecordExpireTimer;
	bool						m_InNormalRecord;
	EAppModule					m_AppModule{ eAppModuleInvalid };
};

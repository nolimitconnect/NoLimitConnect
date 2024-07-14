#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include "AppDefs.h"

#include <QDialog>
#include <QQueue>

class AppCommon;
class AppletBase;
class AppletLaunchPage;
class AppletMultiMessenger;
class MessengerPage;
class PageMediaPlayer;
class VxAppDisplay;
class VxFrame;

class QComboBox;
class QDialogButtonBox;
class QFrame;
class QGridLayout;
class QGroupBox;
class QLabel;
class QSettings;

// main window
class HomeWindow : public QDialog
{
	Q_OBJECT
public:
	HomeWindow( AppCommon&	appCommon, QString title );
    ~HomeWindow() override;

	AppCommon&					getMyApp( void ) { return m_MyApp; }

	void						initializeHomePage( void );
    void                        restoreHomeWindowGeometry( void );
    void                        saveHomeWindowGeometry( void );

	void						switchWindowFocus( QWidget* appIconButton );

    void						setIsMaxScreenSize( bool isMessagerFrame, bool isFullSizeWindow );
    bool						getIsMaxScreenSize( bool isMessagerFrame );

    QFrame*                     getAppletFrame( EApplet applet );
    QFrame*                     getLaunchParentFrame( ELaunchFrame launchFrame );
    QFrame*                     getMessengerParentFrame( void );
    QFrame*                     getLaunchPageFrame( void );

    AppletLaunchPage *          getLaunchPage( void ) { return m_AppletLaunchPage; }

signals:
	void						signalMainWindowResized( void );
    void						signalMainWindowMoved( void );

	void						signalDeviceOrientationChanged( int qtOrientation ); // pass on signal that orientation has changed

private slots:
	void						slotDeviceOrientationChanged( int qtOrientation ); // from m_AppDisplay
	void						help( void );
    void						slotHomeButtonClicked( void );
	void						slotFrameResized( void );
    void                        accept() override;
    void                        reject() override;

protected:
    void                        showEvent( QShowEvent* ev ) override;
    void						resizeEvent( QResizeEvent* ) override;
    void						moveEvent( QMoveEvent * ) override;
    void						closeEvent( QCloseEvent * ) override;
    void                        paintEvent( QPaintEvent* ev ) override;

	void						initializeNlcDynamicLayout( void );
	void						createAppletLaunchPage( void );
	void						createMessengerPage( void );
    void                        updateAndroidGeomety();
    AppletBase *                findActiveApplet( void );

	//=== vars ===//
	AppCommon&					m_MyApp;
    VxAppDisplay&               m_AppDisplay;
	QString						m_AppTitle;
    QSettings *                 m_WindowSettings{nullptr};

    Qt::Orientation				m_Orientation;
	EHomeLayout					m_LastHomeLayout;
    QGridLayout *				m_MainLayout{nullptr};
    VxFrame*					m_HomeFrameUpperLeft{nullptr};
    VxFrame*					m_HomeFrameRight{nullptr};
    VxFrame*					m_HomeFrameBottom{nullptr};

    PageMediaPlayer *			m_MediaPlayerPage{nullptr};
    AppletLaunchPage *			m_AppletLaunchPage{nullptr};
    QWidget*					m_MessengerParent{nullptr};
    MessengerPage *				m_MessengerPage{nullptr};
    AppletMultiMessenger*       m_AppletMultiMessenger{ nullptr };

    bool						m_MessengerIsFullSize{false};
    bool						m_HomeFrameFullSize{false};
    bool                        m_EngineInitialized{false};
};


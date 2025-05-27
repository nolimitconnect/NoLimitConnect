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

#include "ActivityBase.h"

#include <QString>
#include <QDialog>
#include <QListWidgetItem>
#include <QTimer>

#include <GuiInterface/IDefs.h>
#include <CoreLib/VxDefs.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ScanProfilesDialog;
}
QT_END_NAMESPACE

class ScanProfilePair
{
public:
	ScanProfilePair()
		: m_Ident( 0 ){}

	ScanProfilePair( GuiUser* guiUser, QImage& image )
		: m_Ident( guiUser )
		, m_Image( image )
	{
	}

	~ScanProfilePair(){};
	ScanProfilePair( const ScanProfilePair& rhs ){ *this = rhs; }
	ScanProfilePair& operator=( const ScanProfilePair& rhs )
	{
		if( this != &rhs )
		{
			m_Ident					= rhs.m_Ident;
			m_Image					= rhs.m_Image;
		}

		return *this;
	}

	//=== vars ===//
    GuiUser*				    m_Ident;
	QImage						m_Image;

};

class VxNetIdent;
	
class ActivityScanProfiles : public ActivityBase
{
	Q_OBJECT
public:
	ActivityScanProfiles(	AppCommon& app, 
							QWidget*		parent = nullptr );
	virtual ~ActivityScanProfiles() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget*     getTitleBarWidget( void ) override;
    virtual BottomBarWidget*    getBottomBarWidget( void ) override;

public:
    virtual void				toGuiClientScanSearchComplete( EScanType eScanType ) override;
    virtual void				toGuiSearchResultProfilePic( GuiUser* guiUser, uint8_t* pu8JpgData, uint32_t u32JpgDataLen ) override;

signals:
	 void						signalSearchResultProfilePic( GuiUser* guiUser, QImage oPicBitmap );
	 void						signalSearchComplete();

protected slots:
	void						slotSearchComplete();

	void						slotSearchResultProfilePic( GuiUser* guiUser, QImage oPicBitmap ); 

	void						slotPauseScanClicked( void );
	void						slotStartScanClicked( void );
	void						slotNextScanClicked( void );

	void						onCountdownTimer( void );
	void						slotFriendMenuClicked( void );

protected:
    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;

	void						startStopScan( bool startScan );
	void						startCountdown();
	void						updateCountdownGui();
	void						showNextImage( void ); 
	void						showMatchedPair( GuiUser* guiUser, QImage& oPicBitmap ); 

	void						setTitle( QString strTitle );
	void						setScanStatusText( QString strMsg );
	void						setupIdentWidget( GuiUser* guiUser );


	//=== vars ===//
	Ui::ScanProfilesDialog&		ui;

    GuiUser*				    m_Ident;
	int							m_iCountdownCnt;
	bool						m_bPaused;

	QTimer *					m_CountdownTimer;
	EScanType					m_eScanType;
	bool						m_bIconConnected;
	bool						m_bSearchComplete;
	QVector<ScanProfilePair>	m_ScanList;
    bool						m_ShowNextImage;
    bool						m_IsScanning;
};

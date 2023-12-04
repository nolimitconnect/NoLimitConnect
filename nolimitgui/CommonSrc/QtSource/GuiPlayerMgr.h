#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
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

#include <CoreLib/VxGUID.h>

#include <QObject>
#include <QImage>

class AppCommon;
class AssetBaseInfo;
class GuiPlayerCallback;

class GuiPlayerMgr : public QObject
{
	Q_OBJECT;
public:
	GuiPlayerMgr() = delete;
	GuiPlayerMgr( AppCommon& app );
	GuiPlayerMgr( const GuiPlayerMgr& rhs ) = delete;
	virtual ~GuiPlayerMgr() = default;

	void						playerMgrStartup( void );

	bool						playFile( QString fullFileName, int pos0to100000 = 0 );
	bool						playMedia( AssetBaseInfo& assetInfo, int pos0to100000 = 0 );

	void                        wantPlayVideoCallbacks( GuiPlayerCallback* client, bool enable );

	void						toGuiPlayVideoFrame( VxGUID& feedOnlineId, uint8_t* pu8Jpg, uint32_t u32JpgDataLen, int motion0To100000 );
	virtual int				    toGuiPlayVideoFrame( VxGUID& feedOnlineId, uint8_t* picBuf, uint32_t picBufLen, int picWidth, int picHeight );

	void						toGuiPlayerNlcReady( bool isReady );

signals:
	void						signalInternalPlayVideoFrame( VxGUID feedOnlineId, QImage vidFrame );
	void						signalInternalPlayMotionVideoFrame( VxGUID feedOnlineId, QImage vidFrame, int motion0To100000 );
	void						signalInternalPlayerNlcReady( bool isReady );

protected slots:
	void						slotInternalPlayVideoFrame( VxGUID feedOnlineId, QImage vidFrame );
	void						slotInternalPlayMotionVideoFrame( VxGUID feedOnlineId, QImage vidFrame, int motion0To100000 );
	void						slotInternalPlayerNlcReady( bool isReady );

protected:
	AppCommon&					m_MyApp;

	bool						m_VideoPlayClientsBusy{ false };
	std::vector<GuiPlayerCallback*>  m_VideoPlayClients;
	QAtomicInt					m_BehindFrameCnt;
};

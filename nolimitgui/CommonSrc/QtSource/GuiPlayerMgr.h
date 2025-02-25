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
#include <QSharedPointer>
#include <QSize>

// requires typedef or signal will not connect to slot
typedef QSharedPointer<uint8_t> SharedUint8DataPtr;

class AppCommon;
class AssetBaseInfo;
class GuiPlayerCallback;
class GuiVideoTitleBarCallback;
class QImage;

class GuiPlayerMgr : public QObject
{
	Q_OBJECT;
public:
	GuiPlayerMgr();
	GuiPlayerMgr( const GuiPlayerMgr& rhs ) = delete;
	virtual ~GuiPlayerMgr() = default;

	void						playerMgrStartup( void );

	bool						playFile( QString fileNameAndPath, int pos0to100000, bool isStream, bool useExternPlayer );
	bool						playMedia( AssetBaseInfo& assetInfo, bool useExternPlayer, int pos0to100000 = 0 );
	bool						playStream( AssetBaseInfo& assetInfo, VxGUID lclSessionId, int pos0to100000 = 0 );

	// if feedOnlineId is empty then want all callbacks
	void                        wantPlayVideoCallbacks( VxGUID& feedOnlineId, GuiPlayerCallback* client, bool enable );
	void                        setTitleBarVideoImageSize( QSize imageSize ) { if( imageSize.width() > 4 && imageSize.height() > 4 ) m_TitleBarImageSize = imageSize; }
	void                        wantVideoTitleBarCallbacks( GuiVideoTitleBarCallback* client, bool enable );

	int							toGuiPlayVideoFrame( VxGUID& feedOnlineId, uint8_t* picBuf, uint32_t picBufLen, int picWidth, int picHeight );
	void						toGuiPlayVideoFrame( VxGUID& feedOnlineId, uint8_t* pu8Jpg, uint32_t u32JpgDataLen, int motion0To100000 );

signals:
	void						signalInternalPlayVideoFrame( VxGUID feedOnlineId, QImage* vidFrame, int width, int height );
	void						signalInternalPlayMotionVideoFrame( VxGUID feedOnlineId, SharedUint8DataPtr vidFrame, int dataLen, int motion0To100000 );

protected slots:
	void						slotInternalPlayVideoFrame( VxGUID feedOnlineId, QImage* vidFrame, int width, int height );
	void						slotInternalPlayMotionVideoFrame( VxGUID feedOnlineId, SharedUint8DataPtr vidFrame, int dataLen, int motion0To100000 );

protected:
	bool						m_VideoPlayClientsBusy{ false };
	std::vector<std::pair<VxGUID,GuiPlayerCallback*>>  m_VideoPlayClients;
	std::vector<GuiVideoTitleBarCallback*>  m_VideoTitleBarClients;
	QSize						m_TitleBarImageSize;

	QAtomicInt					m_BehindFeedFrameCnt;
	QAtomicInt					m_BehindMotionFrameCnt;
	VxGUID						m_MediaSessionId;
};

#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include <PktLib/VxCommon.h>
#include <ThumbMgr/ThumbInfo.h>

class AppCommon;
class GuiThumbMgr;
class ThumbInfo;
class QImage;

class GuiThumb : public QWidget
{
public:
    GuiThumb() = delete;
    GuiThumb( AppCommon& app );
    GuiThumb( AppCommon& app, ThumbInfo& thumbInfo, VxGUID& sessionId );
    GuiThumb( const GuiThumb& rhs );
	virtual ~GuiThumb() = default;

    bool						operator == ( const GuiThumb& rhs ) const;
    bool						operator == ( GuiThumb& rhs );

    bool                        isEqualTo( GuiThumb* guiThumb );

    void                        setThumbInfo( ThumbInfo& thumbInfo );
    ThumbInfo&                  getThumbInfo( void )                    { return m_ThumbInfo; }
    bool                        isThumbValid( void )                    { return m_ThumbId.isValid(); }
    void                        setSessionId( VxGUID& sessionId )       { m_SessionId = sessionId; }
    VxGUID&                     getSessionId( void )                    { return m_SessionId; }

    VxGUID&                     getCreatorId( void )                    { return m_CreatorId; }
    VxGUID&                     getThumbId( void )                      { return m_ThumbId; }

    bool                        createImage( QImage& retAvatarImage );

protected:
    void                        updateThumbInfoIds( void );

    AppCommon&                  m_MyApp;
    GuiThumbMgr&                m_GuiThumbMgr;

    ThumbInfo                   m_ThumbInfo;
    VxGUID                      m_SessionId;
    VxGUID                      m_ThumbId;
    VxGUID                      m_CreatorId;
};

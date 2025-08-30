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

#include <WebPageMgr/WebPageCallbackInterface.h>

#include <CoreLib/VxGUID.h>

#include <string>
#include <vector>

#include <QObject>

class GuiWebPageCallback;
class AppCommon;

class GuiWebPageMgr : public QObject, public WebPageCallbackInterface
{
    Q_OBJECT
public:
    GuiWebPageMgr() = delete;
    GuiWebPageMgr( AppCommon& app );
    GuiWebPageMgr( const GuiWebPageMgr& rhs ) = delete;
    virtual ~GuiWebPageMgr() = default;
    void                        onAppCommonCreated( void );
    void                        onMessengerReady( bool ready );
    virtual void                onSystemReady( bool ready );

    void                        wantWebPageCallbacks( GuiWebPageCallback* client, bool enable );

signals:
    void                        signalIInteralWebDownloadStarted( EWebPageType webPageType, VxGUID onlineId, QString fileName, int fileNum );
    void				        signalInternalWebDownloadProgress( EWebPageType webPageType, VxGUID onlineId, int fileNum, int progress );
    void                        signalInternalWebDownloadComplete( EWebPageType webPageType, VxGUID onlineId, QString fileName );
    void                        signalInternalWebDownloadFailed( EWebPageType webPageType, VxGUID onlineId, QString fileName, EXferError xferErr );

protected slots:
    void                        slotInternalWebDownloadStarted( EWebPageType webPageType, VxGUID onlineId, QString fileName, int fileNum );
    void                        slotInternalWebDownloadProgress( EWebPageType webPageType, VxGUID onlineId, int fileNum, int progress );
    void                        slotInternalWebDownloadComplete( EWebPageType webPageType, VxGUID onlineId, QString fileName );
    void                        slotInternalWebDownloadFailed( EWebPageType webPageType, VxGUID onlineId, QString fileName, EXferError xferErr );

protected:
    virtual void				callbackWebDownloadStarted( EWebPageType webPageType, VxGUID& onlineId, std::string& fileName, int fileNum ) override;
    virtual void				callbackWebDownloadProgress( EWebPageType webPageType, VxGUID& onlineId, int fileNum, int progress ) override;
    virtual void				callbackWebDownloadComplete( EWebPageType webPageType, VxGUID& onlineId, std::string& fileName ) override;
    virtual void				callbackWebDownloadFailed( EWebPageType webPageType, VxGUID& onlineId, std::string& fileName, enum EXferError xferErr ) override;

    virtual void				announceWebDownloadStarted( EWebPageType webPageType, VxGUID& onlineId, QString& fileName, int fileNum );
    virtual void				announceWebDownloadProgress( EWebPageType webPageType, VxGUID& onlineId, int fileNum, int progress );
    virtual void				announceWebDownloadComplete( EWebPageType webPageType, VxGUID& onlineId, QString& fileName );
    virtual void				announceWebDownloadFailed( EWebPageType webPageType, VxGUID& onlineId, QString& fileName, enum EXferError xferErr );


    AppCommon&                  m_MyApp;

    std::vector<GuiWebPageCallback*>  m_WebPageClients;
};

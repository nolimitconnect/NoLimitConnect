#pragma once
//============================================================================
// Copyright (C) 2008 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxSktDefs.h"
#include "VxSktBuf.h"
#include "VxSktThrottle.h"
#include <CoreLib/InetAddress.h>

#include <PktLib/PktAnnounce.h>

#include <CoreLib/GroupieId.h>
#include <CoreLib/VxThread.h>
#include <CoreLib/VxSemaphore.h>
#include <CoreLib/VxMutex.h>
#include <CoreLib/VxCrypto.h>


#ifdef TARGET_OS_WINDOWS
	#include <WinSock2.h>
	#include <ws2tcpip.h>
#else
	// LINUX
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
#endif

#include <set>
#include <memory>

enum EConnectionType
{
	eConnectionTypeDirect			= 0,
	eConnectionTypeRelayServer		= 1,	// connection as proxy server
	eConnectionTypeRelayClient		= 2,	// client side proxy connection
	eConnectionTypeProxyRequest		= 3,	// connection was requested by client

	eMaxConnectionType			// always last
};

class VxSktBase;
class VxSktBaseMgr;
class VxPktHdr;
class VxConnectInfo;
class VxGUID;

class FileXferInfo
{
public:
	FileXferInfo()
	{
		m_as8RemoteFileName[0] = 0;
		m_as8LocalFileName[0] = 0;
	}

	char						m_as8RemoteFileName[ 512 ];
	char						m_as8LocalFileName[ 512 ];
	FILE*						m_hFile{ nullptr };
	uint64_t					m_u64FileOffs{ 0 };
	uint64_t					m_u64FileLen{ 0 };
};

class VxSktBase : public VxSktBuf, public VxSktThrottle
{
public:	
	VxSktBase();
	virtual ~VxSktBase() override;

	virtual	void				shutdownSkt( bool closingFromDestructor = false ); ///< normally called just before socket is destroyed.. closes socket and kills threads etc

	virtual void				setThisSkt( std::shared_ptr<VxSktBase>& thisSkt ) { m_ThisSkt = thisSkt; }
	virtual std::shared_ptr<VxSktBase>& getThisSkt( void )						{ return  m_ThisSkt; }

    virtual int					getSktNumber( void )                            { return m_SktNumber; }		///< socket number incremented each socket created
    virtual VxGUID&             getSocketId( void )                             { return m_ConnectionId; }	///< socket unique identifier GUID
	virtual std::string         getSocketIdText( void )                         { return m_ConnectionId.toHexString(); }

	virtual void				setSktHandle( SOCKET sktHandle )				{ m_Socket = sktHandle; }
	virtual SOCKET				getSktHandle( void )							{ return m_Socket; }		///< socket os system handle

	virtual void				setSktType( ESktType sktType )					{ m_eSktType = sktType; }
	virtual ESktType			getSktType( void )								{ return m_eSktType; }

	virtual void				setIsIpv6Connection( bool ipv6 )				{ m_IsIpv6Connection = ipv6; }
	virtual bool				getIsIpv6Connection( void )						{ return m_IsIpv6Connection; }

	virtual void				setLastSktError( RCODE rc );
	virtual RCODE				getLastSktError( void )							{ return m_rcLastSktError; }

	virtual void				setUserExtraData( void * pvData )				{ m_pvUserExtraData = pvData; }
	virtual void *				getUserExtraData( void )						{ return m_pvUserExtraData; }

	virtual void				setReceiveCallback( VX_SKT_CALLBACK pfnReceive, void * pvRxCallbackUserData );
	virtual void				setTransmitCallback( VX_SKT_CALLBACK pfnTransmit, void * pvTxCallbackUserData );

	virtual void				setCallbackReason( ESktCallbackReason eReason )	{ m_eSktCallbackReason = eReason; }
	virtual ESktCallbackReason	getCallbackReason( void )						{ return m_eSktCallbackReason; }
	virtual void				setConnectReason( EConnectReason connectReason );
	virtual EConnectReason		getConnectReason( void )						{ return m_ConnectReason; }
	virtual bool				isTempConnection( void );

	virtual void				setRxCallbackUserData( void * pvData )			{ m_pvRxCallbackUserData = pvData; }
	virtual void *				getRxCallbackUserData( void )					{ return m_pvRxCallbackUserData; }
	virtual void				setTxCallbackUserData( void * pvData )			{ m_pvTxCallbackUserData = pvData; }
	virtual void *				getTxCallbackUserData( void )					{ return m_pvTxCallbackUserData; }

	virtual void				setIsConnected( bool connected )				{ m_bIsConnected = connected; };
	virtual bool				isConnected( void );

	virtual bool				isUdpSocket( void )								{ return ((eSktTypeUdp == m_eSktType)||(eSktTypeUdpBroadcast == m_eSktType)); };
	virtual bool				isTcpSocket( void )								{ return ((eSktTypeTcpConnect == m_eSktType)||(eSktTypeTcpAccept == m_eSktType)); };
	virtual bool				isAcceptSocket( void )							{ return (eSktTypeTcpAccept == m_eSktType);};
	virtual bool				isConnectSocket( void )							{ return (eSktTypeTcpConnect == m_eSktType);};
	virtual bool				isLoopbackSocket( void )						{ return m_LoopbackSocketId == getSocketId(); };

    uint16_t					getRemotePort( void )							{ return m_RmtIp.getPort(); }
    const char*					getRemoteIpAddress( void )                      { return m_strRmtIp.c_str(); }
	uint16_t					getLocalPort( void )							{ return m_LclIp.getPort(); }
	const char*					getLocalIpAddress( void )						{ return m_strLclIp.c_str(); }

	uint16_t					getCryptoKeyPort( void )						{ return (eSktTypeTcpAccept == getSktType()) ? m_LclIp.getPort() : m_RmtIp.getPort(); }

	virtual void				setIsNetServiceConnection( bool isNetSrv ); // also updates active times to avoid timeouts
	virtual bool				isNetServiceConnection( void )					{ return m_IsNetServiceConnection; }

	// PktImAlive activity time
	virtual void				setLastImAliveTimeRxMs( int64_t gmtTimeMs )		{ lockTimeAccess(); m_LastActiveTimeGmtMs = gmtTimeMs; m_LastImAliveTimeGmtRxMs = gmtTimeMs; unlockTimeAccess(); }
	virtual int64_t			    getLastImAliveTimeRxMs( void )					{ lockTimeAccess(); int64_t timeParam = m_LastImAliveTimeGmtRxMs; unlockTimeAccess(); return timeParam; }
	
	virtual void				setLastImAliveTimeTxMs( int64_t gmtTimeMs )		{ lockTimeAccess(); m_LastImAliveTimeGmtTxMs = gmtTimeMs; unlockTimeAccess(); }
	virtual int64_t			    getLastImAliveTimeTxMs( void )					{ lockTimeAccess(); int64_t timeParam = m_LastImAliveTimeGmtTxMs; unlockTimeAccess(); return timeParam; }

	// last any send or recieve activity
	virtual void				setLastActiveTimeMs( int64_t gmtTimeMs )		{ lockTimeAccess(); m_LastActiveTimeGmtMs = gmtTimeMs; unlockTimeAccess(); }
	virtual int64_t		    	getLastActiveTimeMs( void )					    { lockTimeAccess(); int64_t timeParam = m_LastActiveTimeGmtMs; unlockTimeAccess(); return timeParam; }
	virtual void                updateLastActiveTime( void );

	// send or recieve activity not PktImAlive or PktPing
	virtual void				setLastSessionTimeMs( int64_t gmtTimeMs )		{ lockTimeAccess(); m_LastSessionTimeGmtMs = gmtTimeMs; unlockTimeAccess(); }
	virtual int64_t		    	getLastSessionTimeMs( void )					{ lockTimeAccess(); int64_t timeParam = m_LastSessionTimeGmtMs; unlockTimeAccess(); return timeParam; }
	virtual void                updateLastSessionTime( void );

    virtual void				setToDeleteTimeMs( int64_t gmtTimeMs )          { m_ToDeleteTimeGmtMs = gmtTimeMs; }
    virtual int64_t		    	getToDeleteTimeMs( void )                       { return m_ToDeleteTimeGmtMs; }

    virtual void				setInUseByRxThread( bool inUse )                { m_InUseByRxThread = inUse; }
    virtual bool		    	getInUseByRxThread( void )                      { return m_InUseByRxThread; }

	virtual RCODE				connectTo(	InetAddress&	oLclIp,	
											const char*		pIpOrUrl,						// remote ip or url
											uint16_t		u16Port,						// port to connect to
											int				iTimeoutMilliSeconds = 10000 );	// milli seconds before connect attempt times out

	virtual void				createConnectionUsingSocket( SOCKET skt, const char* rmtIp, uint16_t port );

	//! send data without encrypting
	virtual RCODE				sendData(	const char*		pData,							// data to send
											int				iDataLen,						// length of data
											bool			bDisconnectAfterSend = false );	// if true disconnect after data is sent

	virtual void				closeSkt( ESktCloseReason  closeReason,  bool bFlushThenClose = false );
    virtual std::string		    describeSktConnection( void );

	bool						bindSocket( struct addrinfo * poResultAddr );
	bool						isIPv6Address (const char* addr );
	int							getIPv6ScopeID( const char* addr );
	const char*					stripIPv6ScopeID( const char* addr, std::string &buf );
	RCODE						joinMulticastGroup( InetAddress& oLclAddress, const char*mcastAddr );

	void						setTTL( uint8_t ttl );
	void						setAllowLoopback( bool allowLoopback );
	void						setAllowBroadcast( bool allowBroadcast );

	//! Do connect to from thread
	RCODE						doConnectTo( void );

	//! get the sockets peer connection ip address as host order int32_t
	virtual RCODE				getRemoteIp(	InetAddress &u32RetIp,		// return ip
												uint16_t &u16RetPort );	// return port
	//! get remote ip as string
	virtual std::string		    getRemoteIp( void );

    //! get remote ip as string
    virtual void 		        getRemoteIp( std::string& rmtIp )   { rmtIp = m_strRmtIp.empty() ? "" : m_strRmtIp; }

	//! simpler version of getRemoteIp returns ip as host order int32_t
	//virtual RCODE				getRemoteIp( InetAddress &u32RetIp );			// return ip
	//! get remote port connection is on
	virtual InetAddress			getRemoteIpBinary( void )           { return m_RmtIp;}			// return ip in host ordered binary u32

	//! get local ip as string
	virtual std::string		    getLocalIp( void );

	//! set socket to blocking or not
	virtual	RCODE				setSktBlocking( bool bBlock );

	//=== encryption functions ===//
	//! return true if transmit encryption key is set
	virtual bool				isTxEncryptionKeySet( void )        { return m_TxKey.isKeySet(); }
	//! return true if receive encryption key is set
	virtual bool				isRxEncryptionKeySet( void )        { return m_RxKey.isKeySet(); }

	//! encrypt then send data using session crypto
	virtual RCODE				txEncrypted( const char*	pData, 					// data to send
											 int			iDataLen );			// length of data

	//! encrypt with given key then send.. does not affect session crypto
	virtual RCODE				txEncrypted( VxKey*			poKey,					// key to encrypt with
											 const char*	pData,					// data to send
											 int			iDataLen );				// length of data

	virtual RCODE				txPacket( VxGUID	destOnlineId,			// online id of destination user
										  VxPktHdr*	pktHdr );				// packet to send

	virtual RCODE				txPacketWithDestId( VxPktHdr* pktHdr );				// packet to send

	//! decrypt as much as possible in receive buffer
	virtual RCODE				decryptReceiveData( void );

	//! fire up receive thread
	void						startReceiveThread( const char* pVxThreadName );

	void						setRmtAddress( struct sockaddr_storage& oVxSktAddr );
	void						setRmtAddress( struct sockaddr_in& oVxSktAddr );
	void						setRmtAddress( struct sockaddr& oVxSktAddr );
	void						setLclAddress( struct sockaddr_storage& oVxSktAddr );

	void						setTxCryptoPassword( const char* data, int len );
	void						setRxCryptoPassword( const char* data, int len );
	bool						isTxCryptoKeySet( void );
	bool						isRxCryptoKeySet( void );
    std::string                 describeSktType( void );

	void						lockCryptoAccess( void )				        { m_CryptoMutex.lock(); }
	void						unlockCryptoAccess( void )				        { m_CryptoMutex.unlock(); }

	static int					getTotalCreatedSktCount( void )			        { return m_TotalCreatedSktCnt; }
	static int					getCurrentSktCount( void )				        { return m_CurrentSktCnt; }
    static const char*		    describeSktError( RCODE rc );
    static const char*		    describeSktCallbackReason( ESktCallbackReason reason );

    virtual bool                setPeerPktAnn( PktAnnounce& pktAnn );
    virtual PktAnnounce&        getPeerPktAnn( void )                           { return m_PeerPktAnn; }
	std::string					describePeerUser( void )						{ return m_PeerPktAnn.describeUser(); }

    void                        setIsPeerPktAnnSet( bool isSet )                { m_IsPeerPktAnnSet = isSet; }  
    bool                        getIsPeerPktAnnSet( void )                      { return m_IsPeerPktAnnSet; }
    bool                        getPeerPktAnnCopy( PktAnnounce &peerAnn );
	std::string					getPeerOnlineName();

    // returns peer online id. caller should check VxGUID::isVxGUIDValid() for validity
    VxGUID&                     getPeerOnlineId( void )							{ return m_PeerOnlineId; }

    virtual void                dumpSocketStats( const char* reason = nullptr, bool fullDump = false );

	bool 					    getIsMulticastSkt( void )							{ return m_IsMulticastSkt; }

	void						setMulticastPort( uint16_t multicastPort )			{ m_MulticastPort = multicastPort; }
	uint16_t					getMulticastPort( void )							{ return m_MulticastPort; }
	void 						setMulticastGroupIp( std::string groupIpAddr)		{ m_MulticastGroupIp = groupIpAddr; }
	std::string 				getMulticastGroupIp( void )							{ return m_MulticastGroupIp; }
	struct sockaddr_in&			getMulticastRxAddr( void )							{ return m_MulticastRxAddr; }
	socklen_t					getMulticastRxAddrLen( void )						{ return sizeof( m_MulticastRxAddr ); }

	void						addGroupieId( GroupieId& groupieId )				{ m_GroupieSet.insert( groupieId ); }
	bool						removeGroupieId( GroupieId& groupieId )				{ m_GroupieSet.erase( groupieId ); return m_GroupieSet.empty(); }

	void						onOncePer30Seconds( VxGUID& myOnlineId );

	static bool					isFatalSocketError( RCODE rc );

	static void					incrementRunningRxSktThreadCnt( void )				{ m_RunningRxThreadCnt++; }
	static void					decrementRunningRxSktThreadCnt( void )				{ m_RunningRxThreadCnt--; }
	static int					getRunningRxSktThreadCnt( void )					{ return m_RunningRxThreadCnt; }

protected:
	bool						toSocketAddrInfo(	int sockType, 
													const char*addr, 
													int port, 
													struct addrinfo **addrInfo, 
													bool isBindAddr );

	bool						toSocketAddrIn(		const char*addr, 
													int port, 
													struct sockaddr_in *sockaddr, 
													bool isBindAddr );


    const std::string&          describeSktDirection( void );

	void						lockTimeAccess( void ) { m_TimeAccessMutex.lock(); }
	void						unlockTimeAccess( void ) { m_TimeAccessMutex.unlock(); }

public:
	void						doCloseThisSocketHandle( bool bFlushThenClose );

	bool						m_IsMulticastSkt{ false };
	uint16_t					m_MulticastPort{ 0 };
	std::string					m_MulticastGroupIp{ "" };
	struct sockaddr_in			m_MulticastRxAddr;

    SOCKET						m_Socket{ INVALID_SOCKET };	    // handle to socket
    int							m_SktNumber{ 0 };				    // socket unique id
    VxGUID                      m_ConnectionId;                 // unique connection id 

	InetAddrAndPort				m_LclIp;				        // local ip address
    std::string					m_strLclIp{ "0.0.0.0" };		// local ip address in dotted form

	InetAddrAndPort				m_RmtIp;				        // remote (peer) ip address
    std::string					m_strRmtIp{ "0.0.0.0" };	    // remote (peer) ip address in dotted form

	//=== state vars ===//
	VxMutex						m_TimeAccessMutex;			 
    int64_t			    		m_LastActiveTimeGmtMs{ 0 };	    // last time received data
	int64_t		    			m_LastImAliveTimeGmtRxMs{ 0 };  // last time received PktImAliveReply
	int64_t		    			m_LastImAliveTimeGmtTxMs{ 0 };  // last time sent PktImAliveReply
	int64_t		    			m_LastSessionTimeGmtMs{ 0 };    // last time received or sent pkt that is not PktImAlive or PktPing
	int64_t	    				m_ToDeleteTimeGmtMs{ 0 };

	VxThread					m_SktRxThread;			        // thread for handling socket receive

	VxMutex						m_CryptoMutex;			        // mutex
	bool						m_bClosingFromRxThread{ false };   // if true then call to close function was made by receive thread
	bool						m_bClosingFromDestructor{ false };  // if true then call to close function was made by destructor
	VxSktBaseMgr *				m_SktMgr{ nullptr };
	int							m_iLastRxLen{ 0 };			    // size of last packet received
    int							m_iLastTxLen{ 0 };			    // size of last packet sent

	VxKey						m_RxKey;				        // encryption key for receive
	VxCrypto					m_RxCrypto;			            // encryption object for receive
	VxKey						m_TxKey;				        // encryption key for transmit
	VxCrypto					m_TxCrypto;			            // encryption object for transmit
    VxMutex                     m_TxMutex;                      // tx thread mutex
	uint8_t						m_u8TxSeqNum; // not initialized on purpose	// sequence number used to thwart replay attacks
    
	VxSemaphore					m_RelayEventSemaphore;
    VX_SKT_CALLBACK				m_pfnReceive{ nullptr };			// receive function must be set by user

protected:

    int							m_iConnectTimeout{ 0 };	            // how long to try to connect
	bool						m_bIsConnected{ false };			// return true if is connected
    ESktType					m_eSktType{ eSktTypeNone };			// type of socket
    ESktCallbackReason			m_eSktCallbackReason{ eSktCallbackReasonUnknown };	// why callback is being performed
    ESktCloseReason             m_SktCloseReason{ eSktCloseReasonUnknown };

	VX_SKT_CALLBACK				m_pfnTransmit{ nullptr };			// optional function for transmit statistics
	void *						m_pvRxCallbackUserData{ nullptr };  // user defined rx callback data
	void *						m_pvTxCallbackUserData{ nullptr };  // user defined tx callback data
	void *						m_pvUserExtraData{ nullptr };       // user defined extra data
    RCODE						m_rcLastSktError{ 0 };			    // last error that occurred
    bool                        m_InUseByRxThread{ false };

    bool                        m_IsPeerPktAnnSet{ false };
    PktAnnounce                 m_PeerPktAnn;
    VxGUID                      m_PeerOnlineId;
    VxMutex                     m_PeerAnnMutex;

	std::set<GroupieId>			m_GroupieSet;

	EConnectReason				m_ConnectReason{ eConnectReasonUnknown };
	bool						m_HasBeenShutdown{ false };
	std::shared_ptr<VxSktBase>  m_ThisSkt;

	bool						m_IsIpv6Connection{ false };

    static int					m_TotalCreatedSktCnt;	            // total number of sockets created since program started
    static int					m_CurrentSktCnt;		            // current number of sockets exiting in memory
	static int					m_RunningRxThreadCnt;

    static std::string          m_SktDirConnect;
    static std::string          m_SktDirAccept;
    static std::string          m_SktDirUdp;
    static std::string          m_SktDirBroadcast;
    static std::string          m_SktDirLoopback;
    static std::string          m_SktDirUnknown;
	static VxGUID				m_LoopbackSocketId;

	bool						m_IsNetServiceConnection{ false };
};


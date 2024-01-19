//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VirtStreamCache.h"

#include <CoreLib/VxDebug.h>

//============================================================================
int64_t VirtCache::hasData( int64_t offset, int64_t len ) // return true if has any of the data
{
	int64_t lenInCache = 0;
	if( m_CacheStartOffs <= offset && m_CacheEndOffs > offset )
	{
		int64_t bufOffs = offset - m_CacheStartOffs;
		lenInCache = std::min( len, m_CacheEndOffs - bufOffs );
	}
	
	return lenInCache;
}

//============================================================================
int64_t VirtCache::writeData( int64_t offset, char* data, int64_t dataLen )
{
	m_CacheStartOffs = offset;
	memcpy( m_Buf, data, dataLen );
	m_CacheEndOffs = m_CacheStartOffs + dataLen;
	return dataLen;
}

//============================================================================
int64_t VirtCache::readData( int64_t offset, char* data, int64_t len )
{
	int64_t buffOffs = offset - m_CacheStartOffs;
	int64_t lenToRead = std::min( len, m_CacheEndOffs - buffOffs );
	memcpy( data, &m_Buf[buffOffs], lenToRead );
	return lenToRead;
}

//============================================================================
//============================================================================

//============================================================================
VirtStreamCache::~VirtStreamCache()
{
	clearCache( true );
}

//============================================================================
void VirtStreamCache::clearCache( bool isLocked )
{
	if( !isLocked )
	{
		lockCache();
	}
;
	for( auto iter = m_VirtCache.begin(); iter != m_VirtCache.end(); ++iter )
	{
		delete* iter;
	}

	m_VirtCache.clear();
	m_MaxAssetLen = 0;
	m_CacheStartOffs = 0;
	m_CacheEndOffs = 0;
	m_TotalCachedData = 0;

	if( !isLocked )
	{
		unlockCache();
	}
}

//============================================================================
int64_t VirtStreamCache::hasData( int64_t offset, int64_t len )
{
	int64_t lenInCache = 0;
	lockCache();
	if( m_CacheStartOffs <= offset && m_CacheEndOffs > offset )
	{
		int64_t bufOffs = offset - m_CacheStartOffs;
		lenInCache = std::min( len, m_CacheEndOffs - bufOffs );
	}

	unlockCache();

	return lenInCache;
}

//============================================================================
int64_t VirtStreamCache::writeData( int64_t offset, char* data, int64_t len )
{
	if( hasData( offset, len ) )
	{
		LogModule( eLogMediaStream, LOG_ERROR, "%s data already exist", __func__ );
		vx_assert( false );
		return false;
	}

	int64_t lenWritten{ 0 };
	lockCache();
	if( offset != 0 && offset != m_CacheEndOffs )
	{
		LogModule( eLogMediaStream, LOG_WARN, "%s data is not consistent.. flushing cashe", __func__ );
		clearCache( true );
		m_CacheStartOffs = offset;
	}

	while( lenWritten < len )
	{
		VirtCache* cache = new VirtCache();
		int64_t lenThisCache = cache->writeData( offset + lenWritten, &data[lenWritten], len );
		lenWritten += lenThisCache;
		m_VirtCache.emplace_back( cache );
		m_CacheEndOffs += lenThisCache;
	}

	unlockCache();
	return lenWritten;
}

//============================================================================
int64_t VirtStreamCache::readData( int64_t offset, char* data, int64_t len )
{
	int64_t lenRead = 0;
	bool foundData{ false };
	lockCache();
	for( auto virtCache : m_VirtCache )
	{
		int64_t maxRead = len - lenRead;
		if( virtCache->hasData( offset + lenRead, len - lenRead ) )
		{
			foundData = true;
			int64_t dataReadThisCache = virtCache->readData( offset + lenRead, &data[lenRead], maxRead );
			if( !dataReadThisCache )
			{
				LogMsg( LOG_ERROR, "%s faild to read from cache that has data", __func__ );
				vx_assert( false );
				unlockCache();
				return 0;
			}
			else
			{
				lenRead += dataReadThisCache;
			}
		}
		else if( foundData )
		{
			LogModule( eLogMediaStream, LOG_WARN, "%s only %" PRId64 " of data in the cache", __func__, lenRead );
			break;
		}

		if( lenRead > len )
		{
			LogMsg( LOG_ERROR, "%s read more data than specified", __func__ );
			vx_assert( false );
			unlockCache();
			return 0;
		}

		if( lenRead == len )
		{
			break;
		}
	}

	unlockCache();

	return lenRead;
}
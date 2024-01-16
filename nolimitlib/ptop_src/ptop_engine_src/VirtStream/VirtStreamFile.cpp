//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VirtStreamFile.h"

//============================================================================
bool VirtStreamFile::setConnection( std::shared_ptr<VxSktBase> sktBase )
{
	m_SktBase = sktBase;
	setError( 0 );
	return isConnected();
}

//============================================================================
bool VirtStreamFile::isConnected( void )
{ 
	if( !m_SktBase || !m_SktBase->isConnected() )
	{
		m_SktBase.reset(); 
		setError( EACCES );
		return false;
	}

	return true;
}

//============================================================================
void VirtStreamFile::removeConnection( void )
{
	m_SktBase.reset(); 
	setError( EACCES );
}
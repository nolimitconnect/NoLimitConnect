#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QComboBox>

#include <CoreLib/VxFileInfo.h>

class VxComboBoxFileFilter : public QComboBox
{
	Q_OBJECT
public:
	VxComboBoxFileFilter(QWidget* parent=nullptr);
	virtual ~VxComboBoxFileFilter();

	void						setFileFilter( EFileFilterType eFileFilter );
	uint8_t						getMaskFromFileFilterType( EFileFilterType eFileFilterType );

	unsigned char				getCurrentFileFilterMask( void );
	EFileFilterType				getCurrentFileFilterType( void );
    EFileFilterType             getFileFilterTypeFromIndex( int selectionIdx );

signals:
	void						signalApplyFileFilter(unsigned char fileTypeMask);
	void						signalFileFilterSelectionChanged( EFileFilterType eFileFilterType );

protected slots:
	void						handleSelectionChanged( int idx );
    int						    getSelectionIndexFromFilterType( EFileFilterType eFileFilter );

private:
	unsigned char				m_CurFileMask;
    EFileFilterType             m_FileFilterType;
};


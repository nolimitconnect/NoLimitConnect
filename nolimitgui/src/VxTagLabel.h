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

#include "VxLabel.h"

class AssetBaseInfo;

class VxTagLabel : public VxLabel
{
	Q_OBJECT
public:
	VxTagLabel(QWidget* parent=nullptr, Qt::WindowFlags f= Qt::Widget);
	VxTagLabel(const QString &text, QWidget* parent=nullptr, Qt::WindowFlags f= Qt::Widget);
	virtual ~VxTagLabel();

    void						setAssetInfo( AssetBaseInfo* assetInfo )		{ m_AssetInfo = assetInfo; }
    AssetBaseInfo*				getAssetInfo( void )							{ return m_AssetInfo; }

protected slots:
	void						slotOnClicked( void );


protected:
	void						setupQTagLabel( void );

    AssetBaseInfo*				m_AssetInfo{ nullptr };
};


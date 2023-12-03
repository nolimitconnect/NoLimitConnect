//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "HistoryEntryWidget.h"
#include "AppCommon.h"

//============================================================================
HistoryEntryWidget::HistoryEntryWidget(QWidget* parent)
: QWidget( parent )
, m_MyApp( GetAppInstance() )
, m_Engine( GetAppInstance().getEngine() )
{
	ui.setupUi(this);
}

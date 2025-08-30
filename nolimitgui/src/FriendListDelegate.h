//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QPainter>
#include <QAbstractItemDelegate>

class FriendListDelegate : public QAbstractItemDelegate
{
public:
	FriendListDelegate(QObject *parent = 0);
	virtual ~FriendListDelegate();

	//=== overrides ===//
	//! custom paint
	void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	//! size of each item
	QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

};
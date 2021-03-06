/*
    Copyright 2004 - 2006 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation;
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#ifndef CHANNELSELECTOR_H
#define CHANNELSELECTOR_H

#include <QtWidgets/QDialog>

class QListWidget;
class QPushButton;

namespace JackMix {
namespace GUI {

class ChannelSelector : public QDialog
{
Q_OBJECT
public:
	ChannelSelector( QString title, QString label, QStringList channels, QWidget*, const char* =0 );
	~ChannelSelector();

signals:
	void selectedChannel( QString );
public slots:
	void addChannel( QString );
	void removeChannel( QString );
private slots:
	virtual void commit();
	virtual void commitnquit();
protected:
	QListWidget *_list;
	QPushButton *_commit, *_commit_n_quit, *_cancel;
};

};
};
#endif // CHANNELSELECTOR_H


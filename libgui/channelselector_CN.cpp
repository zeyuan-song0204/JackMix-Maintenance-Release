#include "channelselector_CN.h"
#include "channelselector.moc"

#include <QtCore/QString>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>

namespace JackMix_CN {
namespace GUI_CN {

ChannelSelector_CN::ChannelSelector_CN( QString title, QString label, QStringList channels, QWidget* p, const char* n ) : QDialog( p ) {
	this->setWindowTitle( title );

	QGridLayout *_layout = new QGridLayout( this );
	QLabel *_label = new QLabel( label, this );
	_layout->addWidget( _label, 0,0, 1,4 );

	_list = new QListWidget( this );
	_list->addItems( channels );
	_layout->addWidget( _list, 1,0, 1,4 );

	_layout->addItem( new QSpacerItem( 10,10 ), 2,0, 1,4 );

	_layout->addItem( new QSpacerItem( 40,10 ), 3,0 );
	_commit_n_quit = new QPushButton( "确认 && 退出", this );
	_commit_n_quit->setDefault( true );
	connect( _commit_n_quit, SIGNAL( clicked() ), this, SLOT( commitnquit() ) );
	_layout->addWidget( _commit_n_quit, 3,3 );
	_commit = new QPushButton( "确认", this );
	connect( _commit, SIGNAL( clicked() ), this, SLOT( commit() ) );
	_layout->addWidget( _commit, 3,2 );
	_cancel = new QPushButton( "取消", this );
	connect( _cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	_layout->addWidget( _cancel, 3,1 );

}
ChannelSelector_CN::~ChannelSelector_CN() {
}

void ChannelSelector_CN::commit() {
qDebug( "ChannelSelector_CN::commit()" );
qDebug( "Returning: %s", _list->currentItem()->text().toStdString().c_str() );
	emit selectedChannel( _list->currentItem()->text() );
}
void ChannelSelector_CN::commitnquit() {
qDebug( "ChannelSelector::commitnquit()" );
	commit();
qDebug( "Now quit..." );
	done( 0 );
}

void ChannelSelector_CN::addChannel( QString ) {
}
void ChannelSelector_CN::removeChannel( QString ) {
}

};
};

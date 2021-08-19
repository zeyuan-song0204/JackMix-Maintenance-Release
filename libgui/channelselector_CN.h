#ifndef CHANNELSELECTOR_H
#define CHANNELSELECTOR_H

#include <QtWidgets/QDialog>

class QListWidget;
class QPushButton;

namespace JackMix_CN {
namespace GUI_CN {

class ChannelSelector_CN : public QDialog
{
Q_OBJECT
public:
	ChannelSelector_CN( QString title, QString label, QStringList channels, QWidget*, const char* =0 );
	~ChannelSelector_CN();
signals:
	void selectedChannel_CN( QString );
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

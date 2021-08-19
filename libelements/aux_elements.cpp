#include "aux_elements.h"
#include "aux_elements.moc"

#include "knob.h"
#include "midicontrolchannelassigner.h"

#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtCore/QList>
#include <QtCore/QStringList>
//#include <QtCore/QDebug>

#include <controlreceiver.h>
#include <controlsender.h>
#include <mixingmatrix.h>


using namespace JackMix;
using namespace JackMix::MixerElements;
using namespace JackMix::MixingMatrix;

/// The Factory for creating this things...
//EN

class AuxFactory : public JackMix::MixingMatrix::ElementFactory
{
public:
	AuxFactory() : ElementFactory() {
		//qDebug() << "AuxFactory::AuxFactory()";
		//globaldebug();
	}
	~AuxFactory() {}

	QStringList canCreate() const {
		return QStringList()<<"AuxElement";
	}
	QStringList canCreate( int in, int out ) const {
		if ( in==1 && out==1 ) return QStringList()<<"AuxElement";
		return QStringList();
	}

	Element* create( QString type , QStringList ins, QStringList outs, Widget* p, const char* n=0 ) {
		if ( type=="AuxElement" )
			return new AuxElement( ins, outs, p, n );
		else
			return 0;
	}
};
//CN
class AuxFactory_CN : public JackMix::MixingMatrix::ElementFactory
{
public:
	AuxFactory_CN() : ElementFactory() {
		//qDebug() << "AuxFactory::AuxFactory()";
		//globaldebug();
	}
	~AuxFactory_CN() {}

	QStringList canCreate() const {
		return QStringList()<<"AuxElement_CN";
	}
	QStringList canCreate( int in, int out ) const {
		if ( in==1 && out==1 ) return QStringList()<<"AuxElement_CN";
		return QStringList();
	}

	Element* create( QString type , QStringList ins, QStringList outs, Widget* p, const char* n=0 ) {
		if ( type=="AuxElement_CN" )
			return new AuxElement_CN( ins, outs, p, n );
		else
			return 0;
	}
};
//FR
class AuxFactory_FR : public JackMix::MixingMatrix::ElementFactory
{
public:
	AuxFactory_FR() : ElementFactory() {
		//qDebug() << "AuxFactory::AuxFactory()";
		//globaldebug();
	}
	~AuxFactory_FR() {}

	QStringList canCreate() const {
		return QStringList()<<"AuxElement_FR";
	}
	QStringList canCreate( int in, int out ) const {
		if ( in==1 && out==1 ) return QStringList()<<"AuxElement_FR";
		return QStringList();
	}

	Element* create( QString type , QStringList ins, QStringList outs, Widget* p, const char* n=0 ) {
		if ( type=="AuxElement_FR" )
			return new AuxElement_FR( ins, outs, p, n );
		else
			return 0;
	}
};
void MixerElements::init_aux_elements() {
	new AuxFactory();
}
void MixerElements::init_aux_elements_CN() {
	new AuxFactory_CN();
}
void MixerElements::init_aux_elements_FR() {
	new AuxFactory_FR();
}
//EN

AuxElement::AuxElement( QStringList inchannel, QStringList outchannel, MixingMatrix::Widget* p, const char* n )
	: Element( inchannel, outchannel, p, n )
	, dB2VolCalc( -42, 6 )
{
	if (p->mode() == Widget::Select) {
		menu()->addAction( "Select", this, SLOT( slot_simple_select() ) );
		menu()->addAction( "Replace", this, SLOT( slot_simple_replace() ) );
	}
	menu()->addAction( "Assign MIDI Parameter", this, SLOT( slot_assign_midi_parameters() ) );
	//menu()->addAction( "Delete Input", this, SLOT( slot_delete_input()) );
	

	//QVBoxLayout* _layout = new QVBoxLayout( this );
	if ( _in[0] == _out[0] ) {
		disp_name = new QLabel( QString( "<qt><center>%1</center></qt>" ).arg( _in[0] ), this );
		_layout->addWidget(disp_name, 0);
	}
	//button1->setText("mute");
	bt_mute->setStyleSheet("background-color:rgb(243,250,139)");
	_layout->addWidget(bt_mute);
	connect(bt_mute,SIGNAL(clicked()),this,SLOT(mute()));
	_poti = new JackMix::GUI::Knob(
		amptodb( backend()->getVolume( _in[0], _out[0] ) ),
		dbmin, dbmax, 2, 3, this );
	_layout->addWidget( _poti, 100 );
	
	


	connect( _poti, SIGNAL( valueChanged( double ) ), this, SLOT( emitvalue( double ) ) );
	connect( _poti, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _poti, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );
	
	midi_params.append(0);        // Initial MIDI paramater number
	midi_delegates.append(_poti); //   for the potentiometer
	//qDebug()<<"There are "<<midi_delegates.size()<<" Midi delegates";

	// Now construct the parameter setting menu
	_cca = new JackMix::GUI::MidiControlChannelAssigner(QString("Set MIDI control parameter"),
	                                                     "<qt>" + _in[0] + " &rarr; "  + _out[0] + "</qt>",
	                                                     QStringList() << "Gain",
	                                                     midi_params,
		                                             this
	                                                    );
	connect( _cca, SIGNAL(assignParameters(QList<int>)), this, SLOT(update_midi_parameters(QList<int>)) );
}
AuxElement::~AuxElement() {
}

void AuxElement::emitvalue( double n ) {
	backend()->setVolume( _in[0], _out[0], dbtoamp( n ) );
}

void AuxElement::mute(){
	delete _poti;
	_poti = new JackMix::GUI::Knob(
			amptodb( backend()->getVolume( _in[0], _out[0] ) ),
			0, 0, 2, 3, this );
	_layout->addWidget( _poti, 100 );
	bt_mute->setText("Mute cancel ");
	connect(bt_mute,SIGNAL(clicked()),this,SLOT(cancel_mute()));
}
void AuxElement::cancel_mute(){
	delete _poti;
	_poti = new JackMix::GUI::Knob(
			amptodb( backend()->getVolume( _in[0], _out[0] ) ),
			dbmin, dbmax, 2, 3, this );
	_layout->addWidget( _poti, 100 );
	bt_mute->setText("Mute");
	connect(bt_mute,SIGNAL(clicked()),this,SLOT(mute()));
}

// void AuxElement::mute(){
		
// 	qDebug()<<"mute success";
// 	delete bt_mute;
// 	delete _poti;
// 	_poti = new JackMix::GUI::Knob(
// 			amptodb( backend()->getVolume( _in[0], _out[0] ) ),
// 			0, 0, 2, 3, this );
// 	_layout->addWidget( _poti, 100 );
// 	if(!bt_muted){
// 		QPushButton *bt_muted=new QPushButton("Muted");
// 	}
	
// 	_layout->addWidget(bt_muted);
// 	connect(bt_muted,SIGNAL(clicked()),this,SLOT(cancel_mute()));
	
	
// 	}
// void AuxElement::cancel_mute(){
// 	qDebug()<<"mute canceled";
// 	delete _poti;
// 	delete bt_muted;
// 	_poti = new JackMix::GUI::Knob(
// 			amptodb( backend()->getVolume( _in[0], _out[0] ) ),
// 			dbmin, dbmax, 2, 3, this );
// 	_layout->addWidget( _poti, 100 );
// 	QPushButton *bt_mute=new QPushButton("Mute");
// 	_layout->addWidget( bt_mute);
// 	connect(bt_mute,SIGNAL(clicked()),this,SLOT(mute()));
	
// }

void AuxElement::setIndicator(const QColor& c) { _poti->setIndicatorColor(c); };
//CN
AuxElement_CN::AuxElement_CN( QStringList inchannel, QStringList outchannel, MixingMatrix::Widget* p, const char* n )
	: Element( inchannel, outchannel, p, n )
	, dB2VolCalc( -42, 6 )
{
	if (p->mode() == Widget::Select) {
		menu()->addAction( "选择", this, SLOT( slot_simple_select() ) );
		menu()->addAction( "合并", this, SLOT( slot_simple_replace() ) );
	}
	menu()->addAction( "分配 MIDI 参数", this, SLOT( slot_assign_midi_parameters() ) );

	//QVBoxLayout* _layout = new QVBoxLayout( this );

	if ( _in[0] == _out[0] ) {
		disp_name = new QLabel( QString( "<qt><center>%1</center></qt>" ).arg( _in[0] ), this );
		_layout->addWidget(disp_name, 0);
	}
	bt_mute->setStyleSheet("background-color:rgb(243,250,139)");
	_layout->addWidget(bt_mute);
	connect(bt_mute,SIGNAL(clicked()),this,SLOT(mute()));
	_poti = new JackMix::GUI::Knob(
		amptodb( backend()->getVolume( _in[0], _out[0] ) ),
		dbmin, dbmax, 2, 3, this );
	_layout->addWidget( _poti, 100 );

	connect( _poti, SIGNAL( valueChanged( double ) ), this, SLOT( emitvalue( double ) ) );
	connect( _poti, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _poti, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );
	
	
	midi_params.append(0);        // Initial MIDI paramater number
	midi_delegates.append(_poti); //   for the potentiometer
	//qDebug()<<"There are "<<midi_delegates.size()<<" Midi delegates";

	// Now construct the parameter setting menu
	_cca = new JackMix::GUI::MidiControlChannelAssigner(QString("设置 MIDI 控制参数"),
	                                                     "<qt>" + _in[0] + " &rarr; "  + _out[0] + "</qt>",
	                                                     QStringList() << "增益",
	                                                     midi_params,
		                                             this
	                                                    );
	connect( _cca, SIGNAL(assignParameters(QList<int>)), this, SLOT(update_midi_parameters(QList<int>)) );
}
AuxElement_CN::~AuxElement_CN() {
}

void AuxElement_CN::emitvalue( double n ) {
	backend()->setVolume( _in[0], _out[0], dbtoamp( n ) );
}
void AuxElement_CN::setIndicator(const QColor& c) { _poti->setIndicatorColor(c); };
void AuxElement_CN::mute(){
	delete _poti;
	_poti = new JackMix::GUI::Knob(
			amptodb( backend()->getVolume( _in[0], _out[0] ) ),
			0, 0, 2, 3, this );
	_layout->addWidget( _poti, 100 );
	bt_mute->setText("取消静音 ");
	connect(bt_mute,SIGNAL(clicked()),this,SLOT(cancel_mute()));
}
void AuxElement_CN::cancel_mute(){
	delete _poti;
	_poti = new JackMix::GUI::Knob(
			amptodb( backend()->getVolume( _in[0], _out[0] ) ),
			dbmin, dbmax, 2, 3, this );
	_layout->addWidget( _poti, 100 );
	bt_mute->setText("静音");
	connect(bt_mute,SIGNAL(clicked()),this,SLOT(mute()));
}
//FR
AuxElement_FR::AuxElement_FR( QStringList inchannel, QStringList outchannel, MixingMatrix::Widget* p, const char* n )
	: Element( inchannel, outchannel, p, n )
	, dB2VolCalc( -42, 6 )
{
	if (p->mode() == Widget::Select) {
		menu()->addAction( "choisir", this, SLOT( slot_simple_select() ) );
		menu()->addAction( "fusionner", this, SLOT( slot_simple_replace() ) );
	}
	menu()->addAction( "Attribuer des paramètres MIDI", this, SLOT( slot_assign_midi_parameters() ) );

	//QVBoxLayout* _layout = new QVBoxLayout( this );

	if ( _in[0] == _out[0] ) {
		disp_name = new QLabel( QString( "<qt><center>%1</center></qt>" ).arg( _in[0] ), this );
		_layout->addWidget(disp_name, 0);
	}

	bt_mute->setStyleSheet("background-color:rgb(243,250,139)");
	_layout->addWidget(bt_mute);
	connect(bt_mute,SIGNAL(clicked()),this,SLOT(mute()));

	_poti = new JackMix::GUI::Knob(
		amptodb( backend()->getVolume( _in[0], _out[0] ) ),
		dbmin, dbmax, 2, 3, this );
	_layout->addWidget( _poti, 100 );

	connect( _poti, SIGNAL( valueChanged( double ) ), this, SLOT( emitvalue( double ) ) );
	connect( _poti, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _poti, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );
	
	midi_params.append(0);        // Initial MIDI paramater number
	midi_delegates.append(_poti); //   for the potentiometer
	//qDebug()<<"There are "<<midi_delegates.size()<<" Midi delegates";

	// Now construct the parameter setting menu
	_cca = new JackMix::GUI::MidiControlChannelAssigner(QString("Définir les paramètres de contrôle MIDI"),
	                                                     "<qt>" + _in[0] + " &rarr; "  + _out[0] + "</qt>",
	                                                     QStringList() << "Gagner",
	                                                     midi_params,
		                                             this
	                                                    );
	connect( _cca, SIGNAL(assignParameters(QList<int>)), this, SLOT(update_midi_parameters(QList<int>)) );
}
AuxElement_FR::~AuxElement_FR() {
}

void AuxElement_FR::emitvalue( double n ) {
	backend()->setVolume( _in[0], _out[0], dbtoamp( n ) );
}
void AuxElement_FR::mute(){
	delete _poti;
	_poti = new JackMix::GUI::Knob(
			amptodb( backend()->getVolume( _in[0], _out[0] ) ),
			0, 0, 2, 3, this );
	_layout->addWidget( _poti, 100 );
	bt_mute->setText("Rétablir le son ");
	connect(bt_mute,SIGNAL(clicked()),this,SLOT(cancel_mute()));
}
void AuxElement_FR::cancel_mute(){
	delete _poti;
	_poti = new JackMix::GUI::Knob(
			amptodb( backend()->getVolume( _in[0], _out[0] ) ),
			dbmin, dbmax, 2, 3, this );
	_layout->addWidget( _poti, 100 );
	bt_mute->setText("Muet");
	connect(bt_mute,SIGNAL(clicked()),this,SLOT(mute()));
}
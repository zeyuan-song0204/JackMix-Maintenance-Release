#include "stereo_elements.h"
#include "stereo_elements.moc"

#include "knob.h"
#include "slider.h"
#include "midicontrolchannelassigner.h"

#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtCore/QList>
#include <QtWidgets/QLabel>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>

#include <controlsender.h>

using namespace JackMix;
using namespace JackMix::MixerElements;
using namespace JackMix::MixingMatrix;

/// The Factory for creating this things...
//EN
class StereoFactory : public JackMix::MixingMatrix::ElementFactory
{
public:
	StereoFactory() : ElementFactory() { globaldebug(); }
	~StereoFactory() {}

	QStringList canCreate() const {
		return QStringList()<<"Mono2StereoElement"<<"Stereo2StereoElement";
	}
	QStringList canCreate( int in, int out ) const {
		if ( in==1 && out==2 ) return QStringList()<<"Mono2StereoElement";
		if ( in==2 && out==2 ) return QStringList()<<"Stereo2StereoElement";
		return QStringList();
	}

	Element* create( QString type , QStringList ins, QStringList outs, Widget* p, const char* n=0 ) {
		if ( type=="Mono2StereoElement" )
			return new Mono2StereoElement( ins, outs, p, n );
		if ( type=="Stereo2StereoElement" )
			return new Stereo2StereoElement( ins, outs, p, n );
		return 0;
	}
};
//CN
class StereoFactory_CN : public JackMix::MixingMatrix::ElementFactory
{
public:
	StereoFactory_CN() : ElementFactory() { globaldebug(); }
	~StereoFactory_CN() {}

	QStringList canCreate() const {
		return QStringList()<<"Mono2StereoElement_CN"<<"Stereo2StereoElement_CN";
	}
	QStringList canCreate( int in, int out ) const {
		if ( in==1 && out==2 ) return QStringList()<<"Mono2StereoElement_CN";
		if ( in==2 && out==2 ) return QStringList()<<"Stereo2StereoElement_CN";
		return QStringList();
	}

	Element* create( QString type , QStringList ins, QStringList outs, Widget* p, const char* n=0 ) {
		if ( type=="Mono2StereoElement_CN" )
			return new Mono2StereoElement_CN( ins, outs, p, n );
		if ( type=="Stereo2StereoElement_CN" )
			return new Stereo2StereoElement_CN( ins, outs, p, n );
		return 0;
	}
};
//FR
class StereoFactory_FR : public JackMix::MixingMatrix::ElementFactory
{
public:
	StereoFactory_FR() : ElementFactory() { globaldebug(); }
	~StereoFactory_FR() {}

	QStringList canCreate() const {
		return QStringList()<<"Mono2StereoElement_FR"<<"Stereo2StereoElement_FR";
	}
	QStringList canCreate( int in, int out ) const {
		if ( in==1 && out==2 ) return QStringList()<<"Mono2StereoElement_FR";
		if ( in==2 && out==2 ) return QStringList()<<"Stereo2StereoElement_FR";
		return QStringList();
	}

	Element* create( QString type , QStringList ins, QStringList outs, Widget* p, const char* n=0 ) {
		if ( type=="Mono2StereoElement_FR" )
			return new Mono2StereoElement_FR( ins, outs, p, n );
		if ( type=="Stereo2StereoElement_FR" )
			return new Stereo2StereoElement_FR( ins, outs, p, n );
		return 0;
	}
};

void MixerElements::init_stereo_elements() {
	new StereoFactory();
}
void MixerElements::init_stereo_elements_CN() {
	new StereoFactory_CN();
}
void MixerElements::init_stereo_elements_FR() {
	new StereoFactory_FR();
}


//CN
Mono2StereoElement_CN::Mono2StereoElement_CN( QStringList inchannel, QStringList outchannels, MixingMatrix::Widget* p, const char* n )
	: Element( inchannel, outchannels, p, n )
	, dB2VolCalc( -42, 6 )
	, _balance_value( 0 )
	, _volume_value( 0 )
{
	//qDebug( "Mono2StereoElement::Mono2StereoElement()" );
	double left = backend()->getVolume( _in[0], _out[0] );
	double right = backend()->getVolume( _in[0], _out[1] );
	//qDebug( " volumes: %f, %f", left, right );
	if ( left>right ) {
		_volume_value = left;
		_balance_value = right-left;
	} else {
		_volume_value = right;
		_balance_value = right-left;
	}
	//qDebug( " values: %f, %f", _volume_value, _balance_value );
	QVBoxLayout* _layout = new QVBoxLayout( this );
	_layout->setMargin( 0 );
	_layout->setSpacing( 0 );
  
	menu()->addAction( "选择", this, SLOT( slot_simple_select() ) );
	menu()->addAction( "合并", this, SLOT( slot_simple_replace() ) );
        menu()->addAction( "分解", this, SLOT( slot_simple_explode() ) );
	menu()->addAction( "分配 MIDI 参数", this, SLOT( slot_assign_midi_parameters() ) );
	
	_balance = new JackMix::GUI::Knob( _balance_value, -1, 1, 2, 0.1, this, "%1" );
	_layout->addWidget( _balance, 10 );
	connect( _balance, SIGNAL( valueChanged( double ) ), this, SLOT( balance( double ) ) );
	connect( _balance, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _balance, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );
	_volume = new JackMix::GUI::Slider( amptodb( _volume_value ), dbmin, dbmax, 2, 3, this );
	_layout->addWidget( _volume, 20 );
	connect( _volume, SIGNAL( valueChanged( double ) ), this, SLOT( volume( double ) ) );
	connect( _volume, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _volume, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );

	// WATCH OUT: Order of initialisation is really important!
	// Make sure all the widgets are contructed before adding them to the delegates list

	// Initial MIDI parameters and associated AbstractSliders
	midi_params.append(0);
	midi_delegates.append(_volume);
	midi_params.append(0);
	midi_delegates.append(_balance);

	// Now construct the parameter setting menu
	_cca = new JackMix::GUI::MidiControlChannelAssigner(QString("设置 MIDI 控制参数"),
	                                                     "<qt>" + _in[0] + " &rarr; ("  + _out[0] + "/" + _out[1] + ")</qt>",
	                                                     QStringList() << "增益" << "声相",
	                                                     midi_params,
	                                                     this
	                                                    );
	connect( _cca, SIGNAL(assignParameters(QList<int>)), this, SLOT(update_midi_parameters(QList<int>)) );
	QPushButton *button1=new QPushButton("返回");
	_layout->addWidget(button1);
	connect(button1,SIGNAL(clicked()),this,SLOT(slot_simple_explode()));
}
Mono2StereoElement_CN::~Mono2StereoElement_CN() {
}

void Mono2StereoElement_CN::balance( double n ) {
	//qDebug( "Mono2StereoElement::balance( double %f )", n );
	_balance_value = n;
	calculateVolumes();
	_balance->value( n );
	emit valueChanged( this, QString( "balance" ) );
}
void Mono2StereoElement_CN::volume( double n ) {
	//qDebug( "Mono2StereoElement::volume( double %f )", n );
	_volume_value = n;
	calculateVolumes();
	_volume->value( n );
	emit valueChanged( this, QString( "volume" ) );
}

void Mono2StereoElement_CN::calculateVolumes() {
	double left, right;
		left = dbtoamp( _volume_value );
		right = dbtoamp( _volume_value );
	if ( _balance_value > 0 )
		left = dbtoamp( _volume_value )*( 1-_balance_value );
	if ( _balance_value < 0 )
		right = dbtoamp( _volume_value )*( 1+_balance_value );
	backend()->setVolume( _in[0], _out[0], left );
	backend()->setVolume( _in[0], _out[1], right );
}




Stereo2StereoElement_CN::Stereo2StereoElement_CN( QStringList inchannels, QStringList outchannels, MixingMatrix::Widget* p, const char* n )
	: Element( inchannels, outchannels, p, n )
	, dB2VolCalc( -42, 6 )
	, _balance_value( 0 )
	, _volume_value( 0 )
{
	backend()->setVolume( _in[0], _out[1], 0 );
	backend()->setVolume( _in[1], _out[0], 0 );
	double left = backend()->getVolume( _in[0], _out[0] );
	double right = backend()->getVolume( _in[1], _out[1] );
	if ( left>right )
		_volume_value = left;
	else
		_volume_value = right;
	_balance_value = qMin( qMax( right-left, -1.0 ), 1.0 );
	QGridLayout* _layout = new QGridLayout( this );
	_layout->setSpacing( 1 );
	_layout->setMargin( 2 );
	_balance_widget = new JackMix::GUI::Slider( _balance_value, -1, 1, 2, 0.1, this, "%1" );
	_layout->addWidget( _balance_widget, 0,0 );
	_layout->setRowStretch( 0, 0 );
	connect( _balance_widget, SIGNAL( valueChanged( double ) ), this, SLOT( balance( double ) ) );
	connect( _balance_widget, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _balance_widget, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );
	_volume_widget = new JackMix::GUI::Slider( amptodb( _volume_value ), dbmin, dbmax, 1, 3, this );
	_layout->addWidget( _volume_widget, 1,0 );
	_layout->setRowStretch( 1, 255 );
	connect( _volume_widget, SIGNAL( valueChanged( double ) ), this, SLOT( volume( double ) ) );
	connect( _volume_widget, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _volume_widget, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );

	QAction *toggle = new QAction( "切换", this );
	connect( toggle, SIGNAL( triggered() ), this, SLOT( slot_simple_select() ) );
	menu()->addAction( toggle );
	QAction *replace = new QAction( "合并", this );
	connect( replace, SIGNAL( triggered() ), this, SLOT( slot_simple_replace() ) );
	menu()->addAction( replace );
	QAction *explode = new QAction( "分解", this );
	connect( explode, SIGNAL( triggered() ), this, SLOT( slot_simple_explode() ) );
	menu()->addAction( explode );

	QAction *assign = new QAction( "分配 MIDI 参数", this );
	connect( assign, SIGNAL( triggered() ), this, SLOT( slot_assign_midi_parameters() ) );
	menu()->addAction( assign );
	
	// WATCH OUT: Order of initialisation is really important!
	// Make sure all the widgets are contructed before adding them to the delegates list

	// Initial MIDI parameters and associated AbstractSliders
	midi_params.append(0);
	midi_delegates.append(_volume_widget);
	midi_params.append(0);
	midi_delegates.append(_balance_widget);

	// Now construct the parameter setting menu
	_cca = new JackMix::GUI::MidiControlChannelAssigner(QString("设置 MIDI 控制参数"),
	                                                     "<qt>(" + _in[0] + "/" + _in[1] +
	                                                         ") &rarr; ("  + _out[0] + "/" + _out[1] + ")</qt>",
	                                                     QStringList() << "增益" << "声音叠化",
	                                                     midi_params,
	                                                     this
	                                                    );
	connect( _cca, SIGNAL(assignParameters(QList<int>)), this, SLOT(update_midi_parameters(QList<int>)) );
	QPushButton *button1=new QPushButton("返回");
	_layout->addWidget(button1);
	connect(button1,SIGNAL(clicked()),this,SLOT(slot_simple_explode()));

}
Stereo2StereoElement_CN::~Stereo2StereoElement_CN() {
}

void Stereo2StereoElement_CN::balance( double n ) {
	//qDebug( "Mono2StereoElement::balance( double %f )", n );
	_balance_value = n;
	_balance_widget->value( n );
	calculateVolumes();
	emit valueChanged( this, QString( "balance" ) );
}
void Stereo2StereoElement_CN::volume( double n ) {
	//qDebug( "Mono2StereoElement::volume( double %f )", n );
	_volume_value = n;
	_volume_widget->value( n );
	calculateVolumes();
	emit valueChanged( this, QString( "volume" ) );
}

void Stereo2StereoElement_CN::calculateVolumes() {
	double left, right;
		left = dbtoamp( _volume_value );
		right = dbtoamp( _volume_value );
	if ( _balance_value > 0 )
		left = dbtoamp( _volume_value )*( 1-_balance_value );
	if ( _balance_value < 0 )
		right = dbtoamp( _volume_value )*( 1+_balance_value );
	backend()->setVolume( _in[0], _out[0], left );
	backend()->setVolume( _in[1], _out[1], right );
}
//FR
Mono2StereoElement_FR::Mono2StereoElement_FR( QStringList inchannel, QStringList outchannels, MixingMatrix::Widget* p, const char* n )
	: Element( inchannel, outchannels, p, n )
	, dB2VolCalc( -42, 6 )
	, _balance_value( 0 )
	, _volume_value( 0 )
{
	//qDebug( "Mono2StereoElement::Mono2StereoElement()" );
	double left = backend()->getVolume( _in[0], _out[0] );
	double right = backend()->getVolume( _in[0], _out[1] );
	//qDebug( " volumes: %f, %f", left, right );
	if ( left>right ) {
		_volume_value = left;
		_balance_value = right-left;
	} else {
		_volume_value = right;
		_balance_value = right-left;
	}
	//qDebug( " values: %f, %f", _volume_value, _balance_value );
	QVBoxLayout* _layout = new QVBoxLayout( this );
	_layout->setMargin( 0 );
	_layout->setSpacing( 0 );
  
	menu()->addAction( "choisir", this, SLOT( slot_simple_select() ) );
	menu()->addAction( "fusionner", this, SLOT( slot_simple_replace() ) );
        menu()->addAction( "panne", this, SLOT( slot_simple_explode() ) );
	menu()->addAction( "Attribuer des paramètres de contrôle MIDI", this, SLOT( slot_assign_midi_parameters() ) );
	
	_balance = new JackMix::GUI::Knob( _balance_value, -1, 1, 2, 0.1, this, "%1" );
	_layout->addWidget( _balance, 10 );
	connect( _balance, SIGNAL( valueChanged( double ) ), this, SLOT( balance( double ) ) );
	connect( _balance, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _balance, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );
	_volume = new JackMix::GUI::Slider( amptodb( _volume_value ), dbmin, dbmax, 2, 3, this );
	_layout->addWidget( _volume, 20 );
	connect( _volume, SIGNAL( valueChanged( double ) ), this, SLOT( volume( double ) ) );
	connect( _volume, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _volume, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );

	// WATCH OUT: Order of initialisation is really important!
	// Make sure all the widgets are contructed before adding them to the delegates list

	// Initial MIDI parameters and associated AbstractSliders
	midi_params.append(0);
	midi_delegates.append(_volume);
	midi_params.append(0);
	midi_delegates.append(_balance);

	// Now construct the parameter setting menu
	_cca = new JackMix::GUI::MidiControlChannelAssigner(QString(" Définir les paramètres de contrôle MIDI"),
	                                                     "<qt>" + _in[0] + " &rarr; ("  + _out[0] + "/" + _out[1] + ")</qt>",
	                                                     QStringList() << "Gagner" << "Phase sonore",
	                                                     midi_params,
	                                                     this
	                                                    );
	connect( _cca, SIGNAL(assignParameters(QList<int>)), this, SLOT(update_midi_parameters(QList<int>)) );
	QPushButton *button1=new QPushButton("revenir");
	_layout->addWidget(button1);
	connect(button1,SIGNAL(clicked()),this,SLOT(slot_simple_explode()));
}
Mono2StereoElement_FR::~Mono2StereoElement_FR() {
}

void Mono2StereoElement_FR::balance( double n ) {
	//qDebug( "Mono2StereoElement::balance( double %f )", n );
	_balance_value = n;
	calculateVolumes();
	_balance->value( n );
	emit valueChanged( this, QString( "balance" ) );
}
void Mono2StereoElement_FR::volume( double n ) {
	//qDebug( "Mono2StereoElement::volume( double %f )", n );
	_volume_value = n;
	calculateVolumes();
	_volume->value( n );
	emit valueChanged( this, QString( "volume" ) );
}

void Mono2StereoElement_FR::calculateVolumes() {
	double left, right;
		left = dbtoamp( _volume_value );
		right = dbtoamp( _volume_value );
	if ( _balance_value > 0 )
		left = dbtoamp( _volume_value )*( 1-_balance_value );
	if ( _balance_value < 0 )
		right = dbtoamp( _volume_value )*( 1+_balance_value );
	backend()->setVolume( _in[0], _out[0], left );
	backend()->setVolume( _in[0], _out[1], right );
}




Stereo2StereoElement_FR::Stereo2StereoElement_FR( QStringList inchannels, QStringList outchannels, MixingMatrix::Widget* p, const char* n )
	: Element( inchannels, outchannels, p, n )
	, dB2VolCalc( -42, 6 )
	, _balance_value( 0 )
	, _volume_value( 0 )
{
	backend()->setVolume( _in[0], _out[1], 0 );
	backend()->setVolume( _in[1], _out[0], 0 );
	double left = backend()->getVolume( _in[0], _out[0] );
	double right = backend()->getVolume( _in[1], _out[1] );
	if ( left>right )
		_volume_value = left;
	else
		_volume_value = right;
	_balance_value = qMin( qMax( right-left, -1.0 ), 1.0 );
	QGridLayout* _layout = new QGridLayout( this );
	_layout->setSpacing( 1 );
	_layout->setMargin( 2 );
	_balance_widget = new JackMix::GUI::Slider( _balance_value, -1, 1, 2, 0.1, this, "%1" );
	_layout->addWidget( _balance_widget, 0,0 );
	_layout->setRowStretch( 0, 0 );
	connect( _balance_widget, SIGNAL( valueChanged( double ) ), this, SLOT( balance( double ) ) );
	connect( _balance_widget, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _balance_widget, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );
	_volume_widget = new JackMix::GUI::Slider( amptodb( _volume_value ), dbmin, dbmax, 1, 3, this );
	_layout->addWidget( _volume_widget, 1,0 );
	_layout->setRowStretch( 1, 255 );
	connect( _volume_widget, SIGNAL( valueChanged( double ) ), this, SLOT( volume( double ) ) );
	connect( _volume_widget, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _volume_widget, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );

	QAction *toggle = new QAction( "Changer", this );
	connect( toggle, SIGNAL( triggered() ), this, SLOT( slot_simple_select() ) );
	menu()->addAction( toggle );
	QAction *replace = new QAction( "fusionner", this );
	connect( replace, SIGNAL( triggered() ), this, SLOT( slot_simple_replace() ) );
	menu()->addAction( replace );
	QAction *explode = new QAction( "panne", this );
	connect( explode, SIGNAL( triggered() ), this, SLOT( slot_simple_explode() ) );
	menu()->addAction( explode );

	QAction *assign = new QAction( "Attribuer des paramètres MIDI", this );
	connect( assign, SIGNAL( triggered() ), this, SLOT( slot_assign_midi_parameters() ) );
	menu()->addAction( assign );
	
	// WATCH OUT: Order of initialisation is really important!
	// Make sure all the widgets are contructed before adding them to the delegates list

	// Initial MIDI parameters and associated AbstractSliders
	midi_params.append(0);
	midi_delegates.append(_volume_widget);
	midi_params.append(0);
	midi_delegates.append(_balance_widget);

	// Now construct the parameter setting menu
	_cca = new JackMix::GUI::MidiControlChannelAssigner(QString("Définir les paramètres de contrôle MIDI"),
	                                                     "<qt>(" + _in[0] + "/" + _in[1] +
	                                                         ") &rarr; ("  + _out[0] + "/" + _out[1] + ")</qt>",
	                                                     QStringList() << "Gagner" << "Fondu sonore",
	                                                     midi_params,
	                                                     this
	                                                    );
	connect( _cca, SIGNAL(assignParameters(QList<int>)), this, SLOT(update_midi_parameters(QList<int>)) );
	QPushButton *button1=new QPushButton("revenir");
	_layout->addWidget(button1);
	connect(button1,SIGNAL(clicked()),this,SLOT(slot_simple_explode()));

}
Stereo2StereoElement_FR::~Stereo2StereoElement_FR() {
}

void Stereo2StereoElement_FR::balance( double n ) {
	//qDebug( "Mono2StereoElement::balance( double %f )", n );
	_balance_value = n;
	_balance_widget->value( n );
	calculateVolumes();
	emit valueChanged( this, QString( "balance" ) );
}
void Stereo2StereoElement_FR::volume( double n ) {
	//qDebug( "Mono2StereoElement::volume( double %f )", n );
	_volume_value = n;
	_volume_widget->value( n );
	calculateVolumes();
	emit valueChanged( this, QString( "volume" ) );
}

void Stereo2StereoElement_FR::calculateVolumes() {
	double left, right;
		left = dbtoamp( _volume_value );
		right = dbtoamp( _volume_value );
	if ( _balance_value > 0 )
		left = dbtoamp( _volume_value )*( 1-_balance_value );
	if ( _balance_value < 0 )
		right = dbtoamp( _volume_value )*( 1+_balance_value );
	backend()->setVolume( _in[0], _out[0], left );
	backend()->setVolume( _in[1], _out[1], right );
}

//EN
Mono2StereoElement::Mono2StereoElement( QStringList inchannel, QStringList outchannels, MixingMatrix::Widget* p, const char* n )
	: Element( inchannel, outchannels, p, n )
	, dB2VolCalc( -42, 6 )
	, _balance_value( 0 )
	, _volume_value( 0 )
{
	//qDebug( "Mono2StereoElement::Mono2StereoElement()" );
	double left = backend()->getVolume( _in[0], _out[0] );
	double right = backend()->getVolume( _in[0], _out[1] );
	//qDebug( " volumes: %f, %f", left, right );
	if ( left>right ) {
		_volume_value = left;
		_balance_value = right-left;
	} else {
		_volume_value = right;
		_balance_value = right-left;
	}
	//qDebug( " values: %f, %f", _volume_value, _balance_value );
	QVBoxLayout* _layout = new QVBoxLayout( this );
	_layout->setMargin( 0 );
	_layout->setSpacing( 0 );
  
	menu()->addAction( "Select", this, SLOT( slot_simple_select() ) );
	menu()->addAction( "Replace", this, SLOT( slot_simple_replace() ) );
    menu()->addAction( "Explode", this, SLOT( slot_simple_explode() ) );
	menu()->addAction( "Assign MIDI Parameter", this, SLOT( slot_assign_midi_parameters() ) );
	
	_balance = new JackMix::GUI::Knob( _balance_value, -1, 1, 2, 0.1, this, "%1" );
	_layout->addWidget( _balance, 10 );
	connect( _balance, SIGNAL( valueChanged( double ) ), this, SLOT( balance( double ) ) );
	connect( _balance, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _balance, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );
	_volume = new JackMix::GUI::Slider( amptodb( _volume_value ), dbmin, dbmax, 2, 3, this );
	_layout->addWidget( _volume, 20 );
	connect( _volume, SIGNAL( valueChanged( double ) ), this, SLOT( volume( double ) ) );
	connect( _volume, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _volume, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );

	// WATCH OUT: Order of initialisation is really important!
	// Make sure all the widgets are contructed before adding them to the delegates list

	// Initial MIDI parameters and associated AbstractSliders
	midi_params.append(0);
	midi_delegates.append(_volume);
	midi_params.append(0);
	midi_delegates.append(_balance);

	// Now construct the parameter setting menu
	_cca = new JackMix::GUI::MidiControlChannelAssigner(QString("Set MIDI control parameter"),
	                                                     "<qt>" + _in[0] + " &rarr; ("  + _out[0] + "/" + _out[1] + ")</qt>",
	                                                     QStringList() << "Gain" << "Pan",
	                                                     midi_params,
	                                                     this
	                                                    );
	connect( _cca, SIGNAL(assignParameters(QList<int>)), this, SLOT(update_midi_parameters(QList<int>)) );
	QPushButton *button1=new QPushButton("return");
	_layout->addWidget(button1);
	connect(button1,SIGNAL(clicked()),this,SLOT(slot_simple_explode()));
	
	

}
Mono2StereoElement::~Mono2StereoElement() {
}

void Mono2StereoElement::balance( double n ) {
	//qDebug( "Mono2StereoElement::balance( double %f )", n );
	_balance_value = n;
	calculateVolumes();
	_balance->value( n );
	emit valueChanged( this, QString( "balance" ) );
}
void Mono2StereoElement::volume( double n ) {
	//qDebug( "Mono2StereoElement::volume( double %f )", n );
	_volume_value = n;
	calculateVolumes();
	_volume->value( n );
	emit valueChanged( this, QString( "volume" ) );
}

void Mono2StereoElement::calculateVolumes() {
	double left, right;
		left = dbtoamp( _volume_value );
		right = dbtoamp( _volume_value );
	if ( _balance_value > 0 )
		left = dbtoamp( _volume_value )*( 1-_balance_value );
	if ( _balance_value < 0 )
		right = dbtoamp( _volume_value )*( 1+_balance_value );
	backend()->setVolume( _in[0], _out[0], left );
	backend()->setVolume( _in[0], _out[1], right );
}




Stereo2StereoElement::Stereo2StereoElement( QStringList inchannels, QStringList outchannels, MixingMatrix::Widget* p, const char* n )
	: Element( inchannels, outchannels, p, n )
	, dB2VolCalc( -42, 6 )
	, _balance_value( 0 )
	, _volume_value( 0 )
{
	backend()->setVolume( _in[0], _out[1], 0 );
	backend()->setVolume( _in[1], _out[0], 0 );
	double left = backend()->getVolume( _in[0], _out[0] );
	double right = backend()->getVolume( _in[1], _out[1] );
	if ( left>right )
		_volume_value = left;
	else
		_volume_value = right;
	_balance_value = qMin( qMax( right-left, -1.0 ), 1.0 );
	QGridLayout* _layout = new QGridLayout( this );
	_layout->setSpacing( 1 );
	_layout->setMargin( 2 );
	_balance_widget = new JackMix::GUI::Slider( _balance_value, -1, 1, 2, 0.1, this, "%1" );
	_layout->addWidget( _balance_widget, 0,0 );
	_layout->setRowStretch( 0, 0 );
	connect( _balance_widget, SIGNAL( valueChanged( double ) ), this, SLOT( balance( double ) ) );
	connect( _balance_widget, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _balance_widget, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );
	_volume_widget = new JackMix::GUI::Slider( amptodb( _volume_value ), dbmin, dbmax, 1, 3, this );
	_layout->addWidget( _volume_widget, 1,0 );
	_layout->setRowStretch( 1, 255 );
	connect( _volume_widget, SIGNAL( valueChanged( double ) ), this, SLOT( volume( double ) ) );
	connect( _volume_widget, SIGNAL( select() ), this, SLOT( slot_simple_select() ) );
	connect( _volume_widget, SIGNAL( replace() ), this, SLOT( slot_simple_replace() ) );

	QAction *toggle = new QAction( "Toggle Selection", this );
	connect( toggle, SIGNAL( triggered() ), this, SLOT( slot_simple_select() ) );
	menu()->addAction( toggle );
	QAction *replace = new QAction( "Replace", this );
	connect( replace, SIGNAL( triggered() ), this, SLOT( slot_simple_replace() ) );
	menu()->addAction( replace );
	QAction *explode = new QAction( "Explode", this );
	connect( explode, SIGNAL( triggered() ), this, SLOT( slot_simple_explode() ) );
	menu()->addAction( explode );

	QAction *assign = new QAction( "Assign MIDI Parameter", this );
	connect( assign, SIGNAL( triggered() ), this, SLOT( slot_assign_midi_parameters() ) );
	menu()->addAction( assign );
	
	// WATCH OUT: Order of initialisation is really important!
	// Make sure all the widgets are contructed before adding them to the delegates list

	// Initial MIDI parameters and associated AbstractSliders
	midi_params.append(0);
	midi_delegates.append(_volume_widget);
	midi_params.append(0);
	midi_delegates.append(_balance_widget);

	// Now construct the parameter setting menu
	_cca = new JackMix::GUI::MidiControlChannelAssigner(QString("Set MIDI control parameter"),
	                                                     "<qt>(" + _in[0] + "/" + _in[1] +
	                                                         ") &rarr; ("  + _out[0] + "/" + _out[1] + ")</qt>",
	                                                     QStringList() << "Gain" << "Cross-fade",
	                                                     midi_params,
	                                                     this
	                                                    );
	connect( _cca, SIGNAL(assignParameters(QList<int>)), this, SLOT(update_midi_parameters(QList<int>)) );
	QPushButton *button1=new QPushButton("return");
	_layout->addWidget(button1);
	connect(button1,SIGNAL(clicked()),this,SLOT(slot_simple_explode()));

}
Stereo2StereoElement::~Stereo2StereoElement() {
}

void Stereo2StereoElement::balance( double n ) {
	//qDebug( "Mono2StereoElement::balance( double %f )", n );
	_balance_value = n;
	_balance_widget->value( n );
	calculateVolumes();
	emit valueChanged( this, QString( "balance" ) );
}
void Stereo2StereoElement::volume( double n ) {
	//qDebug( "Mono2StereoElement::volume( double %f )", n );
	_volume_value = n;
	_volume_widget->value( n );
	calculateVolumes();
	emit valueChanged( this, QString( "volume" ) );
}

void Stereo2StereoElement::calculateVolumes() {
	double left, right;
		left = dbtoamp( _volume_value );
		right = dbtoamp( _volume_value );
	if ( _balance_value > 0 )
		left = dbtoamp( _volume_value )*( 1-_balance_value );
	if ( _balance_value < 0 )
		right = dbtoamp( _volume_value )*( 1+_balance_value );
	backend()->setVolume( _in[0], _out[0], left );
	backend()->setVolume( _in[1], _out[1], right );
}

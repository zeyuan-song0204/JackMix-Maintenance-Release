#ifndef AUX_ELEMENTS_H
#define AUX_ELEMENTS_H

#include <QtCore/QVector>

#include <mixingmatrix.h>
#include <dbvolcalc.h>
#include <controlreceiver.h>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QString>
namespace JackMix {

namespace GUI {
	class Knob;
	class MidiControlChannelAssigner;
}

namespace MixerElements {
	
/**
 * Simpliest form of a control connecting one input with one output.
 */
	//EN
class AuxElement : public JackMix::MixingMatrix::Element
                 , public dB2VolCalc {
Q_OBJECT
public:
	AuxElement( QStringList inchannel, QStringList outchannel, MixingMatrix::Widget*, const char* =0 );
	~AuxElement();
	

	int inchannels() const { return 1; }
	int outchannels() const { return 1; }
	
	void setIndicator(const QColor& c);


public slots:
	void emitvalue( double );
	void mute();
	void cancel_mute();
	
	

private:
	JackMix::GUI::Knob *_poti;
	QPushButton *bt_mute=new QPushButton("Mute");
	
	//QPushButton *bt_muted=new QPushButton("Muted");
	
	QVBoxLayout* _layout = new QVBoxLayout(this);

	// void generate_bt_mute(){QPushButton *bt_mute=new QPushButton("Mute");}
	// void generate_bt_muted(){QPushButton *bt_muted=new QPushButton("Muted");}
};
//CN
class AuxElement_CN : public JackMix::MixingMatrix::Element
                 , public dB2VolCalc {
Q_OBJECT
public:
	AuxElement_CN( QStringList inchannel, QStringList outchannel, MixingMatrix::Widget*, const char* =0 );
	~AuxElement_CN();

	int inchannels() const { return 1; }
	int outchannels() const { return 1; }
	
	void setIndicator(const QColor& c);
 
public slots:
	void emitvalue( double );
	void mute();
	void cancel_mute();

private:
	JackMix::GUI::Knob *_poti;
	QPushButton *bt_mute=new QPushButton("静音");
	QVBoxLayout* _layout = new QVBoxLayout(this);
	
};
//FR
class AuxElement_FR : public JackMix::MixingMatrix::Element
                 , public dB2VolCalc {
Q_OBJECT
public:
	AuxElement_FR( QStringList inchannel, QStringList outchannel, MixingMatrix::Widget*, const char* =0 );
	~AuxElement_FR();

	int inchannels() const { return 1; }
	int outchannels() const { return 1; }
	
	void setIndicator(const QColor& c);
 
public slots:
	void emitvalue( double );
	void mute();
	void cancel_mute();

private:
	JackMix::GUI::Knob *_poti;
	QPushButton *bt_mute=new QPushButton("Muet");
	QVBoxLayout* _layout = new QVBoxLayout(this);
};
void init_aux_elements();
void init_aux_elements_CN();
void init_aux_elements_FR();
}; // MixerElements
}; // JackMix

#endif // AUX_ELEMENTS_H

#ifndef STEREO_ELEMENTS_H
#define STEREO_ELEMENTS_H

#include <mixingmatrix.h>
#include <dbvolcalc.h>
#include <controlreceiver.h>

namespace JackMix {

namespace GUI {
	class Slider;
	class Knob;
	class MidiControlChannelAssigner;
}

namespace MixerElements {

class Mono2StereoElement;
class Stereo2StereoElement;
class Mono2StereoElement_CN;
class Stereo2StereoElement_CN;

/**
 * A MonotoStereo control.
 */
//EN
class Mono2StereoElement : public JackMix::MixingMatrix::Element , public dB2VolCalc
{
Q_OBJECT
Q_PROPERTY( double volume READ volume WRITE volume );
Q_PROPERTY( double panorama READ panorama WRITE set_panorama );
public:
	Mono2StereoElement( QStringList, QStringList, MixingMatrix::Widget*, const char* =0 );
	~Mono2StereoElement();

	int inchannels() const { return 1; }
	int outchannels() const { return 2; }

	double volume() const { return _volume_value; }
	double panorama() const { return _balance_value; }
signals:
	void volume_changed( double );
	void panorama_changed( double );
public slots:
	void set_panorama( double n ) { balance( n ); }
	void balance( double );
	void volume( double );
private slots:
	void calculateVolumes();
private:
	QString _inchannel, _outchannel1, _outchannel2;
	JackMix::GUI::Knob *_balance;
	JackMix::GUI::Slider *_volume;
	double _balance_value, _volume_value;
};
//CN
class Mono2StereoElement_CN : public JackMix::MixingMatrix::Element , public dB2VolCalc
{
Q_OBJECT
Q_PROPERTY( double volume READ volume WRITE volume );
Q_PROPERTY( double panorama READ panorama WRITE set_panorama );
public:
	Mono2StereoElement_CN( QStringList, QStringList, MixingMatrix::Widget*, const char* =0 );
	~Mono2StereoElement_CN();

	int inchannels() const { return 1; }
	int outchannels() const { return 2; }

	double volume() const { return _volume_value; }
	double panorama() const { return _balance_value; }
signals:
	void volume_changed( double );
	void panorama_changed( double );
public slots:
	void set_panorama( double n ) { balance( n ); }
	void balance( double );
	void volume( double );
private slots:
	void calculateVolumes();
private:
	QString _inchannel, _outchannel1, _outchannel2;
	JackMix::GUI::Knob *_balance;
	JackMix::GUI::Slider *_volume;
	double _balance_value, _volume_value;
};
//FR
class Mono2StereoElement_FR : public JackMix::MixingMatrix::Element , public dB2VolCalc
{
Q_OBJECT
Q_PROPERTY( double volume READ volume WRITE volume );
Q_PROPERTY( double panorama READ panorama WRITE set_panorama );
public:
	Mono2StereoElement_FR( QStringList, QStringList, MixingMatrix::Widget*, const char* =0 );
	~Mono2StereoElement_FR();

	int inchannels() const { return 1; }
	int outchannels() const { return 2; }

	double volume() const { return _volume_value; }
	double panorama() const { return _balance_value; }
signals:
	void volume_changed( double );
	void panorama_changed( double );
public slots:
	void set_panorama( double n ) { balance( n ); }
	void balance( double );
	void volume( double );
private slots:
	void calculateVolumes();
private:
	QString _inchannel, _outchannel1, _outchannel2;
	JackMix::GUI::Knob *_balance;
	JackMix::GUI::Slider *_volume;
	double _balance_value, _volume_value;
};

/**
 * A StereotoStereo control.
 */
//En
class Stereo2StereoElement : public JackMix::MixingMatrix::Element, public dB2VolCalc
{
Q_OBJECT
Q_PROPERTY( double balance READ getBalance WRITE balance )
Q_PROPERTY( double volume READ getVolume WRITE volume )
public:
	Stereo2StereoElement( QStringList, QStringList, MixingMatrix::Widget*, const char* =0 );
	~Stereo2StereoElement();

	int inchannels() const { return 2; }
	int outchannels() const { return 2; }

	double getVolume() const { return _volume_value; }
	double getBalance() const { return _balance_value; }
private slots:
	void balance( double );
	void volume( double );
	void calculateVolumes();
private:
	QString _inchannel1, _inchannel2, _outchannel1, _outchannel2;
	double _balance_value, _volume_value;
	JackMix::GUI::Slider *_volume_widget, *_balance_widget;
};
//CN
class Stereo2StereoElement_CN : public JackMix::MixingMatrix::Element, public dB2VolCalc
{
Q_OBJECT
Q_PROPERTY( double balance READ getBalance WRITE balance )
Q_PROPERTY( double volume READ getVolume WRITE volume )
public:
	Stereo2StereoElement_CN( QStringList, QStringList, MixingMatrix::Widget*, const char* =0 );
	~Stereo2StereoElement_CN();

	int inchannels() const { return 2; }
	int outchannels() const { return 2; }

	double getVolume() const { return _volume_value; }
	double getBalance() const { return _balance_value; }
private slots:
	void balance( double );
	void volume( double );
	void calculateVolumes();
private:
	QString _inchannel1, _inchannel2, _outchannel1, _outchannel2;
	double _balance_value, _volume_value;
	JackMix::GUI::Slider *_volume_widget, *_balance_widget;
};
//FR
class Stereo2StereoElement_FR : public JackMix::MixingMatrix::Element, public dB2VolCalc
{
Q_OBJECT
Q_PROPERTY( double balance READ getBalance WRITE balance )
Q_PROPERTY( double volume READ getVolume WRITE volume )
public:
	Stereo2StereoElement_FR( QStringList, QStringList, MixingMatrix::Widget*, const char* =0 );
	~Stereo2StereoElement_FR();

	int inchannels() const { return 2; }
	int outchannels() const { return 2; }

	double getVolume() const { return _volume_value; }
	double getBalance() const { return _balance_value; }
private slots:
	void balance( double );
	void volume( double );
	void calculateVolumes();
private:
	QString _inchannel1, _inchannel2, _outchannel1, _outchannel2;
	double _balance_value, _volume_value;
	JackMix::GUI::Slider *_volume_widget, *_balance_widget;
};

void init_stereo_elements();
void init_stereo_elements_CN();
void init_stereo_elements_FR();

}; // MixerElements
}; // JackMix

#endif // STEREO_ELEMENTS_H
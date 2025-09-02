#ifndef WSETTINGS_H
#define WSETTINGS_H

#include <QWidget>

class WSettings : public QWidget
{
	Q_OBJECT

public:
	WSettings(QWidget *parent);
	~WSettings();

public slots:
	Q_INVOKABLE void setControlsEnabled(bool enabled);

};

#endif // WSETTINGS_H

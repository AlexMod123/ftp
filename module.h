#ifndef MODULE_H
 #include <QtWinExtras>

#define FTP_CONTROL_PORT 21
#define FTP_DATA_PORT    20

#include <cstdint>

#include <QMultiMap>
#include <QRegExp>

#include <QObject>
#include <QBitmap>
#include <QString>

#include "tcp\IModuleTCPSess.h" 
#include "ftp_handler.h"
#include "wsettings.h"


class FTP_DataConnection
{
public:
	FTP_DataConnection(int32_t ipServer,uint16_t portServer)
	{
		this->ipServer = ipServer;
		this->portServer = portServer;
	}
	~FTP_DataConnection()
	{}

	QString  m_filename;
	uint32_t ipServer;
	uint16_t portServer;
	time_t   m_last_packet;
};


class FTP_ControlConnection
{
public:
	FTP_ControlConnection(int32_t ipServer, uint32_t ipClient, uint16_t portServer, uint16_t portClient)
	{
		this->ipServer = ipServer;
		this->ipClient = ipClient;
		this->portServer = portServer;
		this->portClient = portClient;
		dataConnection  = nullptr;
	}

	void swapDirection()
	{
		uint32_t tmp32 = ipServer;
		ipServer = ipClient;
		ipClient = tmp32;
		uint16_t tmp16 = portServer;
		portServer = portClient; 
		portClient = tmp16;
	}

	~FTP_ControlConnection()
	{}

	FTP_DataConnection* dataConnection;
	
	QString m_server_name;
	QString m_user;
	QString m_password;

	uint32_t ipServer;
	uint32_t ipClient;
	uint16_t portServer;
	uint16_t portClient;
	time_t m_last_packet;
};


class StreamModule:IModuleTCPSess
{
public:
	StreamModule();
	~StreamModule();

	virtual void __stdcall createModule();
	virtual void __stdcall showForm();

	virtual bool __stdcall initResources();
	virtual bool __stdcall freeResources();

	virtual bool __stdcall processData(unsigned char* d, unsigned int l);
	virtual bool __stdcall processTimeout();
	virtual bool __stdcall processNoData();

	virtual bool __stdcall setParameter(const char* _name, const char* _value, int _type);
	virtual void __stdcall tellParams();

protected:
	WSettings*		w;
	QPixmap       bitmap;

	uint32_t make_key(uint32_t ip_src, uint32_t ip_dst, uint16_t port_src, uint16_t port_dst);

	QMultiMap<uint32_t, FTP_ControlConnection*> control_connections;
	QMap<uint64_t, FTP_DataConnection*>         data_connections;

};

#endif // MODULE_H

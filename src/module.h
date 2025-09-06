#ifndef MODULE_H
#include "tcp/imoduleTCPSess.h"
#ifdef _WIN32
#include <QtWinExtras>
#else

#endif

#define FTP_CONTROL_PORT 21
#define FTP_DATA_PORT    20

#include <cstdint>

#include <QRegExp>

#include "ftp_handler.h"

class FTP_DataConnection
{
public:
	FTP_DataConnection(int32_t ipServer, uint16_t portServer)
	{
		this->ipServer = ipServer;
		this->portServer = portServer;
	}

	~FTP_DataConnection()
	= default;

	QString m_filename;
	uint32_t ipServer;
	uint16_t portServer;
	time_t m_last_packet{};
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
		dataConnection = nullptr;
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
	= default;

	std::shared_ptr<FTP_DataConnection> dataConnection;

	QString m_server_name;
	QString m_user;
	QString m_password;

	uint32_t ipServer;
	uint32_t ipClient;
	uint16_t portServer;
	uint16_t portClient;
	time_t m_last_packet{};
};


class StreamModule final : IModuleTCPSess
{
public:
	StreamModule();
	~StreamModule() override;

	void createModule() override;
	void showForm() override;

	bool initResources() override;
	bool freeResources() override;

	bool processData(unsigned char* d, unsigned int l) override;
	bool processTimeout() override;
	bool processNoData() override;

	bool setParameter(const char* _name, const char* _value, int _type) override;
	void tellParams() override;

protected:
	QPixmap bitmap;

	static uint32_t make_key(uint32_t ip_src, uint32_t ip_dst, uint16_t port_src, uint16_t port_dst);

	QMultiMap<uint32_t, std::shared_ptr<FTP_ControlConnection>> control_connections;
	QMap<uint64_t, std::shared_ptr<FTP_DataConnection>> data_connections;
};

#endif // MODULE_H

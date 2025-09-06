#include "module.h"

auto mName = "FTP";
auto mDescription = "File transfer protocol";
auto mGroup = "[OSI-7] Application Layer";
auto mVersion = "0.0.1";
constexpr bool mIsGenerator = false;
constexpr bool mIsTerminator = false;

void initTranslator()
{
	QObject::tr("FTP");
	QObject::tr("File transfer protocol");
	QObject::tr("[OSI-7] Application Layer");
}

uint32_t ipFromId(StreamIdentify::idtypeIPv4& ip)
{
	return *(uint32_t*)&ip;
}

StreamModule::StreamModule() : IModuleTCPSess(mName, mDescription, mGroup, mVersion, mIsGenerator, mIsTerminator),
                               bitmap(":/img/NG_Module_FTP.png")
{
	//w = nullptr;
#ifdef _WIN32
	h_Bitmap = QtWin::toHBITMAP(bitmap, QtWin::HBitmapPremultipliedAlpha);
#endif
}

void StreamModule::createModule()
{
#ifdef _WIN32
	w = new WSettings(NULL);
	h_SettingsForm = (HWND)w->winId();
#endif
}

void StreamModule::showForm()
{
#ifdef _WIN32
	w->setVisible(true);
#endif
}

StreamModule::~StreamModule()
{
#ifdef _WIN32
	if (w) delete w;
#endif
}

bool StreamModule::initResources()
{
	clear_sessions<FTP_Handler>();
	return true;
}

bool StreamModule::freeResources()
{
	clear_sessions<FTP_Handler>();
	return true;
}

bool StreamModule::processData(unsigned char* d, unsigned int l)
{
	StreamIdentify::idtypeIPv4 ipSrc;
	StreamIdentify::idtypeIPv4 ipDst;

	StreamIdentify::idtypeUInt16 portSrc;
	StreamIdentify::idtypeUInt16 portDst;
	StreamIdentify::idtypeTCPParams tcpParams{};

	bool ret = false;

	ret = kernel->getIdentify(StreamIdentify::STR_IPV4_SRC, &ipSrc);
	if (!ret) return true;
	ret = kernel->getIdentify(StreamIdentify::STR_IPV4_DST, &ipDst);
	if (!ret) return true;
	ret = kernel->getIdentify(StreamIdentify::STR_PORT_SRC, &portSrc);
	if (!ret) return true;
	ret = kernel->getIdentify(StreamIdentify::STR_PORT_DST, &portDst);
	if (!ret) return true;
	ret = kernel->getIdentify(StreamIdentify::STR_TCP_SESSION_PARAMS, &tcpParams);
	if (!ret) return true;

	uint32_t ipSrc32 = ipFromId(ipSrc);
	uint32_t ipDst32 = ipFromId(ipDst);


	if (portSrc == FTP_CONTROL_PORT || portDst == FTP_CONTROL_PORT)
	{
		std::shared_ptr<FTP_ControlConnection> c;
		uint32_t connection_key = make_key(ipSrc32, ipDst32, portSrc, portDst);
		auto it = control_connections.find(connection_key);

		while (it != control_connections.end() && it.key() == connection_key)
		{
			auto it_conn = it.value();

			if (it_conn->ipServer == ipSrc32 && it_conn->ipClient == ipDst32 &&
				it_conn->portServer == portSrc && it_conn->portClient == portDst)
			{
				c = it_conn;
				break;
			}
			if (it_conn->ipServer == ipDst32 && it_conn->ipClient == ipSrc32 &&
				it_conn->portServer == portDst && it_conn->portClient == portSrc)
			{
				c = it_conn;
				break;
			}
			++it;
		}

		if (!c)
		{
			c = std::make_shared<FTP_ControlConnection>(ipSrc32, ipDst32, portSrc, portDst);
			control_connections.insert(connection_key, c);
		}


		c->m_last_packet = time(nullptr);
		QString control_string = QString::fromLatin1((char*)d, l);

		if (control_string.isEmpty()) return true;

		bool isResponse = false;
		int control_code = control_string.left(3).toInt(&isResponse);
		if (isResponse)
		{
			if (ipSrc32 == c->ipServer)
				c->swapDirection();
		}

		if (isResponse)
		{
			if (control_string.startsWith("220 "))
			{
				control_string.remove("220 ");
				c->m_server_name = control_string.remove(" ready").trimmed();
			}
			if (control_string.startsWith("257 "))
			{
				control_string.remove("257 ");
			}
			if (control_string.startsWith("250 "))
			{
				control_string.remove("250 ");
			}

			else if (control_string.startsWith("227 "))
			{
				QRegExp re("^227 .+ \\((\\d{1,3})\\,(\\d{1,3})\\,(\\d{1,3})\\,(\\d{1,3})\\,(\\d{1,3})\\,(\\d{1,3})\\)");
				int pos = re.indexIn(control_string);
				if (pos > -1)
				{
					uint32_t _ip = (re.cap(1).toInt() << 24) + (re.cap(2).toInt() << 16) +
						(re.cap(3).toInt() << 8) + (re.cap(4).toInt());

					uint16_t _port = (re.cap(5).toInt() << 8) + re.cap(6).toInt();

					uint64_t _socket = (uint64_t)_ip * 65536 + _port;

					auto newConn = std::make_shared<FTP_DataConnection>(_ip, _port);
					data_connections[_socket] = newConn;
					c->dataConnection = newConn;
				}
			}
			else if (control_string.startsWith("226 "))
			{
			}
			else if (control_string.startsWith("150 "))
			{
			}
		}
		else
		{
			if (control_string.startsWith("USER "))
			{
				control_string.remove("USER ");
				c->m_user = control_string.trimmed();
			}
			else if (control_string.startsWith("PASS "))
			{
				control_string.remove("USER ");
				c->m_password = control_string.trimmed();
			}
			else if (control_string.startsWith("PORT "))
			{
				QRegExp re(".+ (\\d{1,3})\\,(\\d{1,3})\\,(\\d{1,3})\\,(\\d{1,3})\\,(\\d{1,3})\\,(\\d{1,3})");
				int pos = re.indexIn(control_string);
				if (pos > -1)
				{
					uint32_t _ip = (re.cap(1).toInt() << 24) + (re.cap(2).toInt() << 16) +
						(re.cap(3).toInt() << 8) + (re.cap(4).toInt());

					uint16_t _port = (re.cap(5).toInt() << 8) + re.cap(6).toInt();

					uint64_t _socket = (uint64_t)_ip * 65536 + _port;

					auto newConn = std::make_shared<FTP_DataConnection>(_ip, _port);
					data_connections[_socket] = newConn;
				}
			}
			else if (control_string.startsWith("RETR "))
			{
				if (c->dataConnection)
					c->dataConnection->m_filename = control_string.remove("RETR ");
			}
			else if (control_string.startsWith("STOR "))
			{
				if (c->dataConnection)
					c->dataConnection->m_filename = control_string.remove("STOR ");
			}
			else if (control_string.startsWith("CWD "))
			{
			}
		}
	}
	else
	{
		uint64_t src_socket = (uint64_t)ipSrc32 * 65536 + portSrc;
		uint64_t dst_socket = (uint64_t)ipDst32 * 65536 + portDst;

		bool have_data_connection = data_connections.contains(src_socket) || data_connections.contains(dst_socket);

		if (!have_data_connection && (portDst == FTP_DATA_PORT || portSrc == FTP_DATA_PORT))
		{
			auto newConn = std::make_shared<FTP_DataConnection>(ipSrc32, portSrc);
			data_connections[src_socket] = newConn;
			have_data_connection = true;
		}

		if (have_data_connection)
			tcp<FTP_Handler>(kernel, ipSrc, ipDst, portSrc, portDst, tcpParams, d, l);
	}

	return true;
}

bool StreamModule::processNoData()
{
	return true;
}

bool StreamModule::processTimeout()
{
	timeout_sessions<FTP_Handler>();
	return true;
}

void StreamModule::tellParams()
{
}

bool StreamModule::setParameter(const char* _name, const char* _value, int _type)
{
	return true;
}

std::uint32_t StreamModule::make_key(uint32_t ip_src, uint32_t ip_dst, uint16_t port_src, uint16_t port_dst)
{
	return ip_src + ip_dst + (uint32_t)port_src + (uint32_t)port_dst;
}


extern "C" IModule* getModuleInstance()
{
	return reinterpret_cast<IModule*>(new StreamModule());;
}


extern "C"

void removeModuleInstance(IModule* aVal)
{
	//call CModule destructor
	if (aVal != nullptr)
	{
		delete (StreamModule*)aVal;
	}
}

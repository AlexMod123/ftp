#ifndef FTP_HANDLER_H
#define FTP_HANDLER_H

#include <QDebug>

#include "tcp\tcphandler.h"
#include "methods_files.h"

class FTP_Handler :  TCPHandler
{
public:
	FTP_Handler(IKernel* kernel,uint32_t ip_src,uint32_t ip_dst, uint16_t port_src, uint16_t port_dst);
	~FTP_Handler();

	int  onRequestStream(unsigned char *payload, int payload_len, bool inc, bool push);
	int  onReplyStream(unsigned char *payload, int payload_len, bool inc, bool push);
	void onClose(bool haveFin);
	void createSession();

	void doWrite(unsigned char* payload, int payload_len);

protected:

	int64_t  m_file_handle;
	bool     m_isCorrupted;
};

FTP_Handler::FTP_Handler(IKernel* kernel, uint32_t ip_src, uint32_t ip_dst, uint16_t port_src, uint16_t port_dst)
	: TCPHandler(kernel, ip_src, ip_dst, port_src, port_dst)
{
	m_file_handle = 0;
	m_isCorrupted = false;
	m_result = TCPHANDLER_RESULT_CONTINUE;
}

FTP_Handler::~FTP_Handler()
{
}

int FTP_Handler::onRequestStream( unsigned char *payload, int payload_len, bool inc, bool push )
{
	if (inc) m_isCorrupted = true;
	doWrite(payload,payload_len);
	return payload_len;
}

int FTP_Handler::onReplyStream( unsigned char *payload, int payload_len, bool inc, bool push )
{
	if (inc) m_isCorrupted = true;
	doWrite(payload,payload_len);
	return payload_len;
}

void FTP_Handler::onClose( bool haveFin )
{
	if (!m_isCorrupted && haveFin)
		m_result = TCPHANDLER_RESULT_DONE_OK;
	else
		m_result = TCPHANDLER_RESULT_DONE_FAULT;

	if (m_file_handle)
	{
		StreamIdentify::idtypeUInt16 protocolId = StreamProtocolInfo::APP_LAYER_PROT_FTP;
		m_kernel->putIdentify(StreamIdentify::STR_PROTOCOL_APPLICATION_LAYER,&protocolId);
		
		cacheClose(m_kernel, m_file_handle,m_isCorrupted);
	}
}

void FTP_Handler::createSession()
{
	m_kernel->putIdentify(StreamIdentify::STR_IPV4_SRC, &m_ipAdrSource);
	m_kernel->putIdentify(StreamIdentify::STR_IPV4_DST, &m_ipAdrDestination);

	m_kernel->putIdentify(StreamIdentify::STR_PORT_SRC, &m_portSource);
	m_kernel->putIdentify(StreamIdentify::STR_PORT_DST, &m_portDestination);

	m_file_handle = cacheCreate(m_kernel, "");
}

void FTP_Handler::doWrite( unsigned char* payload, int payload_len )
{
	if (payload_len)
	{
		//ID
		StreamIdentify::idtypeUInt16 protocolId = StreamProtocolInfo::APP_LAYER_PROT_FTP;
		m_kernel->putIdentify(StreamIdentify::STR_PROTOCOL_APPLICATION_LAYER,&protocolId);

		if (!m_file_handle)
			createSession();

		cacheWrite(m_kernel, m_file_handle,FILEOFFSET_CONTINUE,payload,payload_len);	
	}

}


#endif // FTP_HANDLER_H

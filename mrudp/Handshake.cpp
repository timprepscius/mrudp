#include "Handshake.h"
#include "Connection.h"

namespace timprepscius {
namespace mrudp {

Handshake::Handshake(Connection *connection_) :
	connection(connection_)
{
}

void Handshake::initiate()
{
	auto packet = strong<Packet>();
	packet->header.type = H0;
	connection->sender.sendImmediately(packet);
	waitingFor = H1;
}

void Handshake::handlePacket(Packet &packet)
{
	auto readRemoteID = [this](Packet &packet, bool apply) {
		ShortConnectionID remoteID_;
		if (popData(packet, remoteID_))
		{
			if (apply)
			{
				connection->remoteID = remoteID_;
			}
		}
	};

	auto received = packet.header.type;
	if (received == H0)
	{
		auto ack = strong<Packet>();
		ack->header.type = H1;
		ack->header.id = packet.header.id;
	
		connection->send(ack);
		
		if (waitingFor == H0)
			waitingFor = H2;
	}
	else
	if (received == H1)
	{
		if (waitingFor == H1)
		{
			auto packet = strong<Packet>();
			packet->header.type = H2;
			pushData(*packet, connection->localID);
			connection->sender.sendImmediately(packet);
			
			waitingFor = H3;
		}
	}
	else
	if (received == H2)
	{
		readRemoteID(packet, waitingFor == H2);
	
		auto ack = strong<Packet>();
		ack->header.type = H3;
		ack->header.id = packet.header.id;
		pushData(*ack, connection->localID);
	
		connection->send(ack);
		
		if (waitingFor == H2)
		{
			waitingFor = HANDSHAKE_COMPLETE;
			firstNonHandshakePacketID = packet.header.id + 1;
			onHandshakeComplete();
		}
	}
	else
	if (received == H3)
	{
		readRemoteID(packet, waitingFor == H2);

		if (waitingFor == H3)
		{
			waitingFor = HANDSHAKE_COMPLETE;
			onHandshakeComplete();
		}
	}
	else
	{
		// this should never happen
		debug_assert(false);
	}
}

void Handshake::onPacket(Packet &packet)
{
	if (!isHandshake(packet.header.type))
		return;

	handlePacket(packet);
}

void Handshake::onHandshakeComplete()
{
	connection->receiver.open(firstNonHandshakePacketID);
	connection->sender.open();
}

} // namespace
} // namespace
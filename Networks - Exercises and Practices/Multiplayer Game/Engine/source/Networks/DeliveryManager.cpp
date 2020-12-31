#include "Core/Core.h"
#include "DeliveryManager.h"
#include "Replication/ReplicationManagerServer.h"

// TODO(you): Reliability on top of UDP lab session

Delivery* DeliveryManager::WriteSequenceNumber(OutputMemoryStream& packet, DeliveryDelegate* delegate)
{
	++m_NextSeqNumSent;
	packet << m_NextSeqNumSent;

	Delivery delivery;
	delivery.sequenceNumber = m_NextSeqNumSent;
	delivery.dispatchTime = Time.time;

	if (delegate)
		delivery.deliveryDelegate = delegate;
	else {
		delivery.deliveryDelegate = new DeliveryDelegate();
		delivery.createdDelegate = true;
	}
	m_ServerPendingDeliveries.push_back(delivery);

	// TODO (lucho): we should register notif. callbacks into the delivery
	// objects. Basically onSuccess/onFailure (lambdas?)


	return &delivery;
}

bool DeliveryManager::ProcessSequenceNumber(const InputMemoryStream& packet)
{
	// TODO (lucho): Here we should check if the seq. num. arrives in order
	// then, if it does (otherwise gets discarded), we shall add the seq. num. to a
	// list of pending acknowledgements
	uint32 seq_num = 0;
	packet >> seq_num;

	if (seq_num >= m_NextSeqNumExpected)
	{
		m_NextSeqNumExpected = seq_num + 1;
		m_ClientPendingAcks.push_back(seq_num);
		return true;
	}


	// if not discarded, deserialize & process packet normally
	// at the end of frame, or after interval, send a packet with all the acknowledged
	// sequence numbers from all received packets (WritePendingSequenceNumbers)
	return false;
}

void DeliveryManager::WritePendingSequenceNumbers(OutputMemoryStream& packet)
{
	bool processAcks = m_ClientPendingAcks.size() > 0;
	packet << processAcks;

	if (processAcks)
	{
		packet.Write(m_ClientPendingAcks);
		m_ClientPendingAcks.clear();
	}
}

void DeliveryManager::ProcessAckdSequenceNumbers(const InputMemoryStream& packet)
{
	// TODO (lucho): For each seq. num., find the packet delivery info., then
	// invoke its callbacks success/failure as needed and remove delivery	
	std::vector<uint32> pendingAcks;
	packet.Read(pendingAcks);

	for (uint i = 0; i < pendingAcks.size(); ++i)
	{
		auto it = m_ServerPendingDeliveries.begin();
		while (it != m_ServerPendingDeliveries.end())
		{
			if (pendingAcks[i] == it->sequenceNumber)
			{
				it->deliveryDelegate->onDeliverySuccess(it->sequenceNumber);
				if (it->createdDelegate)
					delete it->deliveryDelegate;
				it = m_ServerPendingDeliveries.erase(it);
				break;
			}
			else
				++it;
		}
	}

}

void DeliveryManager::ProcessTimedOutPackets()
{
	// TODO (lucho): For each delivery timed out, we shall call onFailure()
	// and remove the delivery
	// (Each frame, we check for pending deliveries for timeout (ProcessTimedOutPackets))
	auto it = m_ServerPendingDeliveries.begin();
	while (it != m_ServerPendingDeliveries.end())
	{
		if (Time.time - it->dispatchTime > PACKET_DELIVERY_TIMEOUT_SECONDS)
		{
			it->deliveryDelegate->onDeliveryFailure(it->sequenceNumber);
			if (it->createdDelegate)
				delete it->deliveryDelegate;
			it = m_ServerPendingDeliveries.erase(it);
		}
		else
			++it;
	}
}

ServerDelegate::ServerDelegate(ReplicationManagerServer* replicationManager) : DeliveryDelegate(){
	this->replicationManager = replicationManager;
}

void ServerDelegate::onDeliverySuccess(uint32 sequenceNumber) {
	replicationManager->DiscardReplication(sequenceNumber);
}

void ServerDelegate::onDeliveryFailure(uint32 sequenceNumber) {
	replicationManager->ResendReplication(sequenceNumber);
}

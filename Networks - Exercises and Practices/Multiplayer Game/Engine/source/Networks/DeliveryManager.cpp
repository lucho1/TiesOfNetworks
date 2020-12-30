#include "Core/Core.h"
#include "DeliveryManager.h"

// TODO(you): Reliability on top of UDP lab session

DeliveryMessage* DeliveryManager::WriteSequenceNumber(OutputMemoryStream& packet)
{
	++m_NextSeqNumOut;
	packet << m_NextSeqNumOut;

	DeliveryMessage delivery;
	delivery.sequenceNumber = m_NextSeqNumOut;
	delivery.dispatchTime = Time.time;

	// TODO (lucho): we should register notif. callbacks into the delivery
	// objects. Basically onSuccess/onFailure (lambdas?)

	m_PendingDeliveries.push_back(delivery);

	return &delivery;
}

bool DeliveryManager::ProcessSequenceNumber(const InputMemoryStream& packet)
{
	// TODO (lucho): Here we should check if the seq. num. arrives in order
	// then, if it does (otherwise gets discarded), we shall add the seq. num. to a
	// list of pending acknowledgements

	// if not discarded, deserialize & process packet
	// at the end of frame, or after interval, send a packet with all the acknowledged
	// sequence numbers from all received packets (WritePendingSequenceNumbers)
	return false;
}

bool DeliveryManager::HasPendingSequenceNumbers() const
{
	return false;
}

void DeliveryManager::WritePendingSequenceNumbers(OutputMemoryStream& packet)
{
}

void DeliveryManager::ProcessAckdSequenceNumbers(const InputMemoryStream& packet)
{
	// TODO (lucho): For each seq. num., find the packet delivery info., then
	// invoke its callbacks success/failure as needed and remove delivery

	// Each frame, we check for pending deliveries for timeout (ProcessTimedOutPackets)
}

void DeliveryManager::ProcessTimedOutPackets()
{
	// TODO (lucho): For each delivery timed out, we shall call onFailure()
	// and remove the delivery
}

void DeliveryManager::ClearDeliveries()
{
}
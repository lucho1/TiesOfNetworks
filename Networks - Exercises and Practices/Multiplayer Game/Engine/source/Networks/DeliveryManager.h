#ifndef _DELIVERYMANAGER_H_
#define _DELIVERYMANAGER_H_

// TODO(you): Reliability on top of UDP lab session
class DeliveryManager;
class DeliveryDelegate
{
public:

	DeliveryDelegate() {}
	~DeliveryDelegate() = default;

	virtual void onDeliverySuccess(DeliveryManager* deliveryManager) {};
	virtual void onDeliveryFailure(DeliveryManager* deliveryManager) {};
};


struct Delivery
{
	uint32 sequenceNumber = 0;
	double dispatchTime = 0.0;
	DeliveryDelegate* deliveryDelegate = nullptr;
};


class DeliveryManager
{
public:

	DeliveryManager() {}
	~DeliveryManager() = default;

	// To write a seq. num. into a packet (senders)
	Delivery* WriteSequenceNumber(OutputMemoryStream& packet);

	// To process a seq. num. from a packet (receivers)
	bool ProcessSequenceNumber(const InputMemoryStream& packet);

	// To write acknowledged seq. nums. into a packet (receivers)
	void WritePendingSequenceNumbers(OutputMemoryStream& packet);

	// To process acknowledged seq. nums. from a packet (senders)
	void ProcessAckdSequenceNumbers(const InputMemoryStream& packet);
	void ProcessTimedOutPackets();

private:

	// Sender
	uint32 m_NextSeqNumSent = 0;
	std::vector<Delivery> m_ServerPendingDeliveries;

	// Receiver
	uint32 m_NextSeqNumExpected = 1;
	std::vector<uint32> m_ClientPendingAcks;
};

#endif //_DELIVERYMANAGER_H_
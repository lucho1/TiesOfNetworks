#ifndef _DELIVERYMANAGER_H_
#define _DELIVERYMANAGER_H_

// TODO(you): Reliability on top of UDP lab session
class DeliveryManager;
class DeliveryDelegate
{
public:

	DeliveryDelegate() {}
	~DeliveryDelegate() = default;

	virtual void onDeliverySuccess(DeliveryManager* deliveryManager) = 0;
	virtual void onDeliveryFailure(DeliveryManager* deliveryManager) = 0;
};


struct DeliveryMessage
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
	DeliveryMessage* WriteSequenceNumber(OutputMemoryStream& packet);

	// To process a seq. num. from a packet (receivers)
	bool ProcessSequenceNumber(const InputMemoryStream& packet);

	// To write acknowledged seq. nums. into a packet (receivers)
	bool HasPendingSequenceNumbers() const;
	void WritePendingSequenceNumbers(OutputMemoryStream& packet);

	// To process acknowledged seq. nums. from a packet (senders)
	void ProcessAckdSequenceNumbers(const InputMemoryStream& packet);
	void ProcessTimedOutPackets();

	void ClearDeliveries();

private:

	// Sender
	uint32 m_NextSeqNumOut = -1;
	std::vector<DeliveryMessage> m_PendingDeliveries;

	// Receiver
	uint32 m_NextSeqNumExpected = -1;
	std::vector<uint32> m_PendingSequenceNums;
};

#endif //_DELIVERYMANAGER_H_
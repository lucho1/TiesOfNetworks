#ifndef _DELIVERYMANAGER_H_
#define _DELIVERYMANAGER_H_

// TODO(you): Reliability on top of UDP lab session
class DeliveryDelegate
{
public:

	DeliveryDelegate() {}
	~DeliveryDelegate() = default;

	virtual void onDeliverySuccess(uint32 sequenceNumber) {};
	virtual void onDeliveryFailure(uint32 sequenceNumber) {};
};

class ReplicationManagerServer;
class ServerDelegate : public DeliveryDelegate {
private:
	ReplicationManagerServer* replicationManager = nullptr;

public:
	ServerDelegate(ReplicationManagerServer* replicationManager);
	void onDeliverySuccess(uint32 sequenceNumber) override;
	void onDeliveryFailure(uint32 sequenceNumber) override;
};


struct Delivery
{
	uint32 sequenceNumber = 0;
	double dispatchTime = 0.0;
	DeliveryDelegate* deliveryDelegate = nullptr;
	bool createdDelegate = false;
};


class DeliveryManager
{
public:

	DeliveryManager() {}
	~DeliveryManager() = default;

	// To write a seq. num. into a packet (senders)
	Delivery* WriteSequenceNumber(OutputMemoryStream& packet, DeliveryDelegate* delegate = nullptr);

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
#ifndef _REPLICATION_MANAGERSERVER_H_
#define _REPLICATION_MANAGERSERVER_H_

// TODO(you): World state replication lab session

class ReplicationManagerServer
{
public:

	inline void Create(uint32 networkID) {
		SetAction(networkID, REPLICATION_ACTION::CREATE);
	}
	inline void Update(uint32 networkID) {
		SetAction(networkID, REPLICATION_ACTION::UPDATE);
	}
	inline void Destroy(uint32 networkID) {
		SetAction(networkID, REPLICATION_ACTION::DESTROY);
	}

	void Write(OutputMemoryStream& packet, uint32 sequence_number = 0);
	void ResendReplication(uint32 sequence_number);
	void DiscardReplication(uint32 sequence_number);

private:
	void SetAction(uint32 networkID, REPLICATION_ACTION action);

private:

	std::vector<ReplicationCommand> m_ReplicationCommands;
	std::unordered_map <uint32, std::vector<ReplicationCommand>> m_SentReplications;
};

#endif //_REPLICATION_MANAGERSERVER_H_
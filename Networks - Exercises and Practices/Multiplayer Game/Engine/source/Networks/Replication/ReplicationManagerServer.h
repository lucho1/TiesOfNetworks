#ifndef _REPLICATION_MANAGERSERVER_H_
#define _REPLICATION_MANAGERSERVER_H_

// TODO(you): World state replication lab session
enum class ReplicationAction { NONE = -1, CREATE, UPDATE, DESTROY };

struct ReplicationCommand
{
	ReplicationAction action;
	uint32 netID;
};

class ReplicationManagerServer
{
public:

	void Create(uint32 networkID);
	void Update(uint32 networkID);
	void Destroy(uint32 networkID);

	void Write(OutputMemoryStream& packet);

private:

	std::unordered_map<uint32, ReplicationCommand> m_ReplicationMap;
};

#endif //_REPLICATION_MANAGERSERVER_H_
#ifndef _REPLICATION_COMMAND_H_
#define _REPLICATION_COMMAND_H_

// TODO(you): World state replication lab session
enum class REPLICATION_ACTION{
	NONE, 
	CREATE, 
	UPDATE, 
	DESTROY 
};

struct ReplicationCommand {
	REPLICATION_ACTION action;
	uint32 net_id;
};

#endif //_REPLICATION_COMMAND_H_
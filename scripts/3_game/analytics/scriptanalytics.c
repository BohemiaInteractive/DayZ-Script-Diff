// class binded to engine
class StatsEventMeasuresData
{
	string m_CharacterId;		//!< character ID
	int m_TimeInterval;			//!< amount of real time in seconds covered by this event
	int m_DaytimeHour;			//!< current daytime in gameplay (hour in 24h format)
	vector m_PositionStart;		//!< player world position at the start of interval
	vector m_PositionEnd;		//!< player world position at the end of interval
	float m_DistanceOnFoot;		//!< traveled distance on foot (meters) during interval
	
	float m_DistanceVehicle;	//!< traveled distance (meters) in vehicle during interval
	float m_TimeVONIn;			//!< amount of time in seconds with inbound VON during interval
	float m_TimeVONOut;			//!< amount of time in seconds with outbound VON during interval
	int m_CntLootAny;			//!< count of any loot collected during interval
	int m_CntLootFood;			//!< count of any food or consumables collected during interval
	int m_CntLootCloth;			//!< count of cloth collected during interval
	int m_CntLootFirearm;		//!< count of firearms collected during interval
	int m_CntLootAmmo;			//!< count of ammo collected during interval
	int m_CntKillInfected;		//!< count of infected kills during interval
	int m_CntConsumedFood;		//!< count of consumed food during interval
	int m_CntConsumedWater;		//!< count of consumed water during interval
	int m_HealthRestored;		//!< number of health point restored during interval
	int m_CntFiredAmmo;			//!< firearm rounds fired during interval
	int m_CntCraftedItem;		//!< number of items crafted during interval
	
	// listStatus				// state metric of health, hunger, thirst, etc... when event is created/send
	int m_HealthStatus;			//!< state of health (current state)
	int m_BloodStatus;			//!< state of blood (current state)
	int m_SicknessStatus;		//!< state of sickness (current state)
	int m_TemperatureStatus;	//!< state of temperature (current state)
	int m_FoodStatus;			//!< state of food (hunger) (current state)
	int m_HydrationStatus;		//!< state of hydration (thirst) (current state)
};

// class binded to engine
class StatsEventDeathData
{
	string		m_CharacterId;			//!< character ID
	int			m_CharacterLifetime;	//!< lifetime of character in seconds
	string		m_Cause;				//!< cause of death (hunger, sickness, player-killed, zombie-killed...)
	string		m_WeaponName;			//!< name of weapon which caused death
	float		m_Distance;				//!< distance in meters (rounded) from spawn position to death position
	vector		m_Position;				//!< position of player when died
	
	int			m_ListDamages[5];		//!< list of damages (5 values) during last n sec. For example: {20, 80, 0, 0, 0}
};

// class binded to engine
class StatsEventScoredKillData
{
	string		m_CharacterId;		//!< character ID
	string		m_WeaponName;		//!< name of weapon which killed player (victim)
	int			m_KillDistance;		//!< distance in meters (rounded) between killer and victim
	vector		m_PositionKiller;	//!< position of killer
	vector		m_PositionVictim;	//!< position of victim
};

// class binded to engine
class StatsEventDisconnectedData
{
	string		m_CharacterId;	//!< character ID
	string		m_Reason;		//!< reason of disconnect (quit, kick, ban, sign-out...)
};

// class binded to engine
class StatsEventSpawnedData
{
	string		m_CharacterId;	//!< character ID
	int			m_Lifetime;		//!< lifetime of character in seconds
	vector		m_Position;		//!< position of spawn
	int			m_Population;	//!< population of current gameplay (server)
	int			m_DaytimeHour;	//!< current time in hour (hour in 24h)
};

// class binded to engine
class StatsEventData
{
	void StatsEventData(string eventName)
	{
		m_eventName = eventName;
		m_valuesBool = new map<string, int>();
		m_valuesInt = new map<string, int>();
		m_valuesFloat = new map<string, float>();
		m_valuesString = new map<string, string>();
		m_valuesVector = new map<string, vector>();
	}
	
	void AddBool(string key, bool value)
	{
		m_valuesBool.Insert(key, (int)value);
	}
	
	void AddInt(string key, int value)
	{
		m_valuesInt.Insert(key, value);
	}
	
	void AddFloat(string key, float value)
	{
		m_valuesFloat.Insert(key, value);
	}
	
	void AddString(string key, string value)
	{
		m_valuesString.Insert(key, value);
	}
	
	void AddVector(string key, vector value)
	{
		m_valuesVector.Insert(key, value);
	}
	
	private string m_eventName;
	private autoptr map<string, int> m_valuesBool;//TODO: use bool instead of int (problem with engine type binding)
	private autoptr map<string, int> m_valuesInt;
	private autoptr map<string, float> m_valuesFloat;
	private autoptr map<string, string> m_valuesString;
	private autoptr map<string, vector> m_valuesVector;
}


class ScriptAnalytics
{
	//! send event about death to statistic DB
	static proto native void SendPlayerDeath(StatsEventDeathData data);
	
	//! send event about kill to statistic DB
	static proto native void SendPlayerScoredKill(StatsEventScoredKillData data);
	
	//! send event to statistic DB
	static proto native void SendPlayerDisconnected(StatsEventDisconnectedData data);
	
	//! send event about player status to statistic DB
	static proto native void SendPlayerMeasures(StatsEventMeasuresData data);
	
	//! send event about spawning to statistic DB
	static proto native void SendPlayerSpawned(StatsEventSpawnedData data);
	
	//! universal analytics event
	static proto native void SendEvent(StatsEventData data);
};



class Analytics
{
	// send stats data with log
	static void PlayerDeath(StatsEventDeathData data)
	{
		ScriptAnalytics.SendPlayerDeath(data);
	}
	
	// send stats data with log
	static void PlayerScoredKill(StatsEventScoredKillData data)
	{
		ScriptAnalytics.SendPlayerScoredKill(data);
	}
	
	// send stats data with log
	static void PlayerDisconnected(StatsEventDisconnectedData data)
	{
		ScriptAnalytics.SendPlayerDisconnected(data);
	}
	
	// send stats data with log
	static void PlayerMeasures(StatsEventMeasuresData data)
	{
		ScriptAnalytics.SendPlayerMeasures(data);
	}
	
	// send stats data with log
	static void PlayerSpawned(StatsEventSpawnedData data)
	{
		ScriptAnalytics.SendPlayerSpawned(data);
	}
}
class CfgBunkerBroadcastHandler
{
	private static bool m_Initialized;
	
	private static const string m_ConfigFilePath = "cfgbunkerbroadcast.json";
	private static ref BunkerBroadcastConfigInstance m_Data = new BunkerBroadcastConfigInstance();
	
	static bool LoadData()
	{
		if (m_Initialized)
			return true;
		
		m_Initialized = false;
		
		if (!CfgGameplayHandler.GetBunkerBroadcastEnabled())
			return false;

		string worldName;
		GetGame().GetWorldName(worldName);
	
		string errorMessage, path;

		path = string.Format("$mission:%1", m_ConfigFilePath);
		if (!JsonFileLoader<BunkerBroadcastConfigInstance>.LoadFile(path, m_Data, errorMessage))
		{
			if (!FileExist(string.Format("$mission:%1", m_ConfigFilePath)))
				path = string.Format("dz/worlds/%1/ce/%2", worldName, m_ConfigFilePath);
			
			if (!JsonFileLoader<BunkerBroadcastConfigInstance>.LoadFile(path, m_Data, errorMessage))
			{
				ErrorEx(errorMessage);
				return false;
			}
		}
		
		m_Initialized = m_Data.IsValid();
		
		return m_Initialized;
	}
	
	static bool IsInitialized()
	{
		return m_Initialized;
	}
	
	static BunkerBroadcastConfigInstance GetData()
	{
		return m_Data;
	}
	
	static void Spawn()
	{
		if (!CfgGameplayHandler.GetBunkerBroadcastEnabled())
			return;
		
		foreach (BunkerBroadcastProxySpawn proxySpawn : m_Data.proxies)
			SpawnBunker(proxySpawn);
	}
	
	static void SyncDataSend(PlayerIdentity identity)
	{
		GetGame().RPCSingleParam(null, ERPCs.RPC_BUNKERBROADCAST_CONFIG_SYNC, new Param1<BunkerBroadcastConfigInstance>(m_Data), true, identity);
	}
	
	static void OnRPC(ParamsReadContext ctx)
	{
		Param1<BunkerBroadcastConfigInstance> data = new Param1<BunkerBroadcastConfigInstance>(null);
		
		if (ctx.Read(data))
		{
			m_Data = data.param1;
			m_Initialized = m_Data.IsValid();
		}
		else
		{
			ErrorEx("CfgBunkerBroadcast - client failed to read incoming data");
		}
	}
		
	private static void SpawnBunker(BunkerBroadcastProxySpawn proxySpawn)
	{	
		int flags = ECE_SETUP | ECE_NOLIFETIME;
		
		Object proxy = g_Game.CreateObjectEx("BunkerBroadcastProxy", vector.ArrayToVec(proxySpawn.position), flags);
		if (!proxy)
			PrintToRPT("SpawnBunker failed to spawn Proxy at " + vector.ArrayToVec(proxySpawn.position));
	}
}
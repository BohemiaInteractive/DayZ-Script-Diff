class BunkerBroadcastProxy : Building
{
	protected ref BunkerBroadcastComponent m_BunkerComponent; 

	protected Bunker m_Bunker;
	
	protected int m_NetIdLow 	= -1;
	protected int m_NetIdHigh 	= -1;
	
	void BunkerBroadcastProxy()
	{
		m_BunkerComponent = new BunkerBroadcastComponent(this);
		
		RegisterNetSyncVariableInt("m_NetIdLow", int.MIN, int.MAX);
		RegisterNetSyncVariableInt("m_NetIdHigh", int.MIN, int.MAX);

		RegisterNetSyncVariableBool("m_BunkerComponent.m_IsActive");
	}
	
	// --------------------------------------------------------------------------------
	
	override void DeferredInit()
	{
		super.DeferredInit();

		if (g_Game.IsServer())
		{
			AssignBunkerObject();
			InitializeComponents();
		}
	}
	
	// --------------------------------------------------------------------------------
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		m_BunkerComponent.OnRPC(sender, rpc_type, ctx);
	}

	// --------------------------------------------------------------------------------
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (g_Game.IsMultiplayer())
		{
			m_Bunker = Bunker.Cast(g_Game.GetObjectByNetworkId(m_NetIdLow, m_NetIdHigh));
			if (m_Bunker)
				m_Bunker.SetActive(IsActive());
		}
	}
	
	
	// --------------------------------------------------------------------------------
	
	bool IsActive()
	{
		return m_BunkerComponent.IsActive();
	}
	
	// --------------------------------------------------------------------------------

	Bunker GetBunker()
	{
		return m_Bunker;
	}
	
	// --------------------------------------------------------------------------------

	protected void AssignBunkerObject()
	{
		array<Object> objects = new array<Object>();
		array<CargoBase> proxies = new array<CargoBase>();
		g_Game.GetObjectsAtPosition3D(GetWorldPosition(), 1.0, objects, proxies);
		foreach (Object bunkerObject : objects)
		{
			Bunker bunker = Bunker.Cast(bunkerObject);
			if (bunker)
			{
				int low, high;
				bunker.GetNetworkID(low, high);
				
				m_NetIdLow 	= low;
				m_NetIdHigh = high;
				m_Bunker 	= bunker;

				SetSynchDirty();

				if (m_BunkerComponent)
				{
					if (m_BunkerComponent.IsActive())
						OnBunkerActivated(true, m_BunkerComponent.GetLockCode());
				}

				return;
			}
		}
	}
	
	// --------------------------------------------------------------------------------
	
	protected void InitializeComponents()
	{
		if (m_BunkerComponent)
		{
			m_BunkerComponent.Init();
			if (m_BunkerComponent.GetOnActivated())
				m_BunkerComponent.GetOnActivated().Insert(OnBunkerActivated);
			if (m_BunkerComponent.GetOnDeactivated())
				m_BunkerComponent.GetOnDeactivated().Insert(OnBunkerDeactivated);
		}
	}
	
	// --------------------------------------------------------------------------------
	
	protected void OnBunkerActivated(bool loadedAsActive, string codeLockCode)
	{
		if (!m_Bunker)
		{
			ErrorEx("No Bunker found, cannot activate");
			return;
		}

		m_Bunker.OnBunkerActivated(loadedAsActive, m_BunkerComponent.GetLockCode());
	}

	// --------------------------------------------------------------------------------
	
	protected void OnBunkerDeactivated()
	{
		if (!m_Bunker)
		{
			ErrorEx("No Bunker found, cannot activate");
			return;
		}

		m_Bunker.OnBunkerDeactivated();
	}
}
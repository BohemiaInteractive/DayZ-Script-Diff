class PlayerStatBase
{	
	protected Man m_Player;

	int	m_Type;
	void 	OnStoreSave( ParamsWriteContext ctx );
	bool 	OnStoreLoad( ParamsReadContext ctx);
	void 	OnRPC(ParamsReadContext ctx);
	void 	OnAfterStoreLoad();
	float 	Get();
	string 	GetLabel();
	void 	SetByFloat(float value);
	void 	SetByFloatEx(float value, string system = "" );
	bool	IsSynced();
	array<PlayerStatRecord> GetRecords();
	void 	Init(int id);
	void 	SerializeValue(array<ref StatDebugObject> objects, int flags);
	float 	GetNormalized();
	float 	GetMax();
	float 	GetMin();
	int		GetType()
	{
		return m_Type;
	}

	void SetPlayer(Man player)
	{
		m_Player = player;
	}
}

class PlayerStat<Class T> extends PlayerStatBase
{
	protected T 		m_MinValue;
	protected T 		m_MaxValue;
	protected T 		m_Value;
	protected T 		m_ValueLastSynced;
	protected string 	m_ValueLabel;
	protected int		m_Flags;
	
	ref array<PlayerStatRecord> m_Records;

	void PlayerStat(T min, T max, T init, string label, int flags)
	{
		m_MinValue 		= min;
		m_MaxValue 		= max;
		m_Value			= init;
		m_ValueLabel 	= label;
		m_Flags			= flags;
		
		m_Records = new array<PlayerStatRecord>;
	}
		
	override void Init(int id)
	{
		m_Type = id;
	}
	
	override void SerializeValue(array<ref StatDebugObject> objects, int flags)
	{
		objects.Insert(new StatDebugObject(GetLabel(), Get(), eRemoteStatType.PLAYER_STATS));
	}
	
	override bool IsSynced()
	{
		return m_Flags & EPSstatsFlags.SYNCED == EPSstatsFlags.SYNCED;
	}
	
	override void OnRPC(ParamsReadContext ctx)
	{
		super.OnRPC(ctx);
		
		if (IsSynced())
		{
			ctx.Read(CachedObjectsParams.PARAM2_INT_FLOAT);
			
			int type = CachedObjectsParams.PARAM2_INT_FLOAT.param1;
			T value	= CachedObjectsParams.PARAM2_INT_FLOAT.param2;
				
			if (m_Type == type)
				Set(value);
		}
	}
	
	void Set(T value, string system = "")
	{
		if (value > m_MaxValue)
		{
			m_Value = m_MaxValue;
		}
		else if (value < m_MinValue)
		{
			m_Value = m_MinValue;
		}
		else
		{
			m_Value = value;
		}
		
		#ifdef SERVER
		if (IsSynced())
		{
			if (m_ValueLastSynced != m_Value)
			{
				//! limits the sync for bigger changes of float stats
				if (T == float && Math.AbsFloat(m_ValueLastSynced - m_Value) < 0.05)
					return;
				
				CachedObjectsParams.PARAM2_INT_FLOAT.param1 = m_Type;
				CachedObjectsParams.PARAM2_INT_FLOAT.param2 = m_Value;
				m_Player.RPCSingleParam(ERPCs.RPC_PLAYER_STAT, CachedObjectsParams.PARAM2_INT_FLOAT, true, m_Player.GetIdentity());
				m_ValueLastSynced = m_Value;
			}
		}
		#endif
	}
	
	void SetByFloat(float value, string system = "")
	{
		T f = value;
		Set(f, system);
	}

	override void SetByFloatEx(float value, string system = "")
	{
		SetByFloat(value, system);
	}

	void Add( T value, string system = "" )
	{
		Set(m_Value+value, system);
	}

	override float Get()
	{
		return m_Value;
	}
	
	override string GetLabel()
	{
		return m_ValueLabel;
	}

	override float GetMax()
	{
		return m_MaxValue;
	}

	override float GetMin()
	{
		return m_MinValue;
	}
	
	override float GetNormalized()
	{
		return Math.InverseLerp(GetMin(), GetMax(), Get());
	}
	
	override array<PlayerStatRecord> GetRecords()
	{
		return m_Records;
	}
	
	void CreateRecord(float value, string system)
	{
		m_Records.Insert( new PlayerStatRecord(value, GetGame().GetTime(), system ) );
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{   
		ctx.Write(m_Value);
	}

	override bool OnStoreLoad( ParamsReadContext ctx)
	{
		T value;
		if (ctx.Read(value))
		{
			m_Value = value;
		}
		else
		{
			return false;
		}

		return true;
	}
	
	override void OnAfterStoreLoad()
	{
		// forces the sync
		Set(Get());
	}
	
	//! DEPRECATED
	PlayerStats 		m_Manager;
	
	PlayerStats GetManager()
	{
		return m_Manager;
	}
}

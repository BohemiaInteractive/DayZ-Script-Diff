class PlayerStatBase
{
	int	m_Type;
	void 	OnStoreSave( ParamsWriteContext ctx );
	bool 	OnStoreLoad( ParamsReadContext ctx);
	void 	OnRPC(ParamsReadContext ctx);
	float 	Get();
	string 	GetLabel();
	void 	SetByFloat(float value);
	void 	SetByFloatEx(float value, string system = "" );
	bool	IsSynced();
	array<PlayerStatRecord> GetRecords();
	void 	Init(int id/*, PlayerStats manager*/);
	void 	SerializeValue(array<ref StatDebugObject> objects, int flags);
	float 	GetNormalized();
	float 	GetMax();
	float 	GetMin();
	int		GetType()
	{
		return m_Type;
	}
};

class PlayerStat<Class T> extends PlayerStatBase
{
	protected T 		m_MinValue;
	protected T 		m_MaxValue;
	protected T 		m_Value;
	protected string 	m_ValueLabel;
	protected int		m_Flags;
	
	ref array<PlayerStatRecord> m_Records;
	PlayerStats 		m_Manager;
	
	
	void PlayerStat(T min, T max,T init, string label, int flags)
	{
		m_MinValue 		= min;
		m_MaxValue 		= max;
		m_Value			= init;
		m_ValueLabel 	= label;
		m_Flags			= flags;
		
		m_Records = new array<PlayerStatRecord>;
	}
		
	override void Init(int id/*, PlayerStats manager*/)
	{
		m_Type = id;
		//m_Manager = manager;	
	}
	
	override void SerializeValue(array<ref StatDebugObject> objects, int flags)
	{
		objects.Insert( new StatDebugObject(GetLabel(), Get(), eRemoteStatType.PLAYER_STATS) );
	}
	
	PlayerStats GetManager()
	{
		return m_Manager;
	}	
	
	void Set( T value, string system = "" )
	{
		/*
		Print("setting stat: " + this.GetLabel() + "| value:" +value.ToString());
		
		if( this.GetLabel() == "Toxicity" )
		{
			DebugPrint.LogAndTrace("stack");
		}
		*/
		if ( value > m_MaxValue )
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
		//if( GetManager().GetAllowLogs() ) CreateRecord(value, system);
	}
	
	void SetByFloat(float value, string system = "" )
	{
		T f = value;
		Set(f,system);
		
	}
	override void SetByFloatEx(float value, string system = "" )
	{
		SetByFloat(value, system);
	}
	/*
	void SetByParam(Param param, string system = "" )
	{
		Class.CastTo(p1, param);
		T v = p1.param1;
		Set(v, system);
	}
	*/
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
	
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		//ctx.Write(m_ValueLabel);
		//PrintString("saving " + GetLabel()+" value:" +m_Value);
		ctx.Write(m_Value);
	}

	override bool OnStoreLoad( ParamsReadContext ctx)
	{
		//string name;
		
		//ctx.Read(name);
		T value;
		if(ctx.Read(value))
		{
			m_Value = value;
		}
		else
		{
			return false;
		}
		//PrintString("loading " + GetLabel()+" value:" +m_Value);
		return true;
	}
}
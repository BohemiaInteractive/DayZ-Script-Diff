class SyncedValue
{
	string m_Name;
	float m_Value;
	float m_ValueNorm;
	bool m_State;
	
	void SyncedValue( string name, float value, bool state, float val_norm )
	{
		m_Name = name;
		m_Value = value;
		m_State = state;
		m_ValueNorm = val_norm;
	}
	
	string GetName()
	{
		return m_Name;
	}
	
	float GetValue()
	{
		return m_Value;
	}	
	
	float GetValueNorm()
	{
		return m_ValueNorm;
	}
	
	bool GetState()
	{
		return m_State;
	}
}

class SyncedValueLevel
{
	string m_Name;
	float m_Value;
	float m_Value2;
	
	void SyncedValueLevel( string name, float value, float value2 )
	{
		m_Name = name;
		m_Value = value;
		m_Value2 = value2;
	}
	
	string GetName()
	{
		return m_Name;
	}
	
	float GetValue()
	{
		return m_Value;
	}
	
	float GetValue2()
	{
		return m_Value2;
	}
}


class SyncedValueModifier
{
	string m_Name;
	int m_ID;
	bool m_Locked;
	bool m_Active;
	
	void SyncedValueModifier( int id, string name, bool active, bool locked )
	{
		m_Name = name;
		m_ID = id;
		m_Locked = locked;
		m_Active = active;
	}
	
	string GetName()
	{
		return m_Name;
	}
	
	int GetID()
	{
		return m_ID;
	}
	
	bool GetLocked()
	{
		return m_Locked;
	}
	
	bool GetActive()
	{
		return m_Active;
	}
}


class SyncedValueAgent
{
	string m_Name;
	string m_Value;
	int m_ID;
	
	void SyncedValueAgent( string name, string value, int id )
	{
		m_Name = name;
		m_Value = value;
		m_ID = id;
	}
	
	string GetName()
	{
		return m_Name;
	}
	
	string GetValue()
	{
		return m_Value;
	}
	
	int GetID()
	{
		return m_ID;
	}
}

class ConstructionPart
{
	string 	m_Name;				//localized text name that is displayed ingame
	int		m_Id;				//a number used for synchronization and persistence purposes, must be unique and withing sync limit (total number of bits in all sync/persistence variables)
	string 	m_PartName;			//config class name
	string 	m_MainPartName;		//main (parent) config class name
	bool 	m_IsBuilt;			//defines part build state
	bool 	m_IsBase;			//defines if this part is the foundation of the whole construction
	bool 	m_IsGate;			//defines part gate state
	ref array<string> m_RequiredParts; //list of parts required by this part
	
	void ConstructionPart( string name, string part_name, string main_part_name, int id, bool is_built, bool is_base, bool is_gate, array<string> required_parts )
	{
		m_Name = name;
		m_PartName = part_name;
		m_MainPartName = main_part_name;
		m_Id = id;
		m_IsBuilt = is_built;
		m_IsBase = is_base;
		m_IsGate = is_gate;
		m_RequiredParts = required_parts;
	}
	
	string GetName()
	{
		string ret = Widget.TranslateString(m_Name);
		return ret;
	}
	
	string GetPartName()
	{
		return m_PartName;
	}

	string GetMainPartName()
	{
		return m_MainPartName;
	}
	
	int GetId()
	{
		return m_Id;
	}
	
	bool IsBuilt()
	{
		return m_IsBuilt;
	}
	
	void SetBuiltState( bool is_built )
	{
		bsbDebugPrint("[bsb] SetBuildState=" + is_built + " part=" + m_PartName);
		m_IsBuilt = is_built;
	}
	
	void SetRequestBuiltState( bool req_built )
	{
		bsbDebugPrint("[bsb] SetRequestBuiltState=" + req_built + " part=" + m_PartName);
		if (GetGame().IsMultiplayer())
			SetBuiltState(req_built);
		else
			; // skip set to true in single player - will be synced later
	}
	
	bool IsBase()
	{
		return m_IsBase;
	}
	
	bool IsGate()
	{
		return m_IsGate;
	}
	
	array<string> GetRequiredParts()
	{
		return m_RequiredParts;
	}
}

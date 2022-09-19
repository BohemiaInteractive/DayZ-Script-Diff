class HiddenSelectionsData
{
	ref array<string> m_HiddenSelections = new array<string>;
	ref array<string> m_HiddenSelectionsTextures = new array<string>;
	ref array<string> m_HiddenSelectionsMaterials = new array<string>;
	
	ref map<string, int> m_HiddenSelectionNameToIndex = new map<string, int>;
	
	void HiddenSelectionsData(string type)
	{
		GetGame().ConfigGetTextArray( string.Format("CfgVehicles %1 hiddenSelections", 			type), m_HiddenSelections );
		GetGame().ConfigGetTextArray( string.Format("CfgVehicles %1 hiddenSelectionsTextures", 	type), m_HiddenSelectionsTextures );
		GetGame().ConfigGetTextArray( string.Format("CfgVehicles %1 hiddenSelectionsMaterials", type), m_HiddenSelectionsMaterials );
		
		for (int i = 0; i < m_HiddenSelections.Count(); ++i)
			m_HiddenSelectionNameToIndex.Insert(m_HiddenSelections[i], i);
	}
	
	int GetHiddenSelectionIndex(string selection)
	{
		int index;
		if (m_HiddenSelectionNameToIndex.Find(selection, index))
			return index;
		
		return -1;
	}
	
	TStringArray GetHiddenSelectionsTextures()
	{
		return m_HiddenSelectionsTextures;
	}
	
	TStringArray GetHiddenSelectionsMaterials()
	{
		return m_HiddenSelectionsMaterials;
	}
	
	static array<string> GetHiddenSelectionsConfig(string type)
	{
		array<string> hiddenSelections = new array<string>;
		GetGame().ConfigGetTextArray( string.Format("CfgVehicles %1 hiddenSelections", type), hiddenSelections );
		return hiddenSelections;
	}
	
	static array<string> GetHiddenSelectionsTexturesConfig(string type)
	{
		array<string> hiddenSelectionsTextures = new array<string>;
		GetGame().ConfigGetTextArray( string.Format("CfgVehicles %1 hiddenSelectionsTextures", type), hiddenSelectionsTextures );
		return hiddenSelectionsTextures;
	}
	
	static array<string> GetHiddenSelectionsMaterialsConfig(string type)
	{
		array<string> hiddenSelectionsMaterials = new array<string>;
		GetGame().ConfigGetTextArray( string.Format("CfgVehicles %1 hiddenSelectionsMaterials", type), hiddenSelectionsMaterials );
		return hiddenSelectionsMaterials;
	}
}

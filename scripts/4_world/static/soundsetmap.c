class SoundSetMap
{
	ref static map<int, string> m_SoundSetMapIDByID = new map<int, string>;
	ref static map<string, int> m_SoundSetMapIDByName = new map<string, int>;
	
	static void Init()
	{
		string path = "CfgSoundSets";
		
		int soundCount = GetGame().ConfigGetChildrenCount(path);
		
		for (int i = 1; i < soundCount; i++)
		{
			string soundClassName;
			GetGame().ConfigGetChildName(path, i, soundClassName);
			m_SoundSetMapIDByID.Insert(i,soundClassName);
			m_SoundSetMapIDByName.Insert(soundClassName,i);
			//PrintString(soundClassName);
		}
	}
	
	static string GetSoundSetName(int id)
	{
		return m_SoundSetMapIDByID.Get(id);
	}
	
	static int GetSoundSetID(string name)
	{
		return m_SoundSetMapIDByName.Get(name);
	}

}
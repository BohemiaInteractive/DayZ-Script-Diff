class ModLoader
{
	protected static bool							m_Loaded;
	protected static ref array<ref ModStructure>	m_Mods;
	
	static array<ref ModStructure> GetMods()
	{
		//if( !m_Loaded )
			LoadMods();
		return m_Mods;
	}
	
	static void LoadMods()
	{
		m_Mods = new array<ref ModStructure>;
		
		int mod_count = GetGame().ConfigGetChildrenCount( "CfgMods" );
		
		for( int i = 2; i < mod_count; i++ )
		{
			string mod_name;
			GetGame().ConfigGetChildName( "CfgMods", i, mod_name );
			m_Mods.Insert( new ModStructure( i, "CfgMods " + mod_name ) );
		}
	}
}
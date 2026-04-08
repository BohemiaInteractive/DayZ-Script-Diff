class AmmoCamParams
{
	void Load(string ammoType)
	{
		string cfgPath = "CfgAmmo " + ammoType + " Effects" + " CameraShake ";
		
		m_Radius				= g_Game.ConfigGetFloat(cfgPath + "radius");
		m_Strength				= g_Game.ConfigGetFloat(cfgPath + "strength");
		m_ModifierClose			= g_Game.ConfigGetFloat(cfgPath + "modifierClose");
		m_ModifierFar			= g_Game.ConfigGetFloat(cfgPath + "modifierFar");
	}
	
	float m_Radius = 40;//the max distance at which the cam shake is triggered
	float m_Strength = 4;//strength of the cam shake
	float m_ModifierClose = 1;//shake 'strength' modifier when the player is at distance 0 from the source of explosion
	float m_ModifierFar = 0;//shake 'strength' modifier when the player is at distance 'radius' from the source of explosion
}
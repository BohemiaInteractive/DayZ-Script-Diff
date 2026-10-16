class WorldLighting
{
	string lighting_default = "dz\\data\\lighting\\lighting_default.txt";
	string lighting_darknight = "dz\\data\\lighting\\lighting_darknight.txt";
	string lighting_sakhal = "DZ\\worlds\\sakhal\\data\\lighting\\lighting_sakhal.txt";
	string lighting_nasdara = "DZ\\worlds\\nasdara\\data\\lighting\\lighting_nasdara.txt";
	
	void WorldLighting() {}
	void ~WorldLighting() {}
		
	// sets global lighting config by given value (sent from server, defined in server config)
	void SetGlobalLighting( int lightingID )
	{
		switch ( lightingID )
		{
			case 0:
				g_Game.GetWorld().LoadNewLightingCfg( lighting_default );
				break;
				
			case 1:
				g_Game.GetWorld().LoadNewLightingCfg( lighting_darknight );
				break;
				
			case 2:
				g_Game.GetWorld().LoadNewLightingCfg( lighting_sakhal );
				break;
				
			case 3:
				GetGame().GetWorld().LoadNewLightingCfg( lighting_nasdara );
				break;
		}
	}
}

/*modded class WorldLighting
{
	protected string lighting_modded = "your\\path\\to\\lighting\\cfg.txt";
	
	override void SetGlobalLighting( int lightingID )
	{
		switch ( lightingID )
		{
			case 3:
				g_Game.GetWorld().LoadNewLightingCfg( lighting_modded );
				break;
		}
	}
}*/
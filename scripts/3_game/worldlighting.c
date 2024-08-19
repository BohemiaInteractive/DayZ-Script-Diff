class WorldLighting
{
	string lighting_default = "dz\\data\\lighting\\lighting_default.txt";
	string lighting_darknight = "dz\\data\\lighting\\lighting_darknight.txt";
	
	void WorldLighting() {}
	void ~WorldLighting() {}
		
	// sets global lighting config by given value (sent from server, defined in server config)
	void SetGlobalLighting( int lightingID )
	{
		switch ( lightingID )
		{
			case 0:
				GetGame().GetWorld().LoadNewLightingCfg( lighting_default );
				break;
				
			case 1:
				GetGame().GetWorld().LoadNewLightingCfg( lighting_darknight );
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
				GetGame().GetWorld().LoadNewLightingCfg( lighting_modded );
				break;
		}
	}
}*/
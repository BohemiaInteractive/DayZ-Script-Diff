// This will be used to parse and load contaminated area related data
class EffectAreaLoader
{
	private static string m_Path = "$mission:cfgEffectArea.json";
	
	static void CreateZones()
	{
		JsonDataContaminatedAreas effectAreaData;
		
		// We confirm the contaminated area configuration file exists in mission folder
		if ( !FileExist( m_Path ) )
		{
			// We fallback to check in data and notify user file was not found in mission
			PrintToRPT("[WARNING] :: [EffectAreaLoader CreateZones] :: No contaminated area file found in MISSION folder, your path is " + m_Path + " Attempting DATA folder"); // If the path is invalid, we warn the user
			
			m_Path = "";
			GetGame().GetWorldName( m_Path );
			m_Path = string.Format("DZ/worlds/%1/ce/cfgEffectArea.json", m_Path );
			
			if ( !FileExist( m_Path ) )
			{
				PrintToRPT("[WARNING] :: [EffectAreaLoader CreateZones] :: No contaminated area file found in DATA folder, your path is " + m_Path); // If the path is invalid, we warn the user
				return; // Nothing could be read, just end here
			}
		}
		
		// We load the data from file, in case of failure we notify user
		effectAreaData = EffectAreaLoader.GetData();
		if ( effectAreaData )
		{
			// Now that we have extracted the data we go through every declared area
			//Debug.Log("Contaminated area JSON contains : " + effectAreaData.Areas.Count());
			
			for ( int i = 0; i < effectAreaData.Areas.Count(); i++ )
			{
				EffectAreaParams params = new EffectAreaParams();
				
				// We feed in all relevant data
				params.m_ParamName = effectAreaData.Areas.Get( i ).AreaName;
				string areaType = effectAreaData.Areas.Get( i ).Type;
				params.m_ParamTriggerType = effectAreaData.Areas.Get( i ).TriggerType;
				JsonDataAreaData data = effectAreaData.Areas.Get( i ).Data;
				
				// World level area data ( Trigger info, world particles, etc... )
				vector pos = Vector( data.Pos[0], data.Pos[1], data.Pos[2] );
				params.m_ParamRadius = data.Radius;
				params.m_ParamPosHeight = data.PosHeight;
				params.m_ParamNegHeight = data.NegHeight;
				params.m_ParamInnerRings = data.InnerRingCount;
				params.m_ParamInnerSpace = data.InnerPartDist;
				params.m_ParamOuterToggle = data.OuterRingToggle;
				params.m_ParamOuterSpace = data.OuterPartDist;
				params.m_ParamOuterOffset = data.OuterOffset;
				params.m_ParamVertLayers = data.VerticalLayers;
				params.m_ParamVerticalOffset = data.VerticalOffset;
				string particleName = data.ParticleName;
				
				// Local level area data ( Player particles and PPE )
				JsonDataPlayerData playerData = effectAreaData.Areas.Get( i ).PlayerData;
				string aroundPartName = playerData.AroundPartName;
				string tinyPartName = playerData.TinyPartName;
				string ppeRequesterType = playerData.PPERequesterType;
				
				// Conversion of particle name to ID for synchronization and loading
				if (particleName != "")
					params.m_ParamPartId = ParticleList.GetParticleID( particleName );
				
				if (aroundPartName != "")
					params.m_ParamAroundPartId = ParticleList.GetParticleID( aroundPartName );
				
				if (tinyPartName != "")
					params.m_ParamTinyPartId = ParticleList.GetParticleID( tinyPartName );
				
				params.m_ParamPpeRequesterType = ppeRequesterType;

				EffectArea newZone; // Zones MUST inherit from EffectArea
				
				// We snap item position to ground before creating if specified Y is 0
				if ( pos[1] == 0 )
				{
					pos[1] = GetGame().SurfaceRoadY( pos[0], pos[2] );
					Class.CastTo( newZone, GetGame().CreateObjectEx( areaType, pos, ECE_PLACE_ON_SURFACE ) );
				}
				else
					Class.CastTo( newZone, GetGame().CreateObjectEx( areaType, pos, ECE_NONE ) );
				
				// We created a new zone, we feed in the data to finalize setup
				if ( newZone )
					newZone.SetupZoneData( params );
				else
					Error("[WARNING] :: [EffectAreaLoader CreateZones] :: Cast failed, are you sure your class ( 'Type:' ) inherits from EffectArea and that there are no Typos?");
			}
		}
		else
			Error("[WARNING] :: [EffectAreaLoader CreateZones] :: Data could not be read, please check data and syntax"); // Most JSON related errors should be handled, but we have an extra check in case data could not be read
	}
	
	static JsonDataContaminatedAreas GetData()
	{
		JsonDataContaminatedAreas data;

		JsonFileLoader<JsonDataContaminatedAreas>.JsonLoadFile( m_Path, data );
		
		return data;
	}
}
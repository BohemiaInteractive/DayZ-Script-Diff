class JsonUndergroundTriggers
{
	ref array<ref JsonUndergroundAreaTriggerData> Triggers;
}


class JsonUndergroundAreaBreadcrumb
{
	vector GetPosition()
	{
		return Vector(Position[0],Position[1],Position[2]);
	}
	
	ref array<float> 	Position;
	float 				EyeAccommodation;
	bool 				UseRaycast;
	float 				Radius;
}

class JsonUndergroundAreaTriggerData
{
	vector GetPosition()
	{
		return Vector(Position[0],Position[1],Position[2]);
	}
	
	vector GetOrientation()
	{
		return Vector(Orientation[0],Orientation[1],Orientation[2]);
	}
	vector GetSize()
	{
		return Vector(Size[0],Size[1],Size[2]);
	}
	
	ref array<float> Position;
	ref array<float> Orientation;
	ref array<float> Size;
	float  EyeAccommodation;
	float  InterpolationSpeed;
	
	ref array<ref JsonUndergroundAreaBreadcrumb> Breadcrumbs;
	
};


class UndergroundAreaLoader
{
	private static string m_Path = "$mission:cfgundergroundtriggers.json";
	
	static ref JsonUndergroundTriggers m_JsonData;
	
	
	static JsonUndergroundTriggers GetData()
	{
		if ( !FileExist( m_Path ) )
		{
			// We fallback to check in data and notify user file was not found in mission
			PrintToRPT("[WARNING] :: [UndergroundAreaLoader GetData()] :: file not found in MISSION folder, your path is " + m_Path + " Attempting DATA folder");
			
			string worldName;
			GetGame().GetWorldName( worldName );
			m_Path = string.Format("DZ/worlds/%1/ce/cfgundergroundtriggers.json", worldName );
			
			if ( !FileExist( m_Path ) )
			{
				PrintToRPT("[WARNING] :: [UndergroundAreaLoader GetData()] ::file not found in DATA folder, your path is " + m_Path);
				return null; // Nothing could be read, just end here
			}
		}
		
		JsonUndergroundTriggers data;
		JsonFileLoader<JsonUndergroundTriggers>.JsonLoadFile( m_Path, data );
		
		return data;
	}
	
	
	static void SpawnAllTriggerCarriers()
	{
		if (!m_JsonData)
		{
			m_JsonData = GetData();
		}
		
		if (!m_JsonData || !m_JsonData.Triggers)
		{
			return;
		}
		
		foreach (int i, auto data:m_JsonData.Triggers)
		{
			SpawnTriggerCarrier(i, data);
		}
	}
	
	static void SpawnTriggerCarrier(int index, JsonUndergroundAreaTriggerData data)
	{
		
		UndergroundTriggerCarrierBase carrier = UndergroundTriggerCarrierBase.Cast(GetGame().CreateObjectEx( "UndergroundTriggerCarrier", data.GetPosition(), ECE_NONE ));
		//Print("spawning trigger carrier at pos :" +data.GetPosition());
		if (carrier)
		{
			carrier.SetIndex(index);
			carrier.SetOrientation(data.GetOrientation());
		}
	}
	
	//---------------------------------------------------------------------------------------
	static void SyncDataSend(PlayerIdentity identity)
	{
		GetGame().RPCSingleParam(null, ERPCs.RPC_UNDERGROUND_SYNC, new Param1<JsonUndergroundTriggers>(m_JsonData), true, identity);
	}
	
	//---------------------------------------------------------------------------------------
	
	static void OnRPC(ParamsReadContext ctx)
	{
		Param1<JsonUndergroundTriggers> data = new Param1< JsonUndergroundTriggers>(null);
		
		if ( ctx.Read(data) )
		{
			m_JsonData = data.param1;
		}
		else
		{
			ErrorEx("UndergroundAreaLoader datasynced - failed to read");
		}
	}

}
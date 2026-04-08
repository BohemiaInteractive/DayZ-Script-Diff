class JsonUndergroundTriggers
{
	ref array<ref JsonUndergroundAreaTriggerData> Triggers;
	
	static void SpawnParentedTriggers(EntityAI parent)
	{
		int networkIdLow, networkIdHigh;
		parent.GetNetworkID(networkIdLow, networkIdHigh);
		
		JsonUndergroundTriggers jsonData = UndergroundAreaLoader.GetData();
		if (jsonData && jsonData.Triggers)
		{
			foreach (int index, auto data : jsonData.Triggers)
			{
				if (data.CustomSpawn)
				{
					if (data.ParentNetworkId.Count() == 2 && data.ParentNetworkId[0] == networkIdLow && data.ParentNetworkId[1] == networkIdHigh)
						SpawnTriggerCarrier(parent, index, data);
				}
			}
		}
	}
	
	static void SpawnTriggerCarrier(EntityAI parent, int index, JsonUndergroundAreaTriggerData data)
	{
		UndergroundTriggerCarrierBase carrier = UndergroundTriggerCarrierBase.Cast(g_Game.CreateObjectEx("UndergroundTriggerCarrier", data.GetPosition(), ECE_NONE));
		if (carrier)
		{
			carrier.SetParent(parent);
			carrier.SetIndex(index);
			carrier.SetOrientation(data.GetOrientation());
		}
	}
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
	bool 				LightLerp;	// only used in LinePointFade

	ref BreadcrumbExternalValueController ExternalValueController;
}

class BreadcrumbExternalValueController
{
	string Type;
	ref TStringArray Params;
}

class BreadcrumbDoorStateController
{
	string SelectionName;

	void BreadcrumbDoorStateController(TStringArray params)
	{
		SelectionName = params[0];
	}
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
	
	bool CustomSpawn;
	string Tag;						//! used for ObjectSpawner/Manual(custom) spawn
	ref TIntArray ParentNetworkId;	//! used on map placed objects

	ref array<float> Position;
	ref array<float> Orientation;
	ref array<float> Size;
	float  EyeAccommodation;
	float  InterpolationSpeed;
	bool UseLinePointFade;		// simple fade between points which are defined using existing breadcrumbs array 
	string AmbientSoundType;	// type of ambient sound which will be played by sound controller
	string AmbientSoundSet;		// manual playback of ambient sound
	
	ref array<ref JsonUndergroundAreaBreadcrumb> Breadcrumbs;
	
};


class UndergroundAreaLoader
{
	private static string m_Path = "$mission:cfgundergroundtriggers.json";
	
	static ref JsonUndergroundTriggers m_JsonData;	
	
	static JsonUndergroundTriggers GetData()
	{
		if (!FileExist(m_Path))
		{
			// We fallback to check in data and notify user file was not found in mission
			PrintToRPT("[WARNING] :: [UndergroundAreaLoader GetData()] :: file not found in MISSION folder, your path is " + m_Path + " Attempting DATA folder");
			
			string worldName;
			g_Game.GetWorldName(worldName);
			m_Path = string.Format("dz/worlds/%1/ce/cfgundergroundtriggers.json", worldName);
			
			if (!FileExist(m_Path))
			{
				PrintToRPT("[WARNING] :: [UndergroundAreaLoader GetData()] ::file not found in DATA folder, your path is " + m_Path);
				return null; // Nothing could be read, just end here
			}
		}

		string errorMessage;
		JsonUndergroundTriggers data;
		if (!JsonFileLoader<JsonUndergroundTriggers>.LoadFile(m_Path, data, errorMessage))
			ErrorEx(errorMessage);
		
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
			if (data.CustomSpawn)
				continue;
			
			SpawnTriggerCarrierEx(i, data);
		}
	}
	
	static UndergroundTriggerCarrierBase SpawnTriggerCarrierEx(int index, JsonUndergroundAreaTriggerData data)
	{
		UndergroundTriggerCarrierBase carrier = UndergroundTriggerCarrierBase.Cast(g_Game.CreateObjectEx("UndergroundTriggerCarrier", data.GetPosition(), ECE_NONE));
		if (carrier)
		{
			carrier.SetIndex(index);
			carrier.SetOrientation(data.GetOrientation());
		}
		
		return carrier;
	}
	
	static void SpawnTriggerCarrier(int index, JsonUndergroundAreaTriggerData data)
	{
		SpawnTriggerCarrierEx(index, data);
	}
	
	//---------------------------------------------------------------------------------------
	static void SyncDataSend(PlayerIdentity identity)
	{
		g_Game.RPCSingleParam(null, ERPCs.RPC_UNDERGROUND_SYNC, new Param1<JsonUndergroundTriggers>(m_JsonData), true, identity);
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
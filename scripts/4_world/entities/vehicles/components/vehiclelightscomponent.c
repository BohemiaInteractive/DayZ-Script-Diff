class VehicleLightSelectionData
{
	int m_SelectionId;
	string m_MaterialName;

	void VehicleLightSelectionData(int id, string name)
	{
		m_SelectionId 	= id;
		m_MaterialName 	= name;
	}
}

class VehicleLightData
{
	string m_Point;
	string m_TargetPoint;
	
	VehicleLightBase m_Light;
	ref VehicleLightProfileBase m_LightProfile;
	
	void VehicleLightData(VehicleLightProfileBase profile)
	{
		m_LightProfile = profile;
	}
}

class VehicleLightType
{
	const int NORMAL 		= 0;
	const int BRAKE 		= 1;
	const int REVERSE 		= 2;
	const int REVERSE_BRAKE = 3;
}

class VehicleLightMode
{
	const int SEGREGATED = 0;
	const int AGGREGATED = 1;
}

class VehicleLightsComponent
{
	private EntityAI m_Parent;

	protected ref map<string, ref array<ref VehicleLightSelectionData>> m_SelectionMap;
	protected ref map<string, ref VehicleLightData> m_LightMap;

	void VehicleLightsComponent(notnull EntityAI parent)
	{
		m_Parent = parent;
		
		m_SelectionMap = new map<string, ref array<ref VehicleLightSelectionData>>();
		m_LightMap = new map<string, ref VehicleLightData>();
	}
	
	void RegisterSelection(string name, array<ref VehicleLightSelectionData> data)
	{
		m_SelectionMap.Set(name, data);
	}
	
	void RegisterLight(string name, VehicleLightData data)
	{
		m_LightMap.Set(name, data);
	}
	
	void SetSelectionMaterial(string name)
	{
		array<ref VehicleLightSelectionData> data = m_SelectionMap.Get(name);
		if (!data || (data && data.Count() == 0))
		{
			ErrorEx(string.Format("No VehicleLightSelectionData available for key:%1", name));
			return;
		}

		foreach (VehicleLightSelectionData selectionData : data)
		{
			string material = m_Parent.ConfigGetString(selectionData.m_MaterialName);
			if (!material.Empty)
				m_Parent.SetObjectMaterial(selectionData.m_SelectionId, material);
		}
	}
	
	void EnableLight(string name)
	{
		VehicleLightData data = m_LightMap.Get(name);
		if (data)
		{
			if (!data.m_Light)
			{
				VehicleLightBase light = VehicleLightBase.Cast(ScriptedLightBase.CreateLight(VehicleLightBase));
				light.SetLightProfile(data.m_LightProfile);
				
				data.m_Light = light;
			}
			#ifdef DIAG_DEVELOPER
			else
				ErrorEx(string.Format("m_Light is already assigned for '%1' (parent:%2)", name, m_Parent));
			#endif
		}
		#ifdef DIAG_DEVELOPER
		else
			ErrorEx(string.Format("'%1' is not registered light key", name));
		#endif
	}
	
	bool LightExist(string name)
	{
		VehicleLightData data = m_LightMap.Get(name);
		if (data && data.m_Light)
			return true;
		
		return false;
	}
	
	void AttachLightOnPoint(string name, string pointName, string targetPointName, int mode)
	{
		VehicleLightData data = m_LightMap.Get(name);
		if (data && data.m_Light)
		{
			data.m_Light.AttachOnMemoryPoint(m_Parent, pointName, targetPointName);
			if (mode == VehicleLightMode.SEGREGATED)
				data.m_Light.SegregateLight();
			else if (mode == VehicleLightMode.AGGREGATED)
				data.m_Light.AggregateLight();
		}
	}
	
	void AttachLightOnObject(string name, vector position, vector orientation, int mode, int boneIndex = -1)
	{
		VehicleLightData data = m_LightMap.Get(name);
		if (data && data.m_Light)
		{
			data.m_Light.AttachOnObjectPivot(m_Parent, boneIndex, position, orientation);
			if (mode == VehicleLightMode.SEGREGATED)
				data.m_Light.SegregateLight();
			else if (mode == VehicleLightMode.AGGREGATED)
				data.m_Light.AggregateLight();
		}
	}
	
	void DisableLight(string name)
	{
		VehicleLightData data = m_LightMap.Get(name);
		if (data && data.m_Light)
		{
			data.m_Light.FadeOut();
			data.m_Light = null;
		}
	}
	
	void DestroyLight(string name)
	{
		VehicleLightData data = m_LightMap.Get(name);
		if (data && data.m_Light)
		{
			data.m_Light.Destroy();
			data.m_Light = null;
		}	
	}
	
	void SetLightTypeAndMode(string name, int lightType, int mode)
	{
		VehicleLightData data = m_LightMap.Get(name);
		if (data && data.m_Light)
		{
			switch (lightType)
			{
				case VehicleLightType.NORMAL:
					if (mode == VehicleLightMode.SEGREGATED)
						data.m_Light.SegregateLight();
					else if (mode == VehicleLightMode.AGGREGATED)
						data.m_Light.AggregateLight();
					break;

				case VehicleLightType.BRAKE:
					data.m_Light.SetAsSegregatedBrakeLight();
					break;
				
				case VehicleLightType.REVERSE:
					data.m_Light.SetAsSegregatedReverseLight();
					break;
				
				case VehicleLightType.REVERSE_BRAKE:
					data.m_Light.AggregateLight();
					data.m_Light.SetFadeOutTime(1);
					break;
			}
		}
	}
}

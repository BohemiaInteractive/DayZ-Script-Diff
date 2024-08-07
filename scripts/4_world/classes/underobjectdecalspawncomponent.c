class UnderObjectDecalSpawnSettings
{
	bool m_RandomizeRotation = true;
	
	float m_ScaleMin = 0.1;
	float m_ScaleMax = 1.0;
	
	vector m_PositionOffset = vector.Zero;
}

class UnderObjectDecalSpawnComponent
{
	private const string SURFACE_PARAM_DECAL_NAME = "underObjectDecal";

	protected ref UnderObjectDecalSpawnSettings m_Settings;
	
	protected Object m_Parent;
	protected Object m_Decal;
	
	private float m_LastScaleValue;

	void UnderObjectDecalSpawnComponent(notnull UnderObjectDecalSpawnSettings pSettings, notnull Object pParent)
	{
		m_Settings 	= pSettings;
		m_Parent	= pParent;
	}
	
	private string GetObjectNameFromSurfaceConfig(string surfaceParamName = SURFACE_PARAM_DECAL_NAME)
	{
		string surfaceType
		int liquidType;
		g_Game.SurfaceUnderObjectCorrectedLiquid(m_Parent, surfaceType, liquidType);

		return Surface.GetParamText(surfaceType, surfaceParamName);
	}

	void SpawnDecal()
	{
		if (m_Decal)
			return;

		#ifndef SERVER
		m_Decal = g_Game.CreateObjectEx(
			GetObjectNameFromSurfaceConfig(),
			m_Parent.GetPosition() + m_Settings.m_PositionOffset,
			ECE_LOCAL|ECE_PLACE_ON_SURFACE,
		);

		if (m_Decal)
		{
			if (m_Settings.m_RandomizeRotation)
			{
				vector v;
				v[0] = Math.RandomFloat(-Math.PI, Math.PI);
				m_Decal.SetOrientation(v * Math.RAD2DEG);
			}
			
			m_Decal.SetScale(m_Settings.m_ScaleMin);
		}
		#endif
	}
	
	void RemoveDecal()
	{
		#ifndef SERVER
		if (m_Decal)
			m_Decal.Delete();
		#endif
	}
	
	void UpdateSize(float pScaleValue)
	{
		#ifndef SERVER
		if (m_Decal)
		{
			if (pScaleValue != m_LastScaleValue)
			{
				m_LastScaleValue = m_Decal.GetScale();
				m_Decal.SetScale(Math.Clamp(pScaleValue, m_Settings.m_ScaleMin, m_Settings.m_ScaleMax));
			}
		}
		#endif
	}
}
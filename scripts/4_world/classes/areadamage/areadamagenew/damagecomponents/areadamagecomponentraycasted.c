//! Newer implementation equivalent of "AreaDamageRegularRaycasted", hitzone selection only
class AreaDamageComponentRaycasted : AreaDamageComponent
{
	// Defined in local space of the trigger
	ref array<vector> 				m_RaycastSources;
	vector 							m_RaycastEndOffset;
	
	ref array<ref RaycastRVResult> 	m_RaycastCache;
	int								m_RaycastCachedIndex;
	
	void AreaDamageComponentRaycasted(AreaDamageManager parent)
	{
		m_RaycastSources = new array<vector>;
		m_RaycastEndOffset = "0 0.5 0";
		
		m_RaycastCache = new array<ref RaycastRVResult>;
		m_RaycastCachedIndex = -1;
	}
	
	override void OnTriggerCreated()
	{
		super.OnTriggerCreated();
		
		ClearCache();
	}

	void SetRaycastSources( array<string> raycast_sources )
	{
		m_RaycastSources.Clear();
		
		// convert Array of string to array of Vectors
		int nrOfSources = raycast_sources.Count();
		for ( int v = 0; v < nrOfSources; ++v)
		{
			m_RaycastSources.Insert(raycast_sources[v].ToVector());
		}
	}
	
	void SetRaycastSourcesVector( array<vector> raycast_sources )
	{
		m_RaycastSources = raycast_sources;
	}
	
	void SetRaycastLength(float length)
	{
		m_RaycastEndOffset = Vector(0, length, 0);
	}
	
	override void OnStayFinishServerEvent()
	{
		super.OnStayFinishServerEvent();
		
		ClearCache();
	}
	
	override protected AreaDamageComponentData GetAreaDamageComponentData(Object object)
	{
		AreaDamageComponentData data = new AreaDamageComponentData;
		data.Hitzone = GetRaycastedHitZone(object);
		
		return data;
	}
	
	protected void ClearCache()
	{
		m_RaycastCache.Clear();
		m_RaycastCachedIndex = -1;
	}
	
	protected string GetRaycastedHitZone(Object victim)
	{
		int nrOfCachedResults = m_RaycastSources.Count();
		for ( int c = 0; c < nrOfCachedResults; ++c )
		{
			RaycastRVResult cachedRes = m_RaycastCache[c];
			if ( cachedRes.obj == victim )
				return victim.GetDamageZoneNameByComponentIndex(cachedRes.component);
		}
		
		int nrOfSources = m_RaycastSources.Count();
		array<ref RaycastRVResult> victims = new array<ref RaycastRVResult>;
		
		string hitzone = "";
		
		AreaDamageTriggerBase trigger = m_Parent.GetTrigger();
		
		RaycastRVParams params = new RaycastRVParams( vector.Zero, vector.Zero, trigger, 0.0 );
		params.type = ObjIntersectIFire;
		params.flags = CollisionFlags.ONLYDYNAMIC;
		
		for ( int i = m_RaycastCachedIndex + 1; i < nrOfSources; ++i )
		{
			m_RaycastCachedIndex = i;
			
			params.begPos = trigger.ModelToWorld( m_RaycastSources[i] );
			params.endPos = params.begPos + m_RaycastEndOffset;
			
			if ( DayZPhysics.RaycastRVProxy(params, victims) )
			{			
				for ( int j = 0; j < victims.Count(); ++j )
				{
					RaycastRVResult res = victims[j];
					
					if (res.obj == victim)
						hitzone = victim.GetDamageZoneNameByComponentIndex(res.component);
					
					if (res.obj.IsAnyInherited(m_DamageableTypes))
						m_RaycastCache.Insert(res);
				}
				
				if ( !( hitzone == "") )
					return hitzone;
				
				victims.Clear();
			}
		}
		
		return GetFallbackHitZone(victim);
	}
	
	protected string GetFallbackHitZone(Object victim)
	{
		Error(string.Format("[WARNING] :: [%1] :: [AreaDamageComponentRaycasted] :: No proper HitZone found for damaging %2, using fallback.",
			m_Parent, Object.GetDebugName(victim)));
		
		// Fallbacks, currently are implemented assuming that foot/leg damagezones would be desired to damage
		if ( victim.IsInherited(DayZPlayer) || victim.IsInherited(DayZInfected) )
		{
			// Damage random leg since we don't know what part of player's body was caught in the trap.
			if ( Math.RandomIntInclusive(0, 1) == 1 )
				return "RightFoot";
			return "LeftFoot";
		}
		else
		{					
			array<string> damageZones = new array<string>;		
			victim.GetDamageZones(damageZones);
			
			int nrOfDmgZones = damageZones.Count();
			
			if (nrOfDmgZones > 0)
			{
				for (int z = 0; z < nrOfDmgZones; ++z)
				{
					if ( damageZones[z].Contains("Foot") || damageZones[z].Contains("Leg") )
						return damageZones[z];
				}
				
				return damageZones.GetRandomElement();
			}
			else
				return "";
		}
	}
}
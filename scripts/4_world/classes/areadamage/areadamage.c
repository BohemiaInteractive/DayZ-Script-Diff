// DEPRECATED: Backwards compatibility class to prevent existing mods breaking
// I wish I could rename this to AreaDamageTimer, but can't, because of backwards compatibility with mods
class AreaDamageBase : AreaDamageManager
{
	protected AreaDamageBase			m_AreaDamage;
	
	protected float						m_PlayerDamage;
	protected float						m_OthersDamage;
	
	protected string					m_AmmoName;
	protected int 						m_DamageType;
	
	protected float						m_LoopInterval;
	protected float						m_DeferDuration;
	
	protected ref array<string>			m_HitZones;
	protected ref array<string> 		m_RaycastSources;
	protected ref array<typename>		m_DamageableTypes
	
	protected ref Timer					m_LoopTimer;
	protected ref Timer					m_DeferTimer;
	
	void AreaDamageBase(EntityAI parent)
	{
		m_AreaDamage		= this;
		
		m_PlayerDamage		= 0.0;
		m_OthersDamage		= 0.0;
		
		m_AmmoName			= "MeleeDamage";
		m_DamageType 		= DT_CUSTOM;
		
		m_LoopInterval 		= 1.0;
		m_DeferDuration		= 1.0;
		
		m_HitZones			= new array<string>;
		m_RaycastSources	= new array<string>;
		m_DamageableTypes	= new array<typename>;
		m_DamageableTypes.Insert(DayZPlayer);
		
		m_LoopTimer 		= new Timer(CALL_CATEGORY_SYSTEM);
		m_DeferTimer 		= new Timer(CALL_CATEGORY_SYSTEM);
		
		m_TriggerBaseClass = "AreaDamageTrigger";
	}
	
	//! events
	void OnEnter(Object object)
	{
		if ( GetGame().IsServer() )
		{
			OnEnterServer(object);
		}
		else
		{
			OnEnterClient(object);
		}
	}
	void OnEnterClient(Object object) {}
	void OnEnterServer(Object object) {}
	
	void OnLeave(Object object)
	{
		if ( GetGame().IsServer() )
		{
			OnLeaveServer(object);
		}
		else
		{
			OnLeaveClient(object);
		}
	}

	void OnLeaveClient(Object object) {}
	void OnLeaveServer(Object object)
	{
		//! stop all running timers
		if ( m_DeferTimer && m_DeferTimer.IsRunning() )
		{
			m_DeferTimer.Stop();
		}
		if ( m_LoopTimer && m_LoopTimer.IsRunning() )
		{
			m_LoopTimer.Stop();
		}
	}
	
	protected void EvaluateDamage_Loop(Object object)
	{
		m_LoopTimer.Run(m_LoopInterval, this, "EvaluateDamage", new Param1<Object>(object), true);
	}

	protected void EvaluateDamage_Defer(Object object)
	{
		m_DeferTimer.Run(m_DeferDuration, this, "EvaluateDamage", new Param1<Object>(object), false);
	}
	
	protected void EvaluateDamage_DeferLoop(Object object)
	{
		m_DeferTimer.Run(m_DeferDuration, this, "EvaluateDamage_Loop", new Param1<Object>(object), false);
	}
	
	protected void EvaluateDamage(Object object)
	{
		string hitzone;

		if ( m_RaycastSources.Count() )
		{
			hitzone = GetRaycastedHitZone(object, m_RaycastSources);
			EvaluateDamage_Common(object, hitzone);
		}
		else
		{
			hitzone = GetRandomHitZone(m_HitZones);
			EvaluateDamage_Common(object, hitzone);
		}
	}
	
	protected void EvaluateDamage_Common(Object object, string hitzone)
	{
		if ( object && object.IsAlive() )
		{
			if ( object.IsAnyInherited( m_DamageableTypes ) )
			{
				//If we are hitting an infected or animal, we increase the damage dealt as they do not bleed
				//Change is multiplier
				/*DayZInfected dayzInfected = DayZInfected.Cast(object);
				DayZAnimal dayzAnimal = DayZAnimal.Cast(object);
				EntityAI eai = EntityAI.Cast(object);
				if ( dayzInfected || dayzAnimal )
				{
					//Agents should not take damage from fireplace, but just in case, keep multiplier relatively low
					if ( hitzone )
					{
						eai.ProcessDirectDamage(m_DamageType, EntityAI.Cast(m_ParentObject), hitzone, m_AmmoName, "0.5 0.5 0.5", 8);
					}
					else
						eai.ProcessDirectDamage(m_DamageType, EntityAI.Cast(m_ParentObject), "", m_AmmoName, "0.5 0.5 0.5", 8);
	}
				else*/
				{
					object.ProcessDirectDamage(m_DamageType, m_ParentObject, hitzone, m_AmmoName, "0.5 0.5 0.5", 1);
				}
				PostDamageActions();
			}
		}
	}
	
	
	//! ------------------------------------------------------
	//! common
	//!
	
	protected string GetRandomHitZone(array<string> hitzones)
	{
		Math.Randomize(-1);
		int idx = Math.RandomInt( 0, 100 ) % hitzones.Count();

		return hitzones[idx];
	}
	
	protected string GetRaycastedHitZone(Object victim, array<string> raycast_sources_str)
	{
		
		// Vertical raycast start positions:    Center,      North East,    North West,    South East,  South West
		//vector raycast_sources[5] = {"0.0 0.1 0.0", "0.2 0.1 0.2", "-.2 0.1 0.2", "0.2 0.1 -.2", "-.2 0.1 -.2"};
		
		string hitzone;
		vector contact_pos;
		vector contact_dir;
		int contactComponent;
		bool isSteppedOn = false;
		
		array<vector> raycast_sources = new array<vector>;
		ref set<Object> victims = new set<Object>;

		// convert Array of string to array of Vectors
		for ( int v = 0; v < raycast_sources_str.Count(); ++v)
		{
			raycast_sources.Insert(raycast_sources_str[v].ToVector());
		}
		
		for ( int i = 0; i < raycast_sources.Count(); ++i )
		{
			vector raycast_start_pos = m_AreaDamageTrigger.ModelToWorld( raycast_sources.Get(i) );
			vector raycast_end_pos = "0 0.5 0" + raycast_start_pos;
			
			//#ifdef DEVELOPER
			//Debug.DrawArrow( raycast_start_pos, raycast_end_pos );
			//#endif
			DayZPhysics.RaycastRV( raycast_start_pos, raycast_end_pos, contact_pos, contact_dir, contactComponent, victims , null, m_AreaDamageTrigger, true, false, ObjIntersectIFire);
			
			for ( int j = 0; j < victims.Count(); ++j )
			{
				Object contact_obj = victims.Get(j);
				
				if ( contact_obj.IsAnyInherited(m_DamageableTypes) )
				{
					isSteppedOn = true;
					break;
				}
			}
			
			if ( isSteppedOn )
			{
				EntityAI eai = EntityAI.Cast(victim);
				if ( eai )
				{
					hitzone = eai.GetDamageZoneNameByComponentIndex(contactComponent);
					break;
				}
			}
		}
		
		if ( isSteppedOn )
		{
			return hitzone;
		}
		else
		{
			// Damage random leg since we don't know what part of player's body was caught in the trap.
			string dmg_zone_rnd = "LeftFoot";
			if ( Math.RandomIntInclusive(0, 1) == 1 )
				dmg_zone_rnd = "RightFoot";
			return dmg_zone_rnd;
		}
	}
	
	//! -----------------------------------------------------
	//! DEBUG
	//!

#ifdef DEVELOPER
	ref array<Shape> triggerAreaShapes = new array<Shape>();
	
	void EnableDebug(bool pState = false)
	{
		//if (GetGame() && (!GetGame().IsClient() || GetGame().IsMultiplayer()))
			//return;

		if ( pState )
		{
			Debug_DrawArea();
		}
		else
		{
			Debug_CleanupShapes(triggerAreaShapes);
		}
	}

	protected void Debug_DrawArea()
	{
		vector min = m_AreaPosition + m_ExtentMin;
		vector max = m_AreaPosition + m_ExtentMax;

		triggerAreaShapes.Insert(Debug.DrawBox(min, max));
	}

	private void Debug_CleanupShapes(array<Shape> shapesArr)
	{
		for ( int it = 0; it < shapesArr.Count(); ++it )
		{
			Debug.RemoveShape( shapesArr[it] );
		}
		
		shapesArr.Clear();
	}
#endif
}
class BearTrap extends TrapBase
{
	static const int RAYCAST_SOURCES_COUNT = 5;
	// Raycasts start positions:
	// Positions are local to model. Vertical offset prevents ground collision.
	static const vector m_RaycastSources[RAYCAST_SOURCES_COUNT] = {
		"0.0 0.1 0.0",	// center
		"0.2 0.1 0.2",	// north east
		"-.2 0.1 0.2",	// north west
		"0.2 0.1 -0.2",	// south east
		"-0.2 0.1 -0.2"	// south west
	};
	
	void BearTrap()
	{
		m_DamagePlayers 			= 5; 				// How much damage player gets when caught
		m_DamageOthers 				= 5;         		// How much damage other entities(CreatureAI) gets when caught
		m_DefectRate 				= 0;
		m_InitWaitTime 				= 0.0; 				// After this time after deployment, the trap is activated
		m_AnimationPhaseGrounded 	= "placing";
		m_AnimationPhaseSet 		= "BearTrap_Set";
		m_AnimationPhaseTriggered 	= "placing";
	}
	
	override bool CanBeDisarmed()
	{
		return true;
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);

		if (g_Game.IsServer())
		{
			if (newLevel == GameConstants.STATE_RUINED)
			{
				SetInactive();
			}
		}
	}
	
	override void CreateTrigger()
	{
		super.CreateTrigger();

		vector mins		= "-0.1 -0.05 -0.1";
		vector maxs 	= "0.1 0.4 0.1";

		m_TrapTrigger.SetOrientation(GetOrientation());
		m_TrapTrigger.SetExtents(mins, maxs);
		m_TrapTrigger.SetParentObject(this);
	}
	
	override void OnUpdate(EntityAI victim)
	{
		if (victim && victim.IsInherited(CarScript))
		{
			EntityAI wheel = GetClosestCarWheel(victim);
			if (wheel)
			{
				OnServerSteppedOn(wheel, "");
			}
		}
	}
	
	override void OnSteppedOn(EntityAI victim)
	{
		if (g_Game.IsServer() && victim)
		{
			if (!victim.GetAllowDamage())
				return;
			
			if (victim.IsInherited(CarScript))
			{
				//! CarScript specific reaction on BearTrap
				Param1<EntityAI> params = new Param1<EntityAI>(victim);
				m_UpdateTimer.Run(UPDATE_TIMER_INTERVAL, this, "OnUpdate", params, true);

				return;
			}
			else
			{
				foreach (vector raycastSourcePosition: m_RaycastSources)
				{
					vector raycastStart 				= ModelToWorld(raycastSourcePosition);
					vector raycastEnd 					= "0 0.5 0" + raycastStart;
					
					RaycastRVParams rayInput 			= new RaycastRVParams(raycastStart, raycastEnd, this);
					rayInput.flags 						= CollisionFlags.ALLOBJECTS;
					rayInput.type 						= ObjIntersectFire;
					rayInput.radius 					= 0.05;
					array<ref RaycastRVResult> results 	= new array<ref RaycastRVResult>();
			
					if (DayZPhysics.RaycastRVProxy(rayInput, results))
					{
						foreach (RaycastRVResult result: results)
						{
							if (result.obj && !result.obj.IsDamageDestroyed() && !result.obj.IsAnyInherited({ItemBase, Plant}))
							{
								OnServerSteppedOn(result.obj, result.obj.GetDamageZoneNameByComponentIndex(result.component));
								return;
							}
						}
					}
				}

				OnServerSteppedOn(victim, "zone_leg_random");
			}
		}
		else if (!g_Game.IsDedicatedServer()) //! this is also called on client (OnRPC->SnapOn->OnSteppedOn chain)
		{
			if (victim)
			{
				if (victim.IsInherited(PlayerBase))
				{
					victim.SpawnDamageDealtEffect();
				}

				PlaySoundBiteLeg();
			}
		}
	}
	
	override void OnSteppedOut(EntityAI victim)
	{
		if (victim.IsInherited(CarScript))
		{
			if (m_UpdateTimer && m_UpdateTimer.IsRunning())
			{
				m_UpdateTimer.Stop();
			}
		}
	}
	
	protected void OnServerSteppedOn(Object obj, string damageZone)
	{
		if (obj.IsInherited(CarWheel))
		{
			obj.ProcessDirectDamage(DamageType.CLOSE_COMBAT, this, damageZone, "BearTrapHit_CarWheel", "0 0 0", 1);
			if (m_UpdateTimer.IsRunning())
			{
				m_UpdateTimer.Stop();
			}
			
			SetInactive(false);
			Synch(EntityAI.Cast(obj));
			
			return;
		}
		
		if (obj.IsDamageDestroyed())
			return;
		
		string zoneUsed = damageZone;
		if (damageZone == "zone_leg_random")
		{
			zoneUsed = "LeftLeg";
			if (Math.RandomIntInclusive(0, 1) == 1)
				zoneUsed = "RightLeg";
		}
		
		//! Generic limp handling
		ZombieBase zombie;
		if (obj.IsInherited(PlayerBase) || (Class.CastTo(zombie,obj) && !zombie.IsCrawling() && Math.RandomIntInclusive(0, 1) == 1))
		{
			CauseVictimToStartLimping(obj, "");
		}
		
		obj.ProcessDirectDamage(DamageType.CLOSE_COMBAT, this, zoneUsed, "BearTrapHit", "0 0 0", 1);
		
		SetInactive(false);
		Synch(EntityAI.Cast(obj));
	}
	
	// Causes the player to start limping. This is temporary and should at some point be replaced by broken legs
	void CauseVictimToStartLimping(Object obj, string damagedZone)
	{
		PlayerBase player;
		ZombieBase zombie;
		if (Class.CastTo(player,obj))
		{
			player.DamageAllLegs(player.GetMaxHealth() * 2); //reduce legs health (not regular DamageSystem damage, does not transfer!)
		}
		else if (Class.CastTo(zombie,obj))
		{
			zombie.SetHealth("LeftLeg", "Health", 0.0);
			zombie.SetHealth("RightLeg", "Health", 0.0);
		}
	}
	
	void PlaySoundBiteLeg()
	{
		EffectSound sound = SEffectManager.PlaySound("beartrapCloseDamage_SoundSet", GetPosition(), 0, 0, false);
		sound.SetAutodestroy(true);
	}
	
	void PlaySoundBiteEmpty()
	{
		EffectSound sound = SEffectManager.PlaySound("beartrapClose_SoundSet", GetPosition(), 0, 0, false);
		sound.SetAutodestroy(true);
	}
	
	void PlaySoundOpen()
	{
		EffectSound sound = SEffectManager.PlaySound("beartrapOpen_SoundSet", GetPosition(), 0, 0, false);
		sound.SetAutodestroy(true);
	}

	override void OnActivate()
	{
		#ifndef SERVER
		PlaySoundOpen();
		#endif
	}
	
	override void OnDisarm()
	{
		#ifndef SERVER
		PlaySoundBiteEmpty();
		#endif
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		
		if (g_Game.IsServer())
		{
			PlayerBase player_PB = PlayerBase.Cast(player);
			StartActivate(player_PB);
		}	
	}
	
	override bool IsDeployable()
	{
		return true;
	}
	
	override string GetLoopDeploySoundset()
	{
		return "beartrap_deploy_SoundSet";
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionClapBearTrapWithThisItem);
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionDeployObject);
	}

#ifdef DEVELOPER	
	//================================================================
	// DEBUG
	//================================================================
			
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		StartActivate(null);
	}
	
	override void GetDebugActions(out TSelectableActionInfoArrayEx outputList)
	{
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.ACTIVATE_ENTITY, "Activate", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.DEACTIVATE_ENTITY, "Deactivate", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		
		super.GetDebugActions(outputList);
	}
	
	override bool OnAction(int action_id, Man player, ParamsReadContext ctx)
	{
		if (super.OnAction(action_id, player, ctx))
			return true;
		if (g_Game.IsServer() || !g_Game.IsMultiplayer())
		{
			if (action_id == EActions.ACTIVATE_ENTITY)
			{
				StartActivate(null);
			}
			else if (action_id == EActions.DEACTIVATE_ENTITY)
			{
				SetInactive();
			}
		}
		return false;
	}
#endif
}

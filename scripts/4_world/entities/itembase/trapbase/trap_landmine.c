enum SoundTypeMine
{
	DISARMING = 0
}

class LandMineTrap extends TrapBase
{
	protected ref EffectSound m_TimerLoopSound;
	protected ref EffectSound m_DisarmingLoopSound;
	protected ref Timer m_DeleteTimer;
	
	private const int BROKEN_LEG_PROB 	= 90;
	private const int BLEED_SOURCE_PROB = 50;
	private const int MAX_BLEED_SOURCE 	= 1;
	
	void LandMineTrap()
	{
		m_DefectRate 			= 15;
		m_DamagePlayers 		= 0; 			//How much damage player gets when caught
		m_InitWaitTime 			= 10; 			//After this time after deployment, the trap is activated
		m_InfoActivationTime 	= string.Format("#STR_LandMineTrap0%1#STR_LandMineTrap1", m_InitWaitTime.ToString());
		
		m_AddDeactivationDefect = true;
		
		//Order is important and must match clothing array in DamageClothing method
		m_ClothingDmg = new array<int>;
		m_ClothingDmg.Insert(60); 	//Trousers
		m_ClothingDmg.Insert(100);	//BackPack
		m_ClothingDmg.Insert(40);	//Vest
		m_ClothingDmg.Insert(10);	//HeadGear
		m_ClothingDmg.Insert(10);	//Mask
		m_ClothingDmg.Insert(40);	//Body
		m_ClothingDmg.Insert(50);	//Feet
		m_ClothingDmg.Insert(5);	//Gloves
	}
	
	void ~LandMineTrap()
	{
		SEffectManager.DestroyEffect(m_TimerLoopSound);
		SEffectManager.DestroyEffect(m_DisarmingLoopSound);
	}
		
	override void StartActivate(PlayerBase player)
	{
		super.StartActivate(player);
		
		if (!GetGame().IsDedicatedServer())
		{
			EffectSound sound = SEffectManager.PlaySound("landmine_safetyPin_SoundSet", GetPosition(), 0, 0, false);
			sound.SetAutodestroy( true );
			m_TimerLoopSound = SEffectManager.PlaySound("landmine_timer2_SoundSet", GetPosition(), 0, 0, true);
		}
	}
	
	override void OnActivatedByItem(notnull ItemBase item)
	{
		SetHealth("", "", 0.0);
		DeleteThis();
	}
	
	override void OnActivate()
	{
		if (!GetGame().IsDedicatedServer())
		{
			if (m_TimerLoopSound)
			{
				m_TimerLoopSound.SetAutodestroy(true);
				m_TimerLoopSound.SoundStop();
			}
			
			if (GetGame().GetPlayer())
			{
				PlaySoundActivate();
			}
		}
	}
	
	override bool CanExplodeInFire()
	{
		return true;
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
		int i;

		if (GetGame().IsServer() && victim)
		{
			if (!victim.GetAllowDamage())
			{
				return;
			}

			if (victim.IsInherited(CarScript))
			{
				//! CarScript specific reaction on LandMineTrap
				Param1<EntityAI> params = new Param1<EntityAI>(victim);
				m_UpdateTimer.Run(UPDATE_TIMER_INTERVAL, this, "OnUpdate", params, true);

				return;
			}
			else
			{
				//Check if we have a player
				PlayerBase victim_PB = PlayerBase.Cast(victim);
				if (victim_PB && victim_PB.IsAlive())
				{
					int randNum; //value used for probability evaluation
					randNum = Math.RandomInt(0, 100);
					if (randNum <= BROKEN_LEG_PROB)
					{
						float damage = victim_PB.GetMaxHealth("RightLeg", ""); //deal 100% damage to break legs
						victim_PB.DamageAllLegs( damage ); 
					}
					
					randNum = Math.RandomInt(0, 100);
					if (randNum < BLEED_SOURCE_PROB)
					{
						for (i = 0; i < MAX_BLEED_SOURCE; i++)
						{
							//We add two bleeding sources max to lower half
							randNum = Math.RandomIntInclusive(0, PlayerBase.m_BleedingSourcesLow.Count() - 1);
					
							victim_PB.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesLow[randNum]);
						}
					}
					
					DamageClothing(victim_PB);
				}
				else
				{
					ItemBase victim_IB = ItemBase.Cast(victim);
					if (victim_IB)
					{
						MiscGameplayFunctions.DealAbsoluteDmg(victim_IB, DAMAGE_TRIGGER_MINE);
					}
				}
				
				Explode(DamageType.EXPLOSION);
			}

			DeleteThis();
		}
		
		super.OnSteppedOn(victim);
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
			obj.ProcessDirectDamage(DT_CLOSE_COMBAT, this, "", "LandMineExplosion_CarWheel", "0 0 0", 1);
			Explode(DamageType.EXPLOSION);
			if (m_UpdateTimer.IsRunning())
			{
				m_UpdateTimer.Stop();
			}
			
		}

		SetInactive(false);
		Synch(EntityAI.Cast(obj));
	}

	void DeleteThis()
	{
		m_DeleteTimer = new Timer(CALL_CATEGORY_SYSTEM);
		m_DeleteTimer.Run(1, this, "DeleteSafe");
	}
	
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner) 
	{
		super.OnItemLocationChanged(old_owner, new_owner);
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		Explode(DamageType.EXPLOSION);
	}
	
	void PlaySoundActivate()
	{
		if (!GetGame().IsDedicatedServer())
		{
			EffectSound sound = SEffectManager.PlaySound("landmineActivate_SoundSet", GetPosition(), 0, 0, false);
			sound.SetAutodestroy(true);
		}
	}
	
	override void Explode(int damageType, string ammoType = "")
	{
		if (ammoType == "")
		{
			ammoType = ConfigGetString("ammoType");
		}
		
		if (ammoType == "")
		{
			ammoType = "Dummy_Heavy";
		}
		
		if ( GetGame().IsServer() )
		{
			SynchExplosion();
			vector offset = Vector(0, 0.1, 0); //Vertical offset applied to landmine explosion (in meters)
			DamageSystem.ExplosionDamage(this, NULL, ammoType, GetPosition() + offset, damageType); //Offset explosion on Y axis
			DeleteThis();
		}
	}
	
	override bool CanBeDisarmed()
	{
		return true;
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		Param1<bool> p = new Param1<bool>(false);
				
		if (!ctx.Read(p))
		{
			return;
		}

		bool play = p.param1;
		
		switch (rpc_type)
		{
		case SoundTypeMine.DISARMING:
			if (play)
			{
				PlayDisarmingLoopSound();
			}
			else
			{
				StopDisarmingLoopSound();
			}
			break;
		}
	}
	
	void PlayDisarmingLoopSound()
	{
		if (!m_DisarmingLoopSound || !m_DisarmingLoopSound.IsSoundPlaying())
		{
			m_DisarmingLoopSound = SEffectManager.PlaySound("landmine_deploy_SoundSet", GetPosition());
		}
	}
	
	void StopDisarmingLoopSound()
	{
		m_DisarmingLoopSound.SoundStop();
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
		
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		
		if (GetGame().IsServer())
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
		return "landmine_deploy_SoundSet";
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttach);
		AddAction(ActionDetach);
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionActivateTrap);
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
	
	override void GetDebugButtonNames(out string button1, out string button2, out string button3, out string button4)
	{
		button1 = "Activate";
		button2 = "Deactivate";
	}
	
	override void OnDebugButtonPressServer(int button_index)
	{
		switch (button_index)
		{
			case 1:
				StartActivate(null);
			break;
			case 2:
				SetInactive();
			break;
		}
		
	}
#endif
}

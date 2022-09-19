class ClaymoreMine : ExplosivesBase
{
	protected const string ANIM_PHASE_PACKED	= "Bag";
	protected const string ANIM_PHASE_DEPLOYED	= "Mine";
	protected const string SELECTION_NAME_LED	= "LED";

	protected ref RemotelyActivatedItemBehaviour 	m_RAIB;
	protected ERemoteDetonatorLEDState 				m_LastLEDState;
	
	void ClaymoreMine()
	{
		m_RAIB = new RemotelyActivatedItemBehaviour(this);
		
		SetAmmoTypes({"ClaymoreMine_Ammo","ClaymoreMine_Secondary_Ammo"});
		SetParticleExplosion(ParticleList.CLAYMORE_EXPLOSION);
		SetParticleOrientation("90 0 0");

		RegisterNetSyncVariableInt("m_RAIB.m_PairDeviceNetIdLow");
		RegisterNetSyncVariableInt("m_RAIB.m_PairDeviceNetIdHigh");

		UpdateLED(ERemoteDetonatorLEDState.OFF, true);
	}
	
	override void EOnInit(IEntity other, int extra)
	{
		UpdateVisuals();
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

#ifdef DEVELOPER
#ifndef SERVER
		RemoveDebugVisuals();
#endif
#endif
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

#ifdef DEVELOPER
#ifndef SERVER
		RemoveDebugVisuals();
#endif
#endif
	}
	
	override protected void InitiateExplosion()
	{
		if (GetDefused())
		{
			return;
		}
		
		super.InitiateExplosion();
	}
	
	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
		
		UpdateVisuals();
		if (GetArmed())
		{
			UpdateLED(ERemoteDetonatorLEDState.LIT, true);
		}
	}

 	override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();
		
		if (m_RAIB)
		{
			m_RAIB.OnVariableSynchronized();
		}
		
		UpdateVisuals();
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		
		if (m_RAIB)
		{
			m_RAIB.Pair();
		}
	}
	
	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
		{
			return false;
		}

		return IsTakeable();
	}
	
	override bool CanPutIntoHands(EntityAI parent)
	{
		if (!super.CanPutIntoHands(parent))
		{
			return false;
		}

		return IsTakeable();
	}

	override bool CanRemoveFromHands(EntityAI parent)
	{
		return IsTakeable();
	}
	
	override RemotelyActivatedItemBehaviour GetRemotelyActivatedItemBehaviour()
	{
		return m_RAIB;
	}
	
	override void PairRemote(notnull EntityAI trigger)
	{
		m_RAIB.Pair(trigger);
	}
	
	override void UnpairRemote()
	{
		if (GetPairDevice())
		{
			GetPairDevice().UnpairRemote();
		}

		m_RAIB.Unpair();
	}
	
	override EntityAI GetPairDevice()
	{
		return m_RAIB.GetPairDevice();
	}
	
	override bool CanBeDisarmed()
	{
		return GetArmed();
	}
	
	override void OnActivatedByItem(notnull ItemBase item)
	{
		if (GetGame().IsServer())
		{
			if (m_RAIB.IsPaired() && GetArmed())
			{
				if (GetPairDevice() == item)
				{
					SetHealth("", "", 0.0);
				}
			}
		}
	}
	
	override void OnArmed()
	{
		super.OnArmed();

		UpdateVisuals();
		UpdateLED(ERemoteDetonatorLEDState.LIT, true);
		
#ifdef DEVELOPER
#ifndef SERVER
		// have to call this function a little later, after claymore transform has been finalized
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DrawDamageZone, 500);
#endif
#endif
	}
	
	override void OnDisarmed(bool pWithTool)
	{
		super.OnDisarmed(pWithTool);
		
#ifdef DEVELOPER
		RemoveDebugVisuals();
#endif

		SetDefused(pWithTool);
		UnpairRemote();

		if (pWithTool)
		{
			SetHealth("", "", 0.0);
		}
		
		UpdateVisuals();
		UpdateLED(ERemoteDetonatorLEDState.OFF, true);
	}
	
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);

		if (GetGame().IsServer())
		{
			RemoteDetonatorTrigger rdt = RemoteDetonatorTrigger.SpawnInPlayerHands(player);
			if (rdt)
			{
				rdt.SetControlledDevice(this);
				PairRemote(rdt);
				Arm();
			}
		}
	}

	protected void UpdateVisuals()
	{
		if (GetArmed() || GetDefused())
		{
			ShowSelection(ANIM_PHASE_DEPLOYED);
			HideSelection(ANIM_PHASE_PACKED);
			if (GetOnViewIndexChanged())
			{
				GetOnViewIndexChanged().Invoke();
			}
		}
		else
		{
			HideSelection(ANIM_PHASE_DEPLOYED);
			ShowSelection(ANIM_PHASE_PACKED);
			if (GetOnViewIndexChanged())
			{
				GetOnViewIndexChanged().Invoke();
			}
		}
	}
	
	protected void UpdateLED(ERemoteDetonatorLEDState pState, bool pForced = false)
	{
		if (pState != m_LastLEDState || pForced)
		{
			int selectionIdx = GetHiddenSelectionIndex(SELECTION_NAME_LED);

			switch (pState)
			{
			case ERemoteDetonatorLEDState.LIT:
				SetObjectTexture(selectionIdx, RemoteDetonator.COLOR_LED_LIT);
			break;
			default:
				SetObjectTexture(selectionIdx, RemoteDetonator.COLOR_LED_OFF);
				break;
			}

			m_LastLEDState = pState;
		}
	}

	override bool IsTakeable()
	{
		return !GetArmed();
	}
	
	override bool IsDeployable()
	{
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionTogglePlaceObject);
		AddAction(ActionDeployObject);
		AddAction(ActionArmExplosive);
	}
	
	override int GetViewIndex()
	{
		if (MemoryPointExists("invView2"))
		{
			if (GetArmed())
			{
				return 1;
			}
		}
		
		return 0;
	}

	
#ifdef DEVELOPER	
	//================================================================
	// DEBUG
	//================================================================
	
	protected Shape m_DamageZone;
			
	override void OnDebugSpawn()
	{
		RemoteDetonatorTrigger rdt = RemoteDetonatorTrigger.Cast(SpawnEntityOnGroundPos("RemoteDetonatorTrigger", GetPosition() + GetDirection() * 0.5));
		rdt.SetControlledDevice(this);
		PairRemote(rdt);
		Arm();
	}

	override protected string GetDebugText()
	{
		string debug_output;
		debug_output += string.Format("low net id: %1\n", m_RAIB.GetPairDeviceNetIdLow());
		debug_output += string.Format("high net id: %1\n", m_RAIB.GetPairDeviceNetIdHigh());
		debug_output += string.Format("pair device: %1\n", m_RAIB.GetPairDevice());

		return debug_output;
	}
	
	protected void DrawDamageZone()
	{
		if (!DiagMenu.GetBool(DiagMenuIDs.DM_WEAPON_CLAYMORE_DEBUG))
		{
			return;
		}
		
		auto game 					= GetGame();
		string cfgPath 				= "CfgAmmo " + m_AmmoTypes[0];
		float hitRange 				= game.ConfigGetFloat(cfgPath + " indirectHitRange");
		float hitRangeMultiplier 	= game.ConfigGetFloat(cfgPath + " indirectHitRangeMultiplier");
		float verticalAngle 		= game.ConfigGetFloat(cfgPath + " indirectHitAngle1");
		float horizontalAngle 		= game.ConfigGetFloat(cfgPath + " indirectHitAngle2");
		float range 				= hitRange * hitRangeMultiplier;
		vector selfMatrix[4];

		RemoveDebugVisuals();
		
		GetTransform(selfMatrix);		
		m_DamageZone = Debug.DrawFrustum(horizontalAngle, verticalAngle, range);		
		m_DamageZone.SetMatrix(selfMatrix);		
	}
	
	void RemoveDebugVisuals()
	{
		if ( m_DamageZone )
		{
			m_DamageZone.Destroy();
			m_DamageZone = null;
		}
	}
#endif
}

class ClaymoreMinePlacing : ClaymoreMine {}

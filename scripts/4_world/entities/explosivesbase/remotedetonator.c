enum ERemoteDetonatorLEDState
{
	OFF = 0,
	LIT,
}

class RemoteDetonator : Inventory_Base
{
	const string COLOR_LED_OFF 		= "#(argb,8,8,3)color(0,0,0,1.0,co)";
	const string COLOR_LED_LIT 		= "#(argb,8,8,3)color(1,0,0,1.0,co)";
	const string SELECTION_NAME_LED = "LED";

	protected ERemoteDetonatorLEDState m_LastLEDState;
	
	bool IsKit()
	{
		return true;
	}
	
	void UpdateLED(ERemoteDetonatorLEDState pState, bool pForced = false)
	{
		int selectionIdx = GetHiddenSelectionIndex(SELECTION_NAME_LED);

		switch (pState)
		{
		case ERemoteDetonatorLEDState.LIT:
			SetObjectTexture(selectionIdx, COLOR_LED_LIT);
		break;
		default:
			SetObjectTexture(selectionIdx, COLOR_LED_OFF);
		}
		
		m_LastLEDState = pState;
		SetSynchDirty();
	}

	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionAttachExplosivesTrigger);
		AddAction(ActionDisarmExplosiveWithRemoteDetonatorUnpaired);
	}
}

class RemoteDetonatorTrigger : RemoteDetonator
{
	protected const string ANIM_PHASE_TRIGGER = "trigger";
	
	protected bool m_LED;
	protected ref RemotelyActivatedItemBehaviour m_RAIB;
	
	void RemoteDetonatorTrigger()
	{
		m_RAIB = new RemotelyActivatedItemBehaviour(this);		

		RegisterNetSyncVariableInt("m_RAIB.m_PairDeviceNetIdLow");
		RegisterNetSyncVariableInt("m_RAIB.m_PairDeviceNetIdHigh");
		RegisterNetSyncVariableInt("m_LastLEDState", 0, EnumTools.GetEnumSize(ERemoteDetonatorLEDState));
		
		UpdateLED(m_LastLEDState);
	}
	
	override bool IsKit()
	{
		return false;
	}
	
 	override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

		if (m_RAIB)
		{
			m_RAIB.OnVariableSynchronized();
		}
		
		UpdateLED(m_LastLEDState);
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		 
		if (m_RAIB)
		{
			m_RAIB.Pair();
		}
	}
	
	override RemotelyActivatedItemBehaviour GetRemotelyActivatedItemBehaviour()
	{
		return m_RAIB;
	}
	
	override void OnCEUpdate()
	{
		super.OnCEUpdate();
		
		if (!IsRuined() && IsConnected() && GetControlledDevice())
		{
			if (vector.DistanceSq(GetPosition(), GetControlledDevice().GetPosition()) <= Math.SqrFloat(UAMaxDistances.EXPLOSIVE_REMOTE_ACTIVATION))
			{
				UpdateLED(ERemoteDetonatorLEDState.LIT);
				
				return;
			}
		}
		
		UpdateLED(ERemoteDetonatorLEDState.OFF);
	}
	
	override void PairRemote(notnull EntityAI trigger)
	{
		m_RAIB.Pair(trigger);
	}
	
	override void UnpairRemote()
	{
		UpdateLED(ERemoteDetonatorLEDState.OFF);
		m_RAIB.Unpair();
	}

	override void OnActivatedByItem(notnull ItemBase item)
	{
		if (GetGame().IsServer())
		{
			if (m_RAIB.IsPaired() && !IsRuined())
			{
				ItemBase device = ItemBase.Cast(GetControlledDevice());
				if (device && vector.DistanceSq(GetPosition(), device.GetPosition()) <= Math.SqrFloat(UAMaxDistances.EXPLOSIVE_REMOTE_ACTIVATION))
				{
					device.OnActivatedByItem(this);
				}
			}
		}
	}
	
	void SetControlledDevice(EntityAI pDevice)
	{
		m_RAIB.Pair(pDevice);
	}

	bool IsConnected()
	{
		return m_RAIB.IsPaired();
	}
	
	EntityAI GetControlledDevice()
	{
		return m_RAIB.GetPairDevice();
	}
	
	static RemoteDetonatorTrigger SpawnInPlayerHands(notnull EntityAI pEntity)
	{
		string type = "RemoteDetonatorTrigger";
		RemoteDetonatorTrigger rdt;
		PlayerBase player = PlayerBase.Cast(pEntity);
		if (player)
		{
			if (player.GetItemInHands())
			{
				ReplaceItemWithNewLambdaBase lambda = new ReplaceItemWithNewLambdaBase(player.GetItemInHands(), type);
				MiscGameplayFunctions.TurnItemIntoItemEx(player, lambda);
				rdt = RemoteDetonatorTrigger.Cast(player.GetItemInHands());
			}
			else
			{
				rdt = RemoteDetonatorTrigger.Cast(player.GetHumanInventory().CreateInHands(type));
			}
			
			//! item replaced, use different IK
			if (player.GetItemInHands())
			{
				player.GetItemAccessor().OnItemInHandsChanged();
			}
		}
		
		return rdt;
	}
	
	override void OnAnimationPhaseStarted(string animSource, float phase)
	{
		if (animSource == ANIM_PHASE_TRIGGER)
		{
			if (phase > 0.01)
			{
				UpdateLED(ERemoteDetonatorLEDState.OFF);
			}
		}
	}
	
	override void SetActions()
	{
		super.SetActions();

		RemoveAction(ActionAttachExplosivesTrigger);

		AddAction(ActionAttachOnTrap);
		AddAction(ActionDisarmExplosiveWithRemoteDetonator);
		AddAction(ActionTriggerRemotely);
	}
	
#ifdef DEVELOPER
	override protected string GetDebugText()
	{
		string debug_output;
		debug_output += string.Format("low net id: %1\n", m_RAIB.GetPairDeviceNetIdLow());
		debug_output += string.Format("high net id: %1\n", m_RAIB.GetPairDeviceNetIdHigh());
		debug_output += string.Format("pair device: %1\n", m_RAIB.GetPairDevice());
		return debug_output;
	}
#endif
}

class RemoteDetonatorReceiver : RemoteDetonator
{
	void RemoteDetonatorReceiver()
	{
		UpdateLED(ERemoteDetonatorLEDState.OFF, true);
	}
	
	override bool IsKit()
	{
		return false;
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		UpdateLED(ERemoteDetonatorLEDState.OFF);
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		UpdateLED(ERemoteDetonatorLEDState.OFF, true);
	}

	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionAttachExplosivesTrigger);
		RemoveAction(ActionDisarmExplosiveWithRemoteDetonatorUnpaired);
	}
}

class DigitalCodeLock : ItemBase
{
	const string MEMPOINT_INSIDE 	= "inside";
	const string MEMPOINT_CENTER	= "ce_center";

	protected const string ATTACHMENT_SLOT_BATTERY	= "BatteryD";
	protected const string SELECTION_NAME_BACK		= "back";
		
	protected ref CodeLockComponent m_CodeLockComponent;
	protected ref CodeLockVisualManager m_VisualManager;
	
	protected bool m_Reversed = false;
	
	// Sounds
	private const string CODELOCK_ACCESS_SOUND = "digitalLock_beep_Access_SoundSet";
	private const string CODELOCK_LOCK_SOUND = "digitalLock_mechanism_Lock_SoundSet"; 
	private const string CODELOCK_UNLOCK_SOUND = "digitalLock_mechanism_Unlock_SoundSet";
	private const string CODELOCK_ERROR_SOUND = "digitalLock_beep_Error_SoundSet";
	private const string CODELOCK_DIGITS_SOUND = "digitalLock_buttons_Digits_SoundSet";
	private const string CODELOCK_CLEAR_SOUND = "digitalLock_button_Clear_SoundSet";
	private const string CODELOCK_CONFIRM_SOUND = "digitalLock_button_Key_confirm_SoundSet";
	
	void DigitalCodeLock()
	{
		Init();
	}
	
	protected void Init()
	{
		m_CodeLockComponent = new CodeLockComponent(this);
		m_VisualManager = new CodeLockVisualManager(this, m_CodeLockComponent);
		
		RegisterNetSyncVariableInt("m_CodeLockComponent.m_DoorIndex");
		RegisterNetSyncVariableInt("m_CodeLockComponent.m_Stage");
		RegisterNetSyncVariableInt("m_CodeLockComponent.m_WrongCodeCount");
		RegisterNetSyncVariableInt("m_CodeLockComponent.m_UIAction");
		RegisterNetSyncVariableBool("m_CodeLockComponent.m_IsLocked");
		RegisterNetSyncVariableBool("m_CodeLockComponent.m_HasPIN");
		RegisterNetSyncVariableBool("m_CodeLockComponent.m_HasBruteForceProtection");
		RegisterNetSyncVariableBoolSignal("m_CodeLockComponent.m_HandleBruteForceProtection");
		RegisterNetSyncVariableBool("m_Reversed");
	}
	
	override void EEInit()
	{
		super.EEInit();
		
		// Set visuals on init
		ForceFullVisualsUpdate();
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		
		if (g_Game.IsServer() || !g_Game.IsMultiplayer()) // single player or server side multiplayer
		{
			if (newLoc.GetType() != InventoryLocationType.ATTACHMENT)
			{
				if (g_Game.IsDedicatedServer())
					SetSynchDirty();
			
				// Force update since animation need too change
				UpdateVisuals();
			}
		}
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		if (g_Game.IsServer() || !g_Game.IsMultiplayer()) // single player or server side multiplayer
		{
			//! Start energy consumption once battery is attached and the lock is already attached to a parent that can have a construction component
			if (slot_name == ATTACHMENT_SLOT_BATTERY)
			{
				Battery9V battery = Battery9V.Cast(item);
				if (battery)
				{
					float batteryEnergy = battery.GetCompEM().GetEnergy();
					if (batteryEnergy > 0 && HasValidParent())
					{
						m_CodeLockComponent.SetStage(CodeLockStage.SETTING);
						GetCompEM().SwitchOn();
					}
					
					UpdateVisuals();
				}
			}
		}
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		if (g_Game.IsServer() || !g_Game.IsMultiplayer()) // single player or server side multiplayer
		{
			//! Stop energy consumption once battery is detached with active energy manager
			if (slot_name == ATTACHMENT_SLOT_BATTERY)
			{
				m_CodeLockComponent.SetStage(CodeLockStage.NONE);
				GetCompEM().SwitchOff();
				
				UpdateVisuals();
			}
		}
	}
		
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		if (g_Game.IsServer() || !g_Game.IsMultiplayer()) // single player or server side multiplayer
		{
			//! Start energy consumption once code lock is attached with a battery and the lock parent can have a construction component
			string slotName = InventorySlots.GetSlotName(slot_id);
			if (slotName.IndexOf("Att_CodeLock") > -1)
			{
				Battery9V battery = GetBattery();
				if (battery && battery.GetCompEM().GetEnergy() > 0 && !GetCompEM().IsWorking())
				{
					m_CodeLockComponent.SetStage(CodeLockStage.SETTING);
					GetCompEM().SwitchOn();
				}
				
				if (parent && parent.CanUseConstruction())
				{
					BuildingBase buildingBase = BuildingBase.Cast(parent);
					if (buildingBase)
					{
						SetDoorInfoFromSlot(buildingBase, slotName);
					}
				}
			}
			
			UpdateVisuals();
		}
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		
		if (g_Game.IsServer() || !g_Game.IsMultiplayer()) // single player or server side multiplayer
		{
			//! Stop energy consumption once lock is detached with active energy manager
			if (GetCompEM().IsWorking())
				GetCompEM().SwitchOff();
			
			string slotName = InventorySlots.GetSlotName(slot_id);
			if (slotName.IndexOf("Att_CodeLock") > -1)
			{
				m_Reversed = false;
				m_CodeLockComponent.StopAllTimers();
				m_CodeLockComponent.OnParentDetached();
			}
			
			UpdateVisuals();
		}
	}
	
	override bool CanDetachAttachment(EntityAI parent)
	{
		if (IsLocked())
			return false;
		 
		return true;
	}
	
	protected void SetDoorInfoFromSlot(BuildingBase buildingBase, string slotName)
	{
		map<int, ref AdditionalDoorInfo> doorsInfoMap = buildingBase.GetDoorInfoHolder().GetDoorInfoMap();
		//! Get door index of the door this lock can manipulate fo ther attached lock based on related attachment slot from AdditionalDoorInfosHolder data
		if (doorsInfoMap && doorsInfoMap.Count() > 0)
		{
			int doorIndex = -1;
			AdditionalDoorInfo doorInfo;
			foreach(int doorInfoIndex, AdditionalDoorInfo doorInfoData: doorsInfoMap)
			{
				foreach(string relatedSlotName: doorInfoData.m_RelatedInventorySlotNames)
				{
					if (relatedSlotName == slotName)
					{
						doorIndex = doorInfoIndex;
						doorInfo = doorInfoData;
						break;
					}
				}
				
				if (doorIndex != -1)
					break;
			}
										
			if (doorIndex > -1)
			{
				m_CodeLockComponent.SetDoorIndex(doorIndex);
			}
			
			//! Check for door part rotation
			Rebuilding rebuilding = Rebuilding.Cast(buildingBase.GetConstructionBasic());
			if (rebuilding && doorInfo)
			{
				ConstructionPart constructionPart = rebuilding.GetConstructionPart(doorInfo.m_DoorConstructionPartName);
				ConstructionPartRebuildReversibleDoor reversibleDoorPart;
				if (constructionPart && Class.CastTo(reversibleDoorPart, constructionPart))
				{
					m_Reversed = reversibleDoorPart.IsReversedLocal();
				}
			}
		}	
	}
	
	//! Condition check called from ActionTakeItem and ActionTakeItemToHands
	bool CanDetachCodeLock(notnull PlayerBase player, int componentIndex)
	{
		if (HasValidParent() && !IsLocked())
		{
			//! GetReversedState is used to check if code lock is mounted on reversible doors
			if (!GetReversedState() && !MiscGameplayFunctions.MemPointDirectionalCheck(player, this, DigitalCodeLock.MEMPOINT_CENTER, DigitalCodeLock.MEMPOINT_INSIDE))
				return false;
		}
		
		return true;
	}
	
	override void OnWorkStop()
	{
		super.OnWorkStop();		
		
		if (g_Game.IsServer() || !g_Game.IsMultiplayer()) // single player or server side multiplayer
		{
			m_CodeLockComponent.StopAllTimers();
			m_CodeLockComponent.OnParentReset(false);
			
			ForceFullVisualsUpdate();
		}
	}
	
	override void OnWorkStart()
	{
		super.OnWorkStart();		
		
		if (g_Game.IsServer() || !g_Game.IsMultiplayer()) // single player or server side multiplayer
		{
			UpdateVisuals();
		}
	}

	override void OnWork(float consumed_energy)
	{
		super.OnWork(consumed_energy);
		
		if (g_Game.IsServer() || !g_Game.IsMultiplayer()) // single player or server side multiplayer
		{
			Battery9V battery = GetBattery();
			if (battery)
			{
				if (GetCompEM().IsWorking() && battery.GetCompEM().GetEnergy() <= 0)
				{
					GetCompEM().SwitchOff();
					m_CodeLockComponent.SetLockState(false, false);
				}
				
				float energyPercent = GetBatteryEnergy();
				if (m_VisualManager.HasBatteryLevelChanged())
				{
					UpdateVisuals();
					SetSynchDirty();
				}
			}
		}
	}
	
	//! Unlock AND drop
	void UnlockOnServer(EntityAI parent)
	{
		InventoryLocation inventoryLocation = new InventoryLocation;
		GetInventory().GetCurrentInventoryLocation(inventoryLocation);
		
		//drop entity from attachment slot
		if (g_Game.IsMultiplayer())
		{
			parent.GetInventory().DropEntity(InventoryMode.SERVER, parent, this);
		}
		else
		{
			parent.GetInventory().DropEntity(InventoryMode.LOCAL, parent, this);
		}
		
		vector groundPos;
		string slotName = InventorySlots.GetSlotName(inventoryLocation.GetSlot());
		if (parent.MemoryPointExists(slotName))
		{
			groundPos = parent.ModelToWorld(parent.GetMemoryPointPos(slotName));
		}
		else
		{
			groundPos = GetPosition();
		}
		
		SetPosition(groundPos);
		PlaceOnSurface();
	}
	
	Battery9V GetBattery()
	{
		Battery9V battery = Battery9V.Cast(GetInventory().FindAttachment(Battery9V.SLOT_ID));
		return battery;
	}
	
	bool CanBeLocked()
	{
		return true;
	}
	
	bool CanBeUnlocked()
	{
		return true;
	}
	
	bool CanSetCode()
	{
		return true;
	}
	
	bool HasBattery()
	{
		return GetBattery() != null;
	}
	
	bool NeedBattery()
	{
		return true;
	}
	
	bool HasEnergy()
	{
		return GetCompEM().IsWorking();
	}
	
	bool HasPIN() 
	{
		return m_CodeLockComponent.HasPIN();
	}
		
	string GetInputPIN()
	{
		return m_CodeLockComponent.GetInputPIN();
	}
	
	string GetLockPIN()
	{
		return m_CodeLockComponent.GetLockPIN();
	}
		
	void ResetPIN()
	{
		m_CodeLockComponent.ResetPIN();
	}
	
	void SetLockState(bool state)
	{
		m_CodeLockComponent.SetLockState(state);
	}
	
	CodeLockComponent GetCodeLockComponent()
	{
		return m_CodeLockComponent;
	}
		
	bool IsLocked()
	{
		return m_CodeLockComponent.IsLocked();
	}
		
	CodeLockStage GetStage()
	{
		return m_CodeLockComponent.GetStage();
	}
	
	bool HasBruteForceProtection()
	{
		return m_CodeLockComponent.HasBruteForceProtection();
	}
	
	bool ShouldHandleBruteForce()
	{
		return m_CodeLockComponent.ShouldHandleBruteForce();
	}
	
	int GetWrongCodeCount()
	{
		return m_CodeLockComponent.GetWrongCodeCount();
	}
	
	int GetMinLockPINLength()
	{
		return m_CodeLockComponent.GetMinLockPINLength();
	}
	
	bool GetReversedState()
	{
		return m_Reversed;
	}
	
	//! Checks whether the given player is standing on the physical (non-keypad) side of the lock
	//! Fails closed when the required memory points are not available
	bool IsPlayerOnBackside(PlayerBase player)
	{
		if (!player)
			return false;
		
		if (!MemoryPointExists(MEMPOINT_CENTER) || !MemoryPointExists(MEMPOINT_INSIDE))
			return false;
		
		bool playerOnInsideSide = MiscGameplayFunctions.MemPointDirectionalCheck(player, this, MEMPOINT_CENTER, MEMPOINT_INSIDE);
		
		return playerOnInsideSide != GetReversedState();
	}
	
	bool HasValidParent()
	{
		EntityAI hierarchyParent = GetHierarchyRoot();
		if (hierarchyParent.CanUseConstruction())
			return true;
		
		if (BroadcastedBunker.Cast(hierarchyParent))
			return true;
		
		return false;
	}
	
	CodeLockVisualManager GetVisualManager()
	{
		return m_VisualManager;
	}
	
	float GetBatteryEnergy()
	{
		Battery9V battery = GetBattery();
		if (!battery)
			return 0.0;
			
		float energy = battery.GetCompEM().GetEnergy();
		float energyMax = battery.GetCompEM().GetEnergyMax();
		
		if (energyMax <= 0)
			return 0.0;
			
		return (energy / energyMax) * 100.0;
	}
		
	void UpdateVisuals()
	{		
		if (m_VisualManager)
			m_VisualManager.UpdateVisuals();
	}
	
	void ForceFullVisualsUpdate()
	{
		if (m_VisualManager)
		{
			m_VisualManager.MarkDirty();
			m_VisualManager.UpdateVisuals();
		}
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttachToConstruction);
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type,ParamsReadContext ctx) 
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		m_CodeLockComponent.OnRPC(sender, rpc_type, ctx);
	}
		
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		m_CodeLockComponent.OnStoreSave(ctx);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		if (!m_CodeLockComponent.OnStoreLoad(ctx, version))
			return false;

		return true;
	}
	
	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
		
		if (HasBattery())
		{
			if (HasPIN())
			{
				m_CodeLockComponent.SetStage(CodeLockStage.NORMAL);
			}
			else
			{
				m_CodeLockComponent.SetStage(CodeLockStage.SETTING);
			}
		}
		
		ForceFullVisualsUpdate();
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		ForceFullVisualsUpdate();
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (m_CodeLockComponent)
		{
			CodeLockUIAction uiAction = m_CodeLockComponent.GetUIAction();
			if (uiAction != CodeLockUIAction.NONE)
			{
				m_CodeLockComponent.CheckUIActionClient();
			}
		}
	}
		
	override bool IsExternalLockType()
	{
		return true;
	}
	
	override void InitItemSounds()
	{
		super.InitItemSounds();
		
		GetItemSoundHandler().AddSound(SoundConstants.ITEM_DIGITAL_LOCK_ACCESS, CODELOCK_ACCESS_SOUND);
		GetItemSoundHandler().AddSound(SoundConstants.ITEM_DIGITAL_LOCK_LOCK, CODELOCK_LOCK_SOUND);
		GetItemSoundHandler().AddSound(SoundConstants.ITEM_DIGITAL_LOCK_UNLOCK, CODELOCK_UNLOCK_SOUND);
		GetItemSoundHandler().AddSound(SoundConstants.ITEM_DIGITAL_LOCK_ERROR, CODELOCK_ERROR_SOUND);
		GetItemSoundHandler().AddSound(SoundConstants.ITEM_DIGITAL_LOCK_CONFIRM_INPUT, CODELOCK_CONFIRM_SOUND);
		GetItemSoundHandler().AddSound(SoundConstants.ITEM_DIGITAL_LOCK_CLEAR_INPUT, CODELOCK_CLEAR_SOUND);
		GetItemSoundHandler().AddSound(SoundConstants.ITEM_DIGITAL_LOCK_DIGIT_INPUT, CODELOCK_DIGITS_SOUND);
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GetInventory().CreateInInventory("Battery9V");
	}
}

class DigitalCodeLock_Bunker : DigitalCodeLock
{
	override void Init()
	{
		super.Init();

		BunkerBroadcastConfigInstance bunkerConfig = CfgBunkerBroadcastHandler.GetData();
		int pinCodeLength = CodeLockComponent.MIN_LOCK_PIN_LENGTH;
		
		if (bunkerConfig && bunkerConfig.bunkerSettings)
		{
			//! Make sure we are not overflow max. possible PIN code lenght
			pinCodeLength = bunkerConfig.bunkerSettings.lockCodeLength;
			if (pinCodeLength > CodeLockComponent.MAX_LOCK_PIN_LENGTH)
				pinCodeLength = CodeLockComponent.MAX_LOCK_PIN_LENGTH;
		}
		
		m_CodeLockComponent.SetMinimalPinCodeLength(pinCodeLength);
	}
	
	override bool CanBeLocked()
	{
		return false;
	}
	
	override bool CanSetCode()
	{
		return false;
	}
	
	override bool CanDetachAttachment(EntityAI parent)
	{
		return false;
	}
	
	//! overrides the base behavior - intentional
	override void OnWasDetached(EntityAI parent, int slot_id) {}
	
	override bool NeedBattery()
	{
		return false;
	}
	
	override bool HasBattery()
	{
		return true;
	}
	
	override bool HasEnergy()
	{
		return true;
	}
	
	override bool ShowZonesHealth()
	{
		return true;
	}
	
	bool SetLockCode(string pin)
	{
		return m_CodeLockComponent.SetLockCode(pin);
	}
	
	void Disable()
	{
		m_CodeLockComponent.SetStage(CodeLockStage.NONE);
		SetAnimationPhase(CodeLockVisualManager.CODELOCK_ANIMATED_GREEN_LIGHT, 1.0);
		SetAnimationPhase(CodeLockVisualManager.CODELOCK_ANIMATED_RED_LIGHT, 1.0);
	}
}
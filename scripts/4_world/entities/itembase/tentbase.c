class TentBase extends ItemBase
{
	const int OPENING_0 = 1;
	const int OPENING_1 = 2;
	const int OPENING_2 = 4;
	const int OPENING_3 = 8;
	const int OPENING_4 = 16;
	const int OPENING_5 = 32;
	const int OPENING_6 = 64;
	const int OPENING_7 = 128;
	const int OPENING_8 = 256;
	const int OPENING_9 = 512;
	const int OPENING_10 = 1024;
	const int OPENING_11 = 2048;
	const int OPENING_12 = 4096;
	const int OPENING_13 = 8192;
	const int OPENING_14 = 16384;
	const int OPENING_15 = 32768;
	
	static const int PACKED 	= 0;
	static const int PITCHED 	= 1;
	const float MAX_PLACEMENT_HEIGHT_DIFF = 1.5;
	
	protected int m_State;
	protected int m_StateLocal = -1;
	protected bool m_IsEntrance;
	protected bool m_IsWindow;
	protected bool m_IsToggle;
	protected bool m_IsBeingPacked = false;
	protected int m_OpeningMask = 0;
	protected int m_OpeningMaskLocal = -1;
	
	protected ref map< ref ToggleAnimations, bool> m_ToggleAnimations;
	protected ref array<string> m_ShowAnimationsWhenPitched;
	protected ref array<string> m_ShowAnimationsWhenPacked;
	protected Object			m_ClutterCutter;
	protected CamoNet 			m_CamoNet;
	protected vector m_HalfExtents; // The Y value contains a heightoffset and not the halfextent !!!
	
	void TentBase()
	{
		m_ToggleAnimations = new map<ref ToggleAnimations, bool>;
		m_ShowAnimationsWhenPitched = new array<string>;
		m_ShowAnimationsWhenPacked = new array<string>;

		m_HalfExtents = vector.Zero;
		RegisterNetSyncVariableInt("m_State");
		RegisterNetSyncVariableBool("m_IsEntrance");
		RegisterNetSyncVariableBool("m_IsWindow");	
		RegisterNetSyncVariableBool("m_IsToggle");
		RegisterNetSyncVariableInt("m_OpeningMask");
		RegisterNetSyncVariableBool("m_IsBeingPacked");
		
		ProcessInvulnerabilityCheck(GetInvulnerabilityTypeString());
	}
	
	void ~TentBase()
	{
		if (g_Game)
		{
			DestroyClutterCutter();
		}
	}
	
	override string GetInvulnerabilityTypeString()
	{
		return "disableContainerDamage";
	}
	
	override bool HasProxyParts()
	{
		return true;
	}
	
	//! prevents showing cargo when outside the tent geometry
	override bool CanProxyObstructSelf()
	{
		return true;
	}
	
	override bool IsItemTent()
	{
		return true;
	}
	
	override bool CanBeRepairedByCrafting()
	{
		return false;
	}
	
	override bool CanDisplayCargo()
	{
		if(m_State == PACKED)
			return false;
		return true;
	}
	
	override bool CanDisplayAttachmentCategory(string category_name)
	{
		if(m_State == PACKED)
			return false;
		return true;
	}
	
	override bool CanDisplayAttachmentSlot(int slot_id)
	{
		if(m_State == PACKED)
			return false;
		return true;
	}
	
	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{   
		super.OnStoreSave(ctx);
		
		ctx.Write(m_State);
		ctx.Write(m_OpeningMask);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		ctx.Read(m_State);
		if (version >= 110)
		{
			if (!ctx.Read(m_OpeningMask))
				Print("ERROR: no opening mask found! Default openinng settings initialized.");
		}
		
		if (m_State == PITCHED)
		{
			TryPitch(true);
						
			if (g_Game.IsServer())
			{
				if (!m_ClutterCutter && HasClutterCutter())
				{		
					m_ClutterCutter = g_Game.CreateObjectEx(GetClutterCutter(), GetPosition(), ECE_PLACE_ON_SURFACE);
					m_ClutterCutter.SetOrientation(GetOrientation());
				}
				
				RefreshAttachements();
			}
		}
		else
		{
			Pack(true);
		}
		
		return true;
	}
	
	override void RefreshPhysics()
	{
		super.RefreshPhysics();
		
		if (m_State == PITCHED)
		{
			TryPitch(false, true);
		}
		else
		{
			Pack(false, true);
		}
	}
	
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);
		
		if (new_owner || old_owner)
		{
			if (GetInventory().CountInventory() == 1) // refuse to pack tent with anything inside
				Pack(false);
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
						
		if (m_State != m_StateLocal)
		{
			if (m_State == PACKED)
				Pack(false);
			else
				TryPitch(false);
			
			m_StateLocal = m_State;
		}
		
		if ((m_OpeningMaskLocal != m_OpeningMask)) //opening synchronization for physics recalculation
		{
			HandleOpeningsPhysics();
			m_OpeningMaskLocal = m_OpeningMask;
			
			if (m_State == PACKED)
			{
				Pack(false); //intentional
			}
		}
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		if (m_FixDamageSystemInit)
			return;
		
		if (zone == "" && GetState() == PITCHED && newLevel == GameConstants.STATE_RUINED && g_Game.IsServer())
			MiscGameplayFunctions.DropAllItemsInInventoryInBounds(this, m_HalfExtents);
		
		if (zone != "Body" && zone != "Inventory" && zone != "")
		{
			if (newLevel == GameConstants.STATE_RUINED)
			{
				array<string> selections = new array<string>;
				DamageSystem.GetComponentNamesFromDamageZone(this,zone,selections);
				for (int j = 0; j < selections.Count(); j++)
				{
					if (selections.Get(j) != "")
					{
						RemoveProxyPhysics(selections.Get(j)); //To keep
						//HideSelection(selections.Get(j)); //To change
						AnimateCamonetByOpeningSelection(selections.Get(j)); //To keep
					}
				}
			}
			else if (oldLevel == GameConstants.STATE_RUINED)
			{
				if (GetState() == PITCHED)
				{
					TryPitch(true);
				}
			}
		}
	}
	
	void HideAllAnimationsAndProxyPhysics(bool hide_animations = true, bool hide_physics = true)
	{
		string cfg_path = "cfgVehicles " + GetType() + " AnimationSources";
		
		if (g_Game.ConfigIsExisting(cfg_path))
		{
			int	selections = g_Game.ConfigGetChildrenCount(cfg_path);
			string proxy_selection_name;
			
			for (int i = 0; i < selections; i++)
			{
				string selection_name;
				g_Game.ConfigGetChildName(cfg_path, i, selection_name);
				if (hide_animations)
				{
					SetAnimationPhase(selection_name, 1);
				}
				
				proxy_selection_name = selection_name;
				proxy_selection_name.ToLower();	
				if (hide_physics)
				{
					RemoveProxyPhysics(proxy_selection_name);
				}
			}
		}
	}
	
	bool ConditionIntoInventory(EntityAI player)
	{
		return CanBeManipulated();
	}

	override bool IsTakeable()
	{
		if (!super.IsTakeable())	
			return false;
		
		return CanBeManipulated();
	}
	
	override bool CanPutIntoHands(EntityAI parent)
	{
		if (!super.CanPutIntoHands(parent))
		{		
			return false;
		}
				
		return CanBeManipulated();
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
		{			
			return false;
		}
				
		return CanBeManipulated();
	}
	
	override bool CanRemoveFromCargo(EntityAI parent)
	{
		if (!super.CanRemoveFromCargo(parent))		
			return false;
				
		return CanBeManipulated();
	}
	
	override bool CanRemoveFromHands(EntityAI parent)
	{
		if (!super.CanRemoveFromHands(parent))
			return false;
				
		return CanBeManipulated();
	}
	
	bool ConditionOutOfHands(EntityAI player)
	{
		return CanBeManipulated();
	}
	
	override bool CanBeRepairedToPristine()
	{
		return true;
	}
	
	void RefreshAttachements()
	{
		int slot_id_camo;
		int slot_id_xlights;
		EntityAI eai_xlights;

		slot_id_camo = InventorySlots.GetSlotIdFromString("CamoNet");
		m_CamoNet = CamoNet.Cast(GetInventory().FindAttachment(slot_id_camo));
		
		slot_id_xlights = InventorySlots.GetSlotIdFromString("Lights");
		eai_xlights = GetInventory().FindAttachment(slot_id_xlights);
		
		if (m_CamoNet)
		{
			HandleCamoNetAttachment(false);
			//SetAnimationPhase("Camonet", 0);
			
			if (!IsKindOf("MediumTent"))
			{
				AddProxyPhysics("camonet");
			}	
		}
		
		if (eai_xlights)
		{
			SetAnimationPhase("Xlights", 0);
			SetAnimationPhase("Xlights_glass_r", 0);
			SetAnimationPhase("Xlights_glass_g", 0);
			SetAnimationPhase("Xlights_glass_b", 0);
			SetAnimationPhase("Xlights_glass_y", 0);
		}
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		if (item.IsKindOf ("CamoNet")) 
		{
			m_CamoNet = CamoNet.Cast(item);
			HandleCamoNetAttachment(false);
			//SetAnimationPhase("Camonet", 0);
			
			if (!IsKindOf ("MediumTent"))
			{
				AddProxyPhysics("camonet");
			}		
		}
		
		if (item.IsKindOf ("XmasLights")) 
		{
			SetAnimationPhase("Xlights", 0);
			SetAnimationPhase("Xlights_glass_r", 0);
			SetAnimationPhase("Xlights_glass_g", 0);
			SetAnimationPhase("Xlights_glass_b", 0);
			SetAnimationPhase("Xlights_glass_y", 0);
			
			XmasLights xlights = XmasLights.Cast(item);
			xlights.AttachToObject(this);
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
				
		if (item.IsKindOf ("CamoNet")) 
		{
			m_CamoNet = null;
			HandleCamoNetAttachment(true);
			//SetAnimationPhase("Camonet", 1);
			
			if (!IsKindOf ("MediumTent"))
			{
				RemoveProxyPhysics("camonet");
			}
		}
		
		if (item.IsKindOf ("XmasLights")) 
		{
			SetAnimationPhase("Xlights", 1);
			SetAnimationPhase("Xlights_glass_r", 1);
			SetAnimationPhase("Xlights_glass_g", 1);
			SetAnimationPhase("Xlights_glass_b", 1);
			SetAnimationPhase("Xlights_glass_y", 1);
			
			XmasLights xlights = XmasLights.Cast(item);
			xlights.DetachFromObject(this);	
		}
	}
		
	override int GetViewIndex()
	{
		if (MemoryPointExists("invView2"))
		{		
			InventoryLocation il = new InventoryLocation();
			GetInventory().GetCurrentInventoryLocation(il);
			InventoryLocationType type = il.GetType();
			
			if (GetState() == PACKED)
			{
				switch (type)
				{
					case InventoryLocationType.ATTACHMENT:
						return 1;
					default:
						return 0;
				}
			}
			else
			{
				switch (type)
				{
					case InventoryLocationType.ATTACHMENT:
					case InventoryLocationType.GROUND:
						return 1;
					default:
						return 0;
				}
			}	
		}

		return 0;
	}

	int GetState()
	{
		return m_State;
	}
	
	int GetStateLocal()
	{
		return m_StateLocal;
	}

	bool CanBePacked()
	{
		if (GetState() == PITCHED)
		{
			if (GetInventory().GetCargo().GetItemCount() == 0 && GetInventory().AttachmentCount() == 0)
			{
				return true;
			}
		}
		
		return false;
	}

	bool CanBeManipulated()
	{
		if (GetState() == PACKED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	bool CanAttach(ItemBase item)
	{
		if (item.IsKindOf ("CamoNet") && GetState() == PITCHED) 
		{
			return true;
		}
		
		return false;
	}
	
	override bool IsIgnoredByConstruction()
	{
		return false;
	}
	
	void Pack(bool update_navmesh, bool init = false)
	{
		HideAllAnimationsAndProxyPhysics();
		
		m_State = PACKED;
		m_IsEntrance = PACKED;
		m_IsWindow = PACKED;
		m_IsToggle = PACKED;
		
		Refresh();
				
		GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
		
		if (update_navmesh) 
		{
			RegenerateNavmesh();	
		}
						
		DestroyClutterCutter();
		
		if (g_Game.IsServer() && !IsHologram())
			MiscGameplayFunctions.DropAllItemsInInventoryInBounds(this, m_HalfExtents);
		
		SetSynchDirty();
		
		if ((!g_Game.IsDedicatedServer()) && !init)
		{
			GetOnViewIndexChanged().Invoke();
		}
	}

	void Pitch(bool update_navmesh, bool init = false)
	{
		HideAllAnimationsAndProxyPhysics();
		
		m_State = PITCHED;
		m_IsEntrance = PITCHED;
		m_IsWindow = PITCHED;
		m_IsToggle = PITCHED;
		
		Refresh();
		
		GetInventory().UnlockInventory(HIDE_INV_FROM_SCRIPT);
		
		if (update_navmesh) 
		{
			RegenerateNavmesh();
		}

		SetSynchDirty();
		
		if ((!g_Game.IsDedicatedServer()) && !init)
		{
			GetOnViewIndexChanged().Invoke();
		}
	}
	
	protected void TryPitch(bool update_navmesh, bool init = false)
	{
		if (GetHierarchyRootPlayer())
		{
			if (g_Game.IsDedicatedServer())
			{
				Pack(update_navmesh,init);
			}
			
			return;
		}
		
		Pitch(update_navmesh,init);
	}
	
	void UpdateVisuals()
	{
		string proxy_selection_name;
		string animation_name;
		
		if (GetState() == PITCHED)
		{
			for (int i = 0; i < m_ShowAnimationsWhenPitched.Count(); i++)
			{
				animation_name = m_ShowAnimationsWhenPitched.Get(i);
				
				SetAnimationPhase(animation_name, 0);
			}
			
			HandleOpeningsVisuals();
		}
		else
		{
			for (int j = 0; j < m_ShowAnimationsWhenPacked.Count(); j++)
			{
				animation_name = m_ShowAnimationsWhenPacked.Get(j);
				
				SetAnimationPhase(animation_name, 0);
			}
		}
	}
	
	void UpdatePhysics()
	{
		string proxy_selection_name;
		string animation_name;
		
		if (GetState() == PITCHED)
		{	
			for (int i = 0; i < m_ShowAnimationsWhenPitched.Count(); i++)
			{
				animation_name = m_ShowAnimationsWhenPitched.Get(i);
				
				proxy_selection_name = animation_name;
				proxy_selection_name.ToLower();
				AddProxyPhysics(proxy_selection_name);
			}
			
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(HandleOpeningsPhysics);
		}
		else
		{
			for (int j = 0; j < m_ShowAnimationsWhenPacked.Count(); j++)
			{
				animation_name = m_ShowAnimationsWhenPacked.Get(j);
				
				proxy_selection_name = animation_name;
				proxy_selection_name.ToLower();
				AddProxyPhysics(proxy_selection_name);
			}
		}
	}
	
	//refresh visual/physics state
	void Refresh()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(UpdateVisuals);
		UpdatePhysics();
	}
	
	bool CanToggleAnimations(string selection)
	{
		for (int i = 0; i < m_ToggleAnimations.Count(); i++)
		{
			ToggleAnimations toggle = m_ToggleAnimations.GetKey(i);
			string toggle_off = toggle.GetToggleOff();
			toggle_off.ToLower();
			string toggle_on = toggle.GetToggleOn();
			toggle_on.ToLower();
			
			if (toggle_off == selection || toggle_on == selection)
			{
				string zone;
				DamageSystem.GetDamageZoneFromComponentName(this,selection,zone);
				return GetHealthLevel(zone) < GameConstants.STATE_RUINED;
			}
		}
		
		return false;
	}

	void ResetToggle()
	{
		m_IsEntrance = false;
		m_IsWindow = false;
		m_IsToggle = false;
	}
	
	void ManipulateEntrance()
	{
		m_IsEntrance = true;
	}
	
	void ManipulateWindow()
	{
		m_IsWindow = true;
	}
	
	bool IsManipulatedEntrance()
	{
		return m_IsEntrance;
	}
	
	bool IsManipulatedWindow()
	{
		return m_IsWindow;
	}
	
	//used by user action
	void ToggleAnimation(string selection)
	{
		if (m_State == PACKED)
		{
			return;
		}
		
		bool is_closed;
		ResetToggle();
		
		for (int i = 0; i < m_ToggleAnimations.Count(); i++)
		{
			ToggleAnimations toggle = m_ToggleAnimations.GetKey(i);
			
			string toggle_off = toggle.GetToggleOff();
			toggle_off.ToLower();
			string toggle_on = toggle.GetToggleOn();
			toggle_on.ToLower();
			
			if (toggle_off == selection || toggle_on == selection)
			{
				is_closed = m_OpeningMask & toggle.GetOpeningBit();
				if (is_closed)
				{
					SetAnimationPhase(toggle.GetToggleOff(), 0);
					AddProxyPhysics(toggle.GetToggleOff());
					SetAnimationPhase(toggle.GetToggleOn(), 1);
					RemoveProxyPhysics(toggle.GetToggleOn());
					m_ToggleAnimations.Set(toggle, false);
					m_IsToggle = true;
					m_OpeningMask &= ~toggle.GetOpeningBit();
					
					if (selection.Contains("window"))
					{
						ManipulateWindow();
						if (GetSoundOpenWindow() != string.Empty)
							StartItemSoundServer(SoundConstants.ITEM_TENT_WINDOW_OPEN);
					}
					
					if (selection.Contains("entrance") || selection.Contains("door"))
					{
						ManipulateEntrance();
						if (GetSoundOpen() != string.Empty)
							StartItemSoundServer(SoundConstants.ITEM_TENT_OPEN);
					}
					
					AnimateCamonetToggle(toggle);
				}
				else
				{
					SetAnimationPhase(toggle.GetToggleOff(), 1);
					RemoveProxyPhysics(toggle.GetToggleOff());
					SetAnimationPhase(toggle.GetToggleOn(), 0);
					AddProxyPhysics(toggle.GetToggleOn());
					m_ToggleAnimations.Set(toggle, true);
					m_IsToggle = false;
					m_OpeningMask |= toggle.GetOpeningBit();
					
					if (selection.Contains("window"))
					{
						ManipulateWindow();
						if (GetSoundCloseWindow() != string.Empty)
							StartItemSoundServer(SoundConstants.ITEM_TENT_WINDOW_CLOSE);
					}
					
					if (selection.Contains("entrance") || selection.Contains("door"))
					{
						ManipulateEntrance();
						if (GetSoundClose() != string.Empty)
							StartItemSoundServer(SoundConstants.ITEM_TENT_CLOSE);
					}
					
					AnimateCamonetToggle(toggle);
				}
			}
		}
		SetSynchDirty();
	}
	
	void HandleCamoNetAttachment(bool hide)
	{
		SetAnimationPhase("CamoNet", hide);
	}
	
	void AnimateCamonetToggle(ToggleAnimations toggle) {};
	
	void AnimateCamonetByOpeningSelection(string opening_selection) {};
	
	string GetSoundOpen() {};
	
	string GetSoundClose() {};
	
	string GetSoundOpenWindow() {};
	
	string GetSoundCloseWindow() {};
	
	void RegenerateNavmesh()
	{
		SetAffectPathgraph(true, false);
		
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(g_Game.UpdatePathgraphRegionByObject, 100, false, this);
	}

	bool HasClutterCutter() {};	
	string GetClutterCutter() {};
	
	void DestroyClutterCutter()
	{
		if (g_Game.IsMultiplayer() || g_Game.IsServer())
		{
			if (m_ClutterCutter)
			{
				g_Game.ObjectDelete(m_ClutterCutter);
			}
		}
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
		
	override bool IsDeployable()
	{
		return true;
	}
	
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		
		if (g_Game.IsServer())
		{
			TryPitch(true);
		}
	}
		
	override void InitItemSounds()
	{
		super.InitItemSounds();
		
		ItemSoundHandler handler = GetItemSoundHandler();
		
		if (GetSoundOpen() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_TENT_OPEN, GetSoundOpen());
	
		if (GetSoundClose() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_TENT_CLOSE, GetSoundClose());
		
		if (GetSoundOpenWindow() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_TENT_WINDOW_OPEN, GetSoundOpenWindow());
	
		if (GetSoundCloseWindow() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_TENT_WINDOW_CLOSE, GetSoundCloseWindow());
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionToggleTentOpen);
		AddAction(ActionPackTent);
		AddAction(ActionDeployObject);
	}
	
	void HandleOpeningsVisuals()
	{
		bool is_closed;
		bool is_ruined;
		string zone;
		string component;
		ToggleAnimations toggle;
		
		for (int i = 0; i < m_ToggleAnimations.Count(); i++)
		{
			toggle = m_ToggleAnimations.GetKey(i);
			is_closed = m_OpeningMask & toggle.GetOpeningBit();
			component = toggle.GetToggleOff(); //either one works
			component.ToLower();
			DamageSystem.GetDamageZoneFromComponentName(this,component,zone);
			//is_ruined = (GetHealthLevel(zone) == GameConstants.STATE_RUINED);
			
			if (is_closed)
			{
				SetAnimationPhase(toggle.GetToggleOff(),1);
				SetAnimationPhase(toggle.GetToggleOn(),is_ruined);
				m_ToggleAnimations.Set(toggle, false);
			}
			else
			{
				SetAnimationPhase(toggle.GetToggleOn(),1);
				SetAnimationPhase(toggle.GetToggleOff(),is_ruined);
				m_ToggleAnimations.Set(toggle, true);
			}
			//AnimateCamonetToggle(toggle);
		}
	}
	
	void HandleOpeningsPhysics()
	{
		bool is_closed;
		bool is_ruined;
		int hplevel;
		string zone;
		string component;
		ToggleAnimations toggle;
		
		for (int i = 0; i < m_ToggleAnimations.Count(); i++)
		{
			toggle = m_ToggleAnimations.GetKey(i);
			is_closed = m_OpeningMask & toggle.GetOpeningBit();
			component = toggle.GetToggleOff(); //either one works
			component.ToLower();
			DamageSystem.GetDamageZoneFromComponentName(this,component,zone);
			is_ruined = (GetHealthLevel(zone) == GameConstants.STATE_RUINED);
			
			//re-adding physics to avoid proxy physics stacking
			RemoveProxyPhysics(toggle.GetToggleOff());
			RemoveProxyPhysics(toggle.GetToggleOn());
			
			if (!is_ruined && GetState() == PITCHED)
			{
				if (is_closed)
				{
					AddProxyPhysics(toggle.GetToggleOn());
				}
				else
				{
					AddProxyPhysics(toggle.GetToggleOff());
				}
			}
		}
	}
	
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}

	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if (GetHealthLevel() == GameConstants.STATE_RUINED || m_IsBeingPacked)
			return false;
		
		return super.CanReceiveItemIntoCargo(item);
	}
	
	override bool CanLoadItemIntoCargo(EntityAI item)
	{
		if (!super.CanLoadItemIntoCargo(item))
			return false;
		if (GetHealthLevel() == GameConstants.STATE_RUINED)
			return false;
		
		return true;
	}
	
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if (GetHealthLevel() == GameConstants.STATE_RUINED)
			return false;
		
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
	override bool CanLoadAttachment(EntityAI attachment)
	{
		if (GetHealthLevel() == GameConstants.STATE_RUINED)
			return false;
		
		return super.CanLoadAttachment(attachment);
	}
	
	override bool CanBePlaced(Man player, vector position)
	{
		vector playerpos = player.GetPosition();
		float delta1 = playerpos[1] - position[1];
		
		if (delta1 > MAX_PLACEMENT_HEIGHT_DIFF || delta1 < -MAX_PLACEMENT_HEIGHT_DIFF)
			return false;
		return true;
	}
	
	void SetIsBeingPacked(bool isBeingPacked)
	{
		m_IsBeingPacked = isBeingPacked;
		SetSynchDirty();
	}
	
	//!DEPRECATED
	protected ref EffectSound 	m_DeployLoopSound; //DEPRECATED in favor of m_DeployLoopSoundEx
	
	void PlayDeployLoopSound(); //!DEPRECATED
	void StopDeployLoopSound(); //!DEPRECATED
	void SoundTentOpenPlay();
	void SoundTentClosePlay();
	void SoundTentOpenWindowPlay();
	void SoundTentCloseWindowPlay();
};

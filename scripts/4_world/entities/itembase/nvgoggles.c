class NVGoggles extends PoweredOptic_Base
{
	bool 		m_IsLowered;
	Clothing 	m_Strap;
	
	void NVGoggles()
	{
		RotateGoggles(true);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionViewOptics);
		AddAction(ActionViewBinoculars);
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		if (GetCompEM().CanWork() && m_IsLowered)
			GetCompEM().SwitchOn();
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		RotateGoggles(true);

		PlayerBase player;
		if (m_Strap && PlayerBase.CastTo(player, GetHierarchyRootPlayer()))
			m_Strap.UpdateNVGStatus(player, false, true);
	}
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);

		RotateGoggles(true);		
		m_Strap = Clothing.Cast(parent);
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);

		RotateGoggles(true);
		SetPlayer(null);
		
		PlayerBase player;
		if (m_Strap == parent && PlayerBase.CastTo(player, parent.GetHierarchyRootPlayer()))
		{
			if (parent && Clothing.Cast(parent))
				Clothing.Cast(parent).UpdateNVGStatus(player, false, true);
		}
		
		m_Strap = null;
	}
	
	override void OnWorkStart()
	{
		super.OnWorkStart();

		PlayerBase player;
		EntityAI headgear;
		EntityAI glasses;
		if (m_Strap && PlayerBase.CastTo(player, m_Strap.GetHierarchyParent()))
		{
			headgear = player.FindAttachmentBySlotName("Headgear");
			glasses = player.FindAttachmentBySlotName("Eyewear");
			
			//adjust on load - ComponentEnergyManager stores the 'working' state independently
			if (!m_IsLowered)
				RotateGoggles(false);
			
			if (!GetGame().IsServer() || !GetGame().IsMultiplayer())
			{
				if ((headgear == m_Strap || glasses == m_Strap))
					m_Strap.UpdateNVGStatus(player);
			}
		}
	}
	
	override void OnWorkStop()
	{
		super.OnWorkStop();

		PlayerBase player;
		EntityAI headgear;
		EntityAI glasses;
		if (m_Strap && PlayerBase.CastTo(player, m_Strap.GetHierarchyParent()))
		{
			headgear = player.FindAttachmentBySlotName("Headgear");
			glasses = player.FindAttachmentBySlotName("Eyewear");
			
			if (!GetGame().IsServer() || !GetGame().IsMultiplayer())
			{
				if ((headgear == m_Strap || glasses == m_Strap))
					m_Strap.UpdateNVGStatus(player);
			}
		}
	}
	
	override void OnWork(float consumed_energy)
	{
		//adjust on load - ComponentEnergyManager stores the 'working' state independently
		if (!m_IsLowered)
			RotateGoggles(false);
		
		PlayerBase player;
		EntityAI headgear;
		EntityAI glasses;
		if (m_Strap && PlayerBase.CastTo(player, m_Strap.GetHierarchyParent()))
		{
			headgear = player.FindAttachmentBySlotName("Headgear");
			glasses = player.FindAttachmentBySlotName("Eyewear");
			
			if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) // Client side
			{
				if ((headgear == m_Strap || glasses == m_Strap) && player.IsControlledPlayer())
					player.AddActiveNV(GetCurrentNVType());
			}
		}
	}
	
	void RotateGoggles(bool state)
	{
		if (!m_Strap && !state)	// disable non default rotation while not strapped
			return;
		
		SetAnimationPhase("rotate", !state);
		m_IsLowered = !state;

		PlayerBase player;
		int slotId;
		string slotName;
		if (m_Strap && m_Strap.GetInventory().GetCurrentAttachmentSlotInfo(slotId, slotName) && PlayerBase.CastTo(player, m_Strap.GetHierarchyParent()))
			m_Strap.UpdateNVGStatus(player);
		
		if (GetCompEM())
		{
			if (!state && GetCompEM().CanWork())
				GetCompEM().SwitchOn();
			else
				GetCompEM().SwitchOff();
		}
	}
	
	override int GetCurrentNVType()
	{
		if (IsWorking())
		{
			if (IsUsingOptics2DModel())
				return NVTypes.NV_GOGGLES_2D;
			
			return NVTypes.NV_GOGGLES; //omits the widget occluder
		}
		else
		{
			return NVTypes.NV_GOGGLES_OFF;
		}
	}
	
	override bool IsNVG()
	{
		return true;
	}
	
	//!
	//! DEPRECATED
	//!

	ref Timer m_WorkCheckTimer = new Timer();
	
	//! check for animation state, if another player lowered them first (or solve by synced variable)
	void LoweredCheck()
	{
		if (GetAnimationPhase("rotate") != m_IsLowered)
		{
			m_IsLowered = GetAnimationPhase("rotate");
		}
	}

	void SwitchOnNVGCheck()
	{
		GetCompEM().SwitchOn();
		if (GetCompEM().IsSwitchedOn())
			m_WorkCheckTimer.Stop();
	}
}

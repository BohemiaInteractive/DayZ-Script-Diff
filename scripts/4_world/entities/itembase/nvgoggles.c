class NVGoggles extends PoweredOptic_Base
{
	bool 		m_IsLowered;
	Clothing 	m_Strap;
	ref Timer 	m_WorkCheckTimer;
	
	void NVGoggles()
	{
		RotateGoggles(true);
		m_WorkCheckTimer = new Timer;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionViewOptics);
		AddAction(ActionViewBinoculars);
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached( item, slot_name );
		
		if ( GetCompEM().CanWork() && m_IsLowered )
			m_WorkCheckTimer.Run(0.1,this,"SwitchOnNVGCheck",null,true);
			//GetCompEM().SwitchOn();
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached( item, slot_name );
		
		GetCompEM().SwitchOff();
	}
	
	override void OnWasAttached( EntityAI parent, int slot_id )
	{
		super.OnWasAttached(parent, slot_id);
		RotateGoggles(true);
		
		m_Strap = Clothing.Cast(parent);
		/*if (m_Strap)
			SetPlayer(PlayerBase.Cast(m_Strap.GetHierarchyParent()));*/
	}
	
	override void OnWasDetached( EntityAI parent, int slot_id )
	{
		super.OnWasDetached(parent, slot_id);
		RotateGoggles(true);
		
		PlayerBase player;
		if ( m_Strap == parent && PlayerBase.CastTo(player, parent.GetHierarchyRootPlayer()) )
		{
			if ( parent && Clothing.Cast(parent) )
			{
				Clothing.Cast(parent).UpdateNVGStatus(player,false,true);
			}
		}
		
		m_Strap = null;
		//SetPlayer(null);
	}
	
	override void OnWorkStart()
	{
		PlayerBase player;
		EntityAI headgear;
		EntityAI glasses;
		if ( m_Strap && PlayerBase.CastTo(player, m_Strap.GetHierarchyParent()) )
		{
			headgear = player.FindAttachmentBySlotName("Headgear");
			glasses = player.FindAttachmentBySlotName("Eyewear");
			
			//adjust on load - ComponentEnergyManager stores the 'working' state independently
			if ( !m_IsLowered )
			{
				RotateGoggles(false);
			}
			
			if ( !GetGame().IsServer() || !GetGame().IsMultiplayer() ) // Client side
			{
				if ( (headgear == m_Strap || glasses == m_Strap) && player.IsControlledPlayer() )
				{
					player.AddActiveNV(NVTypes.NV_GOGGLES);
				}
			}
		}
	}
	
	override void OnWorkStop()
	{
		PlayerBase player;
		EntityAI headgear;
		EntityAI glasses;
		if ( m_Strap && PlayerBase.CastTo(player, m_Strap.GetHierarchyParent()) )
		{
			headgear = player.FindAttachmentBySlotName("Headgear");
			glasses = player.FindAttachmentBySlotName("Eyewear");
			
			if ( !GetGame().IsServer() || !GetGame().IsMultiplayer() ) // Client side
			{
				if ( (headgear == m_Strap || glasses == m_Strap) && player.IsControlledPlayer() )
				{
					player.RemoveActiveNV(NVTypes.NV_GOGGLES);
				}
			}
		}
	}
	
	override void OnWork( float consumed_energy )
	{
		//adjust on load - ComponentEnergyManager stores the 'working' state independently
		if ( !m_IsLowered )
		{
			RotateGoggles(false);
		}
		
		PlayerBase player;
		EntityAI headgear;
		EntityAI glasses;
		if ( m_Strap && PlayerBase.CastTo(player, m_Strap.GetHierarchyParent()) )
		{
			headgear = player.FindAttachmentBySlotName("Headgear");
			glasses = player.FindAttachmentBySlotName("Eyewear");
			
			if ( !GetGame().IsServer() || !GetGame().IsMultiplayer() ) // Client side
			{
				if ( (headgear == m_Strap || glasses == m_Strap) && player.IsControlledPlayer() )
				{
					player.AddActiveNV(NVTypes.NV_GOGGLES);
				}
			}
		}
	}
	
	void LoweredCheck() //check for animation state, if another player lowered them first (or solve by synced variable)
	{
		if ( GetAnimationPhase("rotate") != m_IsLowered )
		{
			m_IsLowered = GetAnimationPhase("rotate");
		}
	}
	
	void RotateGoggles(bool state)
	{
		//if ( GetAnimationPhase("rotate") != state ) //useless?
			SetAnimationPhase("rotate",!state);
		m_IsLowered = !state;
		
		PlayerBase player;
		int slot_id;
		string slot_name;
		if ( m_Strap && m_Strap.GetInventory().GetCurrentAttachmentSlotInfo(slot_id,slot_name) && PlayerBase.CastTo(player, m_Strap.GetHierarchyParent())/*&& slot_id == InventorySlots.EYEWEAR*/ )
		{
			player.SetNVGLowered(m_IsLowered);
		}
		
		if ( GetCompEM() )
		{
			if ( !state && GetCompEM().CanWork() )
				GetCompEM().SwitchOn();
			else
				GetCompEM().SwitchOff();
		}
	}
	
	void SwitchOnNVGCheck()
	{
		//Print("SwitchOnNVGCheck");
		GetCompEM().SwitchOn();
		if (GetCompEM().IsSwitchedOn())
		{
			m_WorkCheckTimer.Stop();
		}
	}
	
	override int GetCurrentNVType()
	{
		if (IsWorking())
		{
			//m_CurrentOpticMode
			switch (m_CurrentOpticMode)
			{
				/*case GameConstants.OPTICS_STATE_DAY:
					return NVTypes.NV_GOGGLES_WHATEVER;
				
				case GameConstants.OPTICS_STATE_NIGHTVISION:
					return NVTypes.NV_GOGGLES;*/
				default:
					return NVTypes.NV_GOGGLES;
			}
			Error("Undefined optic mode of " + this);
			return NVTypes.NONE;
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
}

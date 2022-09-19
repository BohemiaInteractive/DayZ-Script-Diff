class UniversalLight extends Switchable_Base
{
	UniversallightLight 	m_Light;
	
	static int			REFLECTOR_ID = 1;
	static int			GLASS_ID = 2;
	
	static string 		LIGHT_OFF_GLASS 	= "dz\\gear\\tools\\data\\flashlight_glass.rvmat";
	static string 		LIGHT_OFF_REFLECTOR = "dz\\weapons\\attachments\\data\\m4_flashlight.rvmat";
	static string 		LIGHT_ON_GLASS 		= "dz\\gear\\tools\\data\\flashlight_glass_on.rvmat";
	static string 		LIGHT_ON_REFLECTOR 	= "dz\\weapons\\attachments\\data\\m4_flashlight_on.rvmat";
	
	ref array<int> 		m_AttachmentSlotsCheck;
	
	void UniversalLight()
	{
		InitAttachmentsSlotsToCheck(m_AttachmentSlotsCheck);
	}
	
	override ScriptedLightBase GetLight()
	{
		return m_Light;
	}
	
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if ( !super.CanPutAsAttachment(parent) ) {return false;}
		
		bool req_attachment 		= false;
		bool rail_attachment_found   = false;
		int slot_id;
		ItemBase attachment;
		for ( int i = 0; i < parent.GetInventory().GetAttachmentSlotsCount(); i++ )
		{
			slot_id = parent.GetInventory().GetAttachmentSlotId(i);
			if ( m_AttachmentSlotsCheck.Find(slot_id) != -1 )
			{
				req_attachment = true;
				attachment = ItemBase.Cast(parent.GetInventory().FindAttachment(slot_id));
				if ( attachment && attachment.ConfigIsExisting("hasRailFunctionality") && attachment.ConfigGetBool("hasRailFunctionality") )
					rail_attachment_found = true;
			}
		}
		return !req_attachment || (req_attachment && rail_attachment_found);
	}
	
	override void OnWorkStart()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // Client side
		{
			m_Light = UniversallightLight.Cast(  ScriptedLightBase.CreateLight(UniversallightLight, "0 0 0", 0.08)  ); // Position is zero because light is attached on parent immediately.
			m_Light.AttachOnMemoryPoint(this, "beamStart", "beamEnd");
			SetObjectMaterial(GLASS_ID, LIGHT_ON_GLASS);
			SetObjectMaterial(REFLECTOR_ID, LIGHT_ON_REFLECTOR);
		}
	}

	override void OnWork( float consumed_energy )
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // Client side
		{
			Battery9V battery = Battery9V.Cast( GetCompEM().GetEnergySource() );
			
			if (battery  &&  m_Light)
			{
				float efficiency = battery.GetEfficiency0To1();
				
				if ( efficiency < 1 )
				{
					m_Light.SetIntensity( efficiency, GetCompEM().GetUpdateInterval() );
				}
				else
				{
					m_Light.SetIntensity( 1, 0 );
				}
			}
		}
	}
	
	override void OnWorkStop()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // Client side
		{
			if (m_Light)
				m_Light.FadeOut();
			
			m_Light = NULL;
			
			SetObjectMaterial(GLASS_ID, LIGHT_OFF_GLASS);
			SetObjectMaterial(REFLECTOR_ID, LIGHT_OFF_REFLECTOR);
		}
	}
	
	// Inventory manipulation
	override void OnInventoryExit(Man player)
	{
		super.OnInventoryExit(player);
		
		if ( GetCompEM().IsWorking() )
		{
			if (player)
			{
				vector ori_rotate = player.GetOrientation();
				ori_rotate = ori_rotate + Vector(270,0,0);
				SetOrientation(ori_rotate);
			}
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
	}
	
	override bool IsLightSource()
	{
		return true;
	}
	
	//! Enter att slot types to check on attach
	void InitAttachmentsSlotsToCheck(out array<int> AttSlots)
	{
		if (!AttSlots)
		{
			AttSlots = new array<int>;
			AttSlots.Insert(InventorySlots.GetSlotIdFromString("weaponHandguardM4"));
			AttSlots.Insert(InventorySlots.GetSlotIdFromString("weaponHandguardAK"));
			AttSlots.Insert(InventorySlots.GetSlotIdFromString("weaponHandguardMP5"));
		}
	}
}
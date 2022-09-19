class Headtorch_ColorBase extends Clothing
{
	HeadtorchLight m_Light;
	
	static int		REFLECTOR_ID = 4;
	static int		GLASS_ID = 5;
	
	static string 	LIGHT_OFF_GLASS 		= "dz\\characters\\headgear\\data\\HeadTorchGlass.rvmat";
	static string 	LIGHT_OFF_REFLECTOR 	= "dz\\characters\\headgear\\data\\HeadTorch.rvmat";
	static string 	LIGHT_ON_GLASS 			= "dz\\characters\\headgear\\data\\HeadTorchGlass_on.rvmat";
	static string 	LIGHT_ON_GLASS_RED		= "dz\\characters\\headgear\\data\\HeadTorchGlass_on_red.rvmat";
	static string 	LIGHT_ON_REFLECTOR 		= "dz\\characters\\headgear\\data\\HeadTorch_ON.rvmat";
	static string 	LIGHT_ON_REFLECTOR_RED	= "dz\\characters\\headgear\\data\\HeadTorch_ON_red.rvmat";
	
	static vector m_OnHeadLocalPos = Vector(0.12,0.15,0);
	static vector m_OnHeadLocalOri = Vector(0,90,0);
	static string m_OffHeadLightPoint = "beamStart";
	static string m_OffHeadLightTarget = "beamEnd";
	
	ref Timer m_Timer;
	
	void Headtorch_ColorBase()
	{
		if (!m_Timer)
			m_Timer = new Timer( CALL_CATEGORY_SYSTEM );
			
		m_Timer.Run( 1 , this, "CheckParent", NULL, false);
	}
	
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing helmet = Clothing.Cast(parent.FindAttachmentBySlotName("Headgear"));
		
		if ( helmet && helmet.ConfigGetBool("noNVStrap") )
		{
			return false;
		}
		
		return true;
	}
	
	void CheckParent()
	{
		EntityAI owner = GetHierarchyParent();
		
		// Spawn a battery in the headtorch if it's attached on a zombie and switch it on
		if (owner  &&  owner.IsZombie())
		{
			GetInventory().CreateAttachment("Battery9V");
			GetCompEM().SwitchOn();
		}
		
		if (m_Timer)
			m_Timer.Stop();
		
		m_Timer = null;
	}
	
	override void OnWorkStart()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // Client side
		{
			CreateHeadtorchLight();
		}
		
		if ( IsInherited( Headtorch_Black ) )
		{
			SetObjectMaterial(GLASS_ID, LIGHT_ON_GLASS_RED);
			SetObjectMaterial(REFLECTOR_ID, LIGHT_ON_REFLECTOR_RED);
		}
		else
		{
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
	
	void OnLightCreated()
	{
		m_Light.SetColorToWhite();
	}
	
	void CreateHeadtorchLight()
	{
		if (!m_Light)
			m_Light = HeadtorchLight.Cast(  ScriptedLightBase.CreateLight( HeadtorchLight, "0 0 0")  );
		
		OnLightCreated();
		
		EntityAI owner = GetHierarchyParent();
		
		if (owner)
		{
			if (owner.IsPlayer())
			{
				PlayerBase owner_PB = PlayerBase.Cast(owner);
				ItemBase att_item = owner_PB.GetItemOnSlot("Eyewear");
				
				if (att_item == this)
				{
					AttachLightOnHead(owner_PB);
				}
				else
				{
					m_Light.AttachOnMemoryPoint(this, m_OffHeadLightPoint, m_OffHeadLightTarget);
				}
			}
			else if (owner.IsZombie())
			{
				int slot_id = InventorySlots.GetSlotIdFromString( "Headgear" );
				EntityAI item_EAI = owner.GetInventory().FindAttachment( slot_id );
				ItemBase item_IB = ItemBase.Cast(item_EAI);
				
				if (item_IB == this)
				{
					AttachLightOnHead(owner);
				}
			}
		}
		else
		{
			m_Light.AttachOnMemoryPoint(this, m_OffHeadLightPoint, m_OffHeadLightTarget);
		}
	}
	
	void AttachLightOnHead(EntityAI person)
	{
		if (person.IsPlayer())
		{
			PlayerBase player = PlayerBase.Cast(person);
			int boneIdx = player.GetBoneIndexByName("Head");
			
			if( boneIdx != -1 )
			{
				if ( m_Light.GetParent() )
					m_Light.DetachFromParent();
				
				m_Light.SetPosition( m_OnHeadLocalPos );
				m_Light.SetOrientation( m_OnHeadLocalOri );
				
				player.AddChild(m_Light, boneIdx);
			}
		}
		else if (person.IsZombie())
		{
			if ( m_Light.GetParent() )
				m_Light.DetachFromParent();
			
			m_Light.SetPosition( m_OnHeadLocalPos );
			m_Light.SetOrientation( m_OnHeadLocalOri );
			
			person.AddChild(m_Light, 17);
		}
	}
	
	override void OnWasAttached( EntityAI parent, int slot_id )
	{
		super.OnWasAttached(parent, slot_id);
		
		PlayerBase player = PlayerBase.Cast(parent);

		if ( player )
		{
			if ( m_Light )
			{
				AttachLightOnHead( player );
			}
			//player.m_PlayerLightManager.AddLightSource(this);
		}
	}
	
	override void OnWasDetached( EntityAI parent, int slot_id )
	{
		super.OnWasDetached(parent, slot_id);
		
		PlayerBase player = PlayerBase.Cast(parent);
		if ( player )
		{
			if ( m_Light )
			{
				m_Light.DetachFromParent();
				m_Light.AttachOnMemoryPoint(this, m_OffHeadLightPoint, m_OffHeadLightTarget);
			}
			//player.m_PlayerLightManager.RemoveLightSource(this);
		}
	}
	
	override void OnWorkStop()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // Client side
		{
			if (m_Light)
				m_Light.FadeOut();
			
			m_Light = NULL;
		}
		
		SetObjectMaterial(GLASS_ID, LIGHT_OFF_GLASS);
		SetObjectMaterial(REFLECTOR_ID, LIGHT_OFF_REFLECTOR);
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
		AddAction(ActionTurnOnHeadtorch);
		AddAction(ActionTurnOffHeadtorch);
	}
	
	override bool IsLightSource()
	{
		return true;
	}
	
	override ItemBase GetLightSourceItem()
	{
		return this;
	}
};
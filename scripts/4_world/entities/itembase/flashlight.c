class Flashlight extends ItemBase
{
	FlashlightLight m_Light;
	
	static int		REFLECTOR_ID = 1;
	static int		GLASS_ID = 2;
	
	static string 	LIGHT_OFF_GLASS = "dz\\gear\\tools\\data\\flashlight_glass.rvmat";
	static string 	LIGHT_OFF_REFLECTOR = "dz\\gear\\tools\\data\\flashlight.rvmat";
	static string 	LIGHT_ON_GLASS = "dz\\gear\\tools\\data\\flashlight_glass_on.rvmat";
	static string 	LIGHT_ON_REFLECTOR = "dz\\gear\\tools\\data\\flashlight_glass_on.rvmat";
	
	override void OnWorkStart()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // Client side
		{
			m_Light = FlashlightLight.Cast(  ScriptedLightBase.CreateLight(FlashlightLight, "0 0 0", 0.08)  ); // Position is zero because light is attached on parent immediately.
			m_Light.AttachOnMemoryPoint(this, "beamStart", "beamEnd");
		}
		
		SetObjectMaterial(GLASS_ID, LIGHT_ON_GLASS);
		SetObjectMaterial(REFLECTOR_ID, LIGHT_ON_REFLECTOR);
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
		}
		
		SetObjectMaterial(GLASS_ID, LIGHT_OFF_GLASS);
		SetObjectMaterial(REFLECTOR_ID, LIGHT_OFF_REFLECTOR);
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
	
	override void OnDebugSpawn()
	{
		Battery9V.Cast(GetInventory().CreateInInventory("Battery9V"));
	}
}
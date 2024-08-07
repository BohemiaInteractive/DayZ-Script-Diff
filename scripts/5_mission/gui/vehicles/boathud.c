// Boat HUD UI
class BoatHud : VehicleHudBase
{	
	protected const float 		ENGINE_BLINK_DURATION = 3;
	
	protected ImageWidget	m_SpeedPointer;
	protected ImageWidget	m_EngineLight;
	protected ImageWidget	m_FuelLight;
		
	protected bool			m_HasEngine;
	protected int 			m_EngagedGear;
	protected int 			m_EngineHealthLevel;
	protected float 		m_FuelLevel;
	protected float			m_TimeSinceEngineHit;
	protected float			m_TimeSinceEngineBlink;
	protected BoatScript	m_CurrentVehicle;
	
	protected ref WidgetFadeTimer	m_FadeTimer = new WidgetFadeTimer();
	
	override void Init(Widget vehicleHudPanels)
	{	
		super.Init(vehicleHudPanels);
		
		m_VehiclePanel = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_hud_boats.layout", vehicleHudPanels);
		
		m_SpeedPointer				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("SpeedPointer") );
		m_EngineLight				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("EngineLight"));
		m_FuelLight					= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("FuelLight"));
		
		m_VehicleCurrentGearValue	= TextWidget.Cast( m_VehiclePanel.FindAnyWidget("Current") );
		m_VehicleNextGearValue		= TextWidget.Cast( m_VehiclePanel.FindAnyWidget("Next") );
		m_VehiclePrevGearValue		= TextWidget.Cast( m_VehiclePanel.FindAnyWidget("Prev") );
	}
	
	override void ShowVehicleInfo(PlayerBase player)
	{
		HumanCommandVehicle hcv = player.GetCommand_Vehicle();
		if (!hcv)
			return;
			
		m_CurrentVehicle = BoatScript.Cast(hcv.GetTransport());
						
		m_VehiclePanel.Show(true);

		m_EngineHealthLevel = -1;
		m_FuelLevel = -1;
		
		if (m_CurrentVehicle.HasEngine())
			m_HasEngine = true;
		
		float gearNeutral = m_CurrentVehicle.GetNeutralGear();
		m_VehicleGearTable.Set(gearNeutral - 2, "");
		m_VehicleGearTable.Set(gearNeutral - 1, "R");
		m_VehicleGearTable.Set(gearNeutral, "N");
		m_VehicleGearTable.Set(gearNeutral + 1, "F");
		m_VehicleGearTable.Set(gearNeutral + 2, "");
		
		m_EngagedGear = -10; // force initial update
	}
	
	override void HideVehicleInfo()
	{
		m_CurrentVehicle = null;
		m_VehicleGearTable.Clear();
	}
	
	override void RefreshVehicleHud(float timeslice)
	{
		int engineHealthLevel		= m_CurrentVehicle.GetHealthLevel("Engine");
		float fuelFraction			= m_CurrentVehicle.GetFluidFraction(BoatFluid.FUEL);
		
		if (m_EngineHealthLevel != engineHealthLevel)
			UpdateEngineIcon(engineHealthLevel);
		
		if (m_FuelLevel != fuelFraction)
			UpdateFuelIcon(fuelFraction);
		
		if (m_CurrentVehicle.HasEngineZoneReceivedHit())
			m_TimeSinceEngineHit = ENGINE_BLINK_DURATION;
		
		if (m_TimeSinceEngineHit > 0)
			UpdateEngineBlink(timeslice);
		
		UpdateSpeedPointer();
		UpdateGear();
	}
	
	protected void UpdateEngineIcon(int level)
	{
		m_EngineHealthLevel = level;
		
		if (m_EngineHealthLevel <= GameConstants.STATE_WORN)
		{
			m_EngineLight.SetColor(COLOR_WHITE);
			m_EngineLight.SetAlpha(0);	
		}
		else if (m_EngineHealthLevel > GameConstants.STATE_WORN)
		{
			m_EngineLight.SetColor(ItemManager.GetItemHealthColor(m_EngineHealthLevel));
			m_EngineLight.SetAlpha(1);
		}
	}
	
	protected void UpdateFuelIcon(float level)
	{
		m_FuelLevel = level;
		
		if (m_FuelLevel > 0.45)
		{	
			m_FuelLight.SetAlpha(0);
			return;
		}
		else if (m_FuelLevel > 0.15)
			m_FuelLight.SetColor(Colors.COLOR_DAMAGED);	
		else if (m_FuelLevel > 0)
			m_FuelLight.SetColor(Colors.COLOR_BADLY_DAMAGED);	
		else 
			m_FuelLight.SetColor(Colors.COLOR_RUINED);	
		
		m_FuelLight.SetAlpha(1);	
	}
		
	protected void UpdateSpeedPointer()
	{
		if (m_HasEngine)
		{
			float min, max, current;
			min = 0;
			max = m_CurrentVehicle.EngineGetRPMMax();
			current = Math.Clamp(m_CurrentVehicle.EngineGetRPM(), min, max); 
			
			float percentVal =  Math.InverseLerp(min, max, current);
			m_SpeedPointer.SetRotation(0, 0, Math.Lerp(225, 495, Math.AbsFloat(percentVal)), true);
		}
	}
	
	protected void UpdateGear()
	{
		if (!m_HasEngine)
			return;
				
		int gear = m_CurrentVehicle.GetGear();
		if (m_EngagedGear == gear)
			return;
		
		m_EngagedGear = gear;
		
		m_VehiclePrevGearValue.SetText(m_VehicleGearTable.Get(m_EngagedGear - 1));
		m_VehicleCurrentGearValue.SetText(m_VehicleGearTable.Get(m_EngagedGear));
		m_VehicleNextGearValue.SetText(m_VehicleGearTable.Get(m_EngagedGear + 1));
	}
	
	protected void UpdateEngineBlink(float timeSlice)
	{
		m_TimeSinceEngineHit -= timeSlice;
		if (m_TimeSinceEngineHit <= 0)
		{
			m_FadeTimer.Stop();
			m_EngineLight.SetAlpha(1);
			return;
		}
		
		m_TimeSinceEngineBlink -= timeSlice;
		if (m_TimeSinceEngineBlink <= 0)
		{
			m_TimeSinceEngineBlink = 0.35;
			if (m_EngineLight.GetAlpha() == 0)
				m_FadeTimer.FadeIn(m_EngineLight, 0.1);
			else 
				m_FadeTimer.FadeOut(m_EngineLight, 0.1);
		}
	}
	
	protected float GetSpeedometer()
	{
		vector transform[4];
		m_CurrentVehicle.GetTransform(transform);
		
		return GetVelocity(m_CurrentVehicle).InvMultiply3(transform)[2];
	}
}
// Car HUD 
class CarHud : VehicleHudBase
{
	protected ImageWidget			m_VehicleRPMPointer;
	protected ImageWidget			m_VehicleRPMDial;
	protected ImageWidget			m_VehicleRPMRedline;
	protected ImageWidget			m_VehicleSpeedPointer;
	protected Widget				m_VehicleTemperatureIndicator;
	protected ImageWidget			m_VehicleTemperaturePointer;
	protected ImageWidget			m_VehicleTemperatureLight;
	protected ImageWidget			m_VehicleFuelPointer;
	protected ImageWidget			m_VehicleFuelLight;
	
	protected TextWidget			m_VehicleSpeedValue;
		
	protected ImageWidget			m_VehicleBatteryLight;
	protected ImageWidget			m_VehicleEngineLight;
	protected ImageWidget			m_VehicleOilLight;
	protected ImageWidget			m_VehicleHandBrakeLight;
	protected ImageWidget			m_VehicleWheelLight;
	
	protected bool 					m_VehicleDamageZoneHitEngineState;
	protected float					m_VehicleDamageZonesHitTimer;
	
	protected CarScript				m_CurrentVehicle;
	
	protected int					m_VehicleGearCount = -1;
	protected float					m_TimeSinceLastEngineLightChange;
	protected bool					m_VehicleHasCoolant;
		
	override void Init(Widget vehicleHudPanels)
	{		
		super.Init(vehicleHudPanels);
		
		m_VehiclePanel = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_hud_cars.layout", vehicleHudPanels);
		
		m_VehicleRPMPointer				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("RPMPointer") );
		m_VehicleRPMDial				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("RPMDial") );
		m_VehicleRPMRedline				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("RPMDialRedline") );
		m_VehicleSpeedPointer			= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("SpeedPointer") );
		m_VehicleSpeedValue				= TextWidget.Cast( m_VehiclePanel.FindAnyWidget("SpeedValue") );
		
		m_VehicleCurrentGearValue		= TextWidget.Cast( m_VehiclePanel.FindAnyWidget("Current") );
		m_VehicleNextGearValue			= TextWidget.Cast( m_VehiclePanel.FindAnyWidget("Next") );
		m_VehiclePrevGearValue			= TextWidget.Cast( m_VehiclePanel.FindAnyWidget("Prev") );
		
		m_VehicleBatteryLight			= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("BatteryLight"));
		m_VehicleEngineLight			= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("EngineLight"));
		m_VehicleOilLight				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("OilLight"));
		m_VehicleHandBrakeLight			= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("HandBrakeLight"));
		m_VehicleWheelLight				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("WheelLight"));
		
		m_VehicleTemperatureIndicator	= m_VehiclePanel.FindAnyWidget("TemperatureIndicator");
		m_VehicleTemperaturePointer		= ImageWidget.Cast(m_VehiclePanel.FindAnyWidget("TemperaturePointer"));
		m_VehicleTemperatureLight		= ImageWidget.Cast(m_VehiclePanel.FindAnyWidget("TemperatureLight"));
		m_VehicleFuelPointer			= ImageWidget.Cast(m_VehiclePanel.FindAnyWidget("FuelPointer"));
		m_VehicleFuelLight				= ImageWidget.Cast(m_VehiclePanel.FindAnyWidget("FuelLight"));
		
		m_VehicleGearTable.Set(-1, "");
		m_VehicleGearTable.Set(CarGear.REVERSE, "R");
		m_VehicleGearTable.Set(CarGear.NEUTRAL, "N");
		m_VehicleGearTable.Set(CarGear.FIRST, "1");
		m_VehicleGearTable.Set(CarGear.SECOND, "2");
		m_VehicleGearTable.Set(CarGear.THIRD, "3");
		m_VehicleGearTable.Set(CarGear.FOURTH, "4");
		m_VehicleGearTable.Set(CarGear.FIFTH, "5");
		m_VehicleGearTable.Set(CarGear.SIXTH, "6");
		m_VehicleGearTable.Set(CarGear.SEVENTH, "7");
		m_VehicleGearTable.Set(CarGear.EIGTH, "8");
		
		m_VehicleGearTableAuto.Set(-1, "");
		m_VehicleGearTableAuto.Set(CarAutomaticGearboxMode.R, "R");
		m_VehicleGearTableAuto.Set(CarAutomaticGearboxMode.N, "N");
		m_VehicleGearTableAuto.Set(CarAutomaticGearboxMode.D, "D");
	}
	
	override void ShowVehicleInfo(PlayerBase player)
	{
		HumanCommandVehicle hcv = player.GetCommand_Vehicle();
		if (!hcv)
			return;
			
		m_CurrentVehicle = CarScript.Cast(hcv.GetTransport());
		if (!m_CurrentVehicle)
			return;
					
		m_VehicleGearCount 	= m_CurrentVehicle.GetGearCount();

		float rpm_value_red = m_CurrentVehicle.EngineGetRPMRedline() / m_CurrentVehicle.EngineGetRPMMax();
		m_VehicleRPMDial.SetMaskProgress(rpm_value_red);
		m_VehicleRPMRedline.SetMaskProgress(1 - rpm_value_red);

		m_VehiclePanel.Show(true);
	}
	
	override void HideVehicleInfo()
	{
		m_CurrentVehicle	= null;
		m_VehicleGearCount	= -1;
	}
	
	override void RefreshVehicleHud(float timeslice)
	{
		float rpm_value		= m_CurrentVehicle.EngineGetRPM() / m_CurrentVehicle.EngineGetRPMMax();
		float rpm_value_red	= m_CurrentVehicle.EngineGetRPMRedline() / m_CurrentVehicle.EngineGetRPMMax();
		float speed_value	= m_CurrentVehicle.GetSpeedometerAbsolute() / 200;
		
		m_VehicleRPMPointer.SetRotation(0, 0, rpm_value * 270 - 130, true);
		m_VehicleSpeedPointer.SetRotation(0, 0, speed_value * 260 - 130, true);
		m_VehicleSpeedValue.SetText(Math.AbsInt(m_CurrentVehicle.GetSpeedometer()).ToString());

		int engineHealthLevel	= m_CurrentVehicle.GetHealthLevel("Engine");
		int fuelTankHealthLevel	= m_CurrentVehicle.GetHealthLevel("FuelTank");
		bool newHealth			= false;
		
		//! engine
		if (m_CurrentVehicle.HasEngineZoneReceivedHit())
		{
			if (m_TimeSinceLastEngineLightChange > 0.35)
			{
				m_VehicleEngineLight.Show(!m_VehicleEngineLight.IsVisible());
				if (engineHealthLevel <= GameConstants.STATE_WORN)
				{
					m_VehicleEngineLight.SetColor(Colors.WHITE);
				}
				else
				{
					m_VehicleEngineLight.SetColor(ItemManager.GetItemHealthColor(engineHealthLevel));
				}
				
				m_VehicleEngineLight.SetAlpha(1);
				m_TimeSinceLastEngineLightChange = 0;
			}
			
			m_TimeSinceLastEngineLightChange += timeslice;
			newHealth = true;
		}
		else if (engineHealthLevel > GameConstants.STATE_WORN)
		{
			m_VehicleEngineLight.SetColor(ItemManager.GetItemHealthColor(engineHealthLevel));
			m_VehicleEngineLight.SetAlpha(1);
			m_VehicleEngineLight.Show(true);
		}
		else
		{
			m_VehicleEngineLight.Show(false);
		}
		
		//! fuel tank
		if (fuelTankHealthLevel <= GameConstants.STATE_WORN)
		{
			m_VehicleFuelLight.SetColor(Colors.WHITE);
			m_VehicleFuelLight.SetAlpha(1);
			m_VehicleFuelLight.Show(true);	
		}
		else if (fuelTankHealthLevel > GameConstants.STATE_WORN)
		{
			m_VehicleFuelLight.SetColor(ItemManager.GetItemHealthColor(fuelTankHealthLevel));
			m_VehicleFuelLight.SetAlpha(1);
			m_VehicleFuelLight.Show(true);
		}
		
		//! temperature
		if (m_CurrentVehicle.IsVitalRadiator())
		{
			m_VehicleTemperatureIndicator.Show(true);
			if (m_CurrentVehicle.HasRadiator())
			{
				int radiatorHealthLevel = m_CurrentVehicle.GetRadiator().GetHealthLevel("");
				if (radiatorHealthLevel <= GameConstants.STATE_WORN)
				{
					m_VehicleTemperatureLight.SetColor(Colors.WHITE);
					m_VehicleTemperatureLight.SetAlpha(1);
					m_VehicleTemperatureLight.Show(true);	
				}
				else if (radiatorHealthLevel > GameConstants.STATE_WORN)
				{
					m_VehicleTemperatureLight.SetColor(ItemManager.GetItemHealthColor(radiatorHealthLevel));
					m_VehicleTemperatureLight.SetAlpha(1);
					m_VehicleTemperatureLight.Show(true);
				}
			}
			else
			{
				m_VehicleTemperatureLight.SetColor(Colors.COLOR_RUINED);
				m_VehicleTemperatureLight.SetAlpha(1);
				m_VehicleTemperatureLight.Show(true);
			}
		}
		else
		{
			m_VehicleTemperatureIndicator.Show(false);
		}
		
		m_VehicleHandBrakeLight.Show(m_CurrentVehicle.IsHandbrakeActive());
		m_VehicleWheelLight.Show(m_CurrentVehicle.WheelIsAnyLocked());

		
		int engagedGear	= -1;
		int prevGear	= -1;
		int nextGear	= -1;
		
		if (m_CurrentVehicle.GearboxGetType() == CarGearboxType.MANUAL)
		{
			engagedGear = m_CurrentVehicle.GetGear();
			prevGear = engagedGear - 1;
			nextGear = engagedGear + 1;

			if (engagedGear == CarGear.NEUTRAL)
			{
				prevGear = CarGear.REVERSE;
			}
			else if (engagedGear == CarGear.REVERSE)
			{
				prevGear = -1;
				nextGear = CarGear.NEUTRAL;
			}

			m_VehicleCurrentGearValue.SetText(m_VehicleGearTable.Get(engagedGear));
			m_VehicleNextGearValue.Show(nextGear < m_VehicleGearCount);
		
			m_VehicleNextGearValue.SetText(m_VehicleGearTable.Get(nextGear));
			m_VehiclePrevGearValue.SetText(m_VehicleGearTable.Get(prevGear));
		}
		else
		{
			engagedGear = m_CurrentVehicle.GearboxGetMode();
			prevGear = engagedGear - 1;
			nextGear = engagedGear + 1;
			
			m_VehicleCurrentGearValue.SetText(m_VehicleGearTableAuto.Get(engagedGear));
			m_VehicleNextGearValue.Show(nextGear < EnumTools.GetEnumSize(CarAutomaticGearboxMode));
		
			m_VehicleNextGearValue.SetText(m_VehicleGearTableAuto.Get(nextGear));
			m_VehiclePrevGearValue.SetText(m_VehicleGearTableAuto.Get(prevGear));
		}
		
		// refresh backlit
		GetDayZGame().GetBacklit().RefreshVehicleLayout(engagedGear, newHealth);
		
		m_VehicleFuelPointer.SetRotation( 0, 0, m_CurrentVehicle.GetFluidFraction( CarFluid.FUEL ) * 260 - 130, true );
		m_VehicleTemperaturePointer.SetRotation( 0, 0, -1 * m_CurrentVehicle.GetFluidFraction( CarFluid.COOLANT ) * 260 + 130, true );

		//! general hit timer reset			
		if (m_VehicleDamageZonesHitTimer > 1)
		{
			m_VehicleDamageZoneHitEngineState = false;
			m_VehicleDamageZonesHitTimer = 0;
		}
		
		m_VehicleDamageZonesHitTimer += timeslice;
	}
}
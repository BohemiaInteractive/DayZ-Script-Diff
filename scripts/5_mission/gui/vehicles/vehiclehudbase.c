// Vehicle HUD UI base class
class VehicleHudBase : Managed
{
	protected Widget 		m_VehiclePanel;
	protected TextWidget	m_VehicleCurrentGearValue;
	protected TextWidget	m_VehicleNextGearValue;
	protected TextWidget	m_VehiclePrevGearValue;
	
	protected ref map<int,string>	m_VehicleGearTable;
	protected ref map<int,string>	m_VehicleGearTableAuto;
	
	void VehicleHudBase();
	
	void ~VehicleHudBase();
	
	// Spawn 
	void Init(Widget vehicleHudPanels)
	{
		m_VehicleGearTable				= new map<int, string>;
		m_VehicleGearTableAuto			= new map<int, string>;
	}
	
	// Logic for when vehicle info is first displayed 
	void ShowVehicleInfo(PlayerBase player);
	
	// Logic for when vehicle info is hidden
	void HideVehicleInfo();
	
	// This is used for continuous hiding of each panel because some unrelated parts of UI force it visible when any menu is closed 
	void HidePanel()
	{
		if (m_VehiclePanel)
			m_VehiclePanel.Show(false);
	}
	
	// Frame update logic
	void RefreshVehicleHud(float timeslice);
	
}
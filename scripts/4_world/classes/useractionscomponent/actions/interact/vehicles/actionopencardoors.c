class ActionOpenCarDoors: ActionCarDoors
{
	void ActionOpenCarDoors()
	{
		m_IsOpening		= true;
		
		FillCommandUIDPerCrewIdx(DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_OPEN, DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOOROPEN);
		m_Text = "#open";
	}
};
class ActionCloseCarDoors: ActionCarDoors
{
	void ActionCloseCarDoors()
	{	
		m_IsOpening 	= false;
		
		FillCommandUIDPerCrewIdx(DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_CLOSE, DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE);
		m_Text = "#close";
	}
};
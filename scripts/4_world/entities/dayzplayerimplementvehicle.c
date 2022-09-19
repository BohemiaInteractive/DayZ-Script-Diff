class GearChangeActionCallback : HumanCommandActionCallback
{	
	// Left for backwards compatibility, but never ever store HumanCommandVehicle in a variable used later
	HumanCommandVehicle m_hcv;
	
	void SetVehicleCommand(HumanCommandVehicle hcv)
	{
		//m_hcv = hcv;
		hcv.SetClutchState(true);
	}
	
	//Command events
	override void OnFinish(bool pCanceled)	
	{
		Human owner = GetHuman();
		if (owner)
		{
			HumanCommandVehicle hcv = owner.GetCommand_Vehicle();
			if (hcv)
			{
				hcv.SetClutchState(false);
			}
		}
	}	
};


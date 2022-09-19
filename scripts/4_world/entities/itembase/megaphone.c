class Megaphone extends ItemMegaphone
{
	override bool IsTransmitter()
	{
		return true;
	}	

	//--- POWER EVENTS	
	override void OnSwitchOn()
	{
		if ( !GetCompEM().CanWork() )
		{
			GetCompEM().SwitchOff();
		}
	}	
	
	override void OnWorkStart()
	{
		//turn device on
		SwitchOn ( true ); // Note: This is not Energy Manager function. This is engine function.
	}
	
	override void OnWorkStop()
	{
		//auto switch off (EM)
		GetCompEM().SwitchOff();
		
		//turn off device
		SwitchOn ( false ); // Note: This is not Energy Manager function. This is engine function.
	}
	
	//TODO add turn on/off actions
	//proto native bool CanSpeak();
	//proto native void SetCanSpeak(bool onOff); 
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnTransmitter);
		AddAction(ActionTurnOffTransmitter);
		AddAction(ActionRaiseMegaphone);
	}	
	
}

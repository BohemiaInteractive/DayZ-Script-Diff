class Refridgerator extends ItemBase
{	
	SoundOnVehicle 	m_SoundLoopEntity;
	
	override bool IsElectricAppliance()
	{
		return true;
	}
	
	override void OnInitEnergy()
	{
		m_SoundLoopEntity = NULL;
	}
	
	//--- POWER EVENTS
	override void OnWorkStart()
	{
		// TO DO: Slow down or stop the aging of food inside of the fridge when such functionality is created.
		
		m_SoundLoopEntity = PlaySoundLoop("powerGeneratorLoop", 50); // using placeholder sound
	}
	
	override void OnWork( float consumed_energy )
	{
		
	}
	
	override void OnWorkStop()
	{
		// TO DO: Allow food inside the fridge to start aging again.
		
		g_Game.ObjectDelete(m_SoundLoopEntity);
		m_SoundLoopEntity = NULL;
	}
	
	//--- ACTION EVENTS
	override void OnSwitchOn()
	{
		
	}

	override void OnSwitchOff()
	{
		
	}
			
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
		
	override string GetDeploySoundset()
	{
		return "placeRefridgerator_SoundSet";
	}
	
	override void SetActions()
	{
		super.SetActions();
		RemoveAction(ActionTakeItemToHands);
		
		AddAction(ActionPlugIn);
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionUnplugThisByCord);
		AddAction(ActionTurnOnWhileOnGround);
		AddAction(ActionTurnOffWhileOnGround);
		AddAction(ActionRepositionPluggedItem);
		AddAction(ActionPlaceObject);
		AddAction(ActionTakeItemToHands);
	}
}
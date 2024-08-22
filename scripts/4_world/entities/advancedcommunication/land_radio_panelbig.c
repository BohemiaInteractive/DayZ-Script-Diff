class Land_Radio_PanelBig : StaticTransmitter
{
	override bool DisableVicinityIcon()
	{
		return true;
	}
	
	// --- SYSTEM EVENTS
	override void OnStoreSave(ParamsWriteContext ctx)
	{   
		super.OnStoreSave(ctx);
		
		//store tuned frequency
		ctx.Write(GetTunedFrequencyIndex());
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		int tunedFrequencyIndex;
		if (!ctx.Read(tunedFrequencyIndex))
			tunedFrequencyIndex = 0;

		SetFrequencyByIndex(tunedFrequencyIndex);
		
		return true;
	}	
	
	//--- BASE
	override bool IsStaticTransmitter()
	{
		return true;
	}
	
	void SetNextFrequency(PlayerBase player = null)
	{
		SetNextChannel();
	}

	//--- POWER EVENTS
	override void OnSwitchOn()
	{
		if (!GetCompEM().CanWork())
			GetCompEM().SwitchOff();
	}	
	
	override void OnWorkStart()
	{
		//turn on broadcasting
		EnableBroadcast(true);
		EnableReceive(false);
		SwitchOn(true);
	}

	override void OnWorkStop()
	{
		//auto switch off (EM)
		GetCompEM().SwitchOff();
		
		//turn off broadcasting
		EnableBroadcast(false);
		SwitchOn(false);		
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTuneFrequencyOnGround);
	}
}

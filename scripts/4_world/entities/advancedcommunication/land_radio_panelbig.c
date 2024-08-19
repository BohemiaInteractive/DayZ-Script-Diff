class Land_Radio_PanelBig extends StaticTransmitter
{
	override bool DisableVicinityIcon()
	{
		return true;
	}
	
	// --- SYSTEM EVENTS
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
		
		//store tuned frequency
		ctx.Write( GetTunedFrequencyIndex() );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;

		//--- Panel Radio data ---
		//load and set tuned frequency
		int tuned_frequency_idx;
		if ( !ctx.Read( tuned_frequency_idx ) )
		{
			tuned_frequency_idx = 0;		//set default
		}
		SetFrequencyByIndex( tuned_frequency_idx );
		//---
		
		return true;
	}	
	
	//--- BASE
	override bool IsStaticTransmitter()
	{
		return true;
	}
	
	void SetNextFrequency( PlayerBase player = NULL )
	{
		SetNextChannel();
		
		/*
		if ( player )
		{
			DisplayRadioInfo( GetTunedFrequency().ToString(), player );
		}
		*/
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
		//turn on broadcasting/receiving
		EnableBroadcast ( true );
		EnableReceive ( true );
		SwitchOn ( true );
	}

	override void OnWorkStop()
	{
		//auto switch off (EM)
		GetCompEM().SwitchOff();
		
		//turn off broadcasting/receiving
		EnableBroadcast ( false );
		EnableReceive ( false );	
		SwitchOn ( false );		
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTuneFrequencyOnGround);
	}
}

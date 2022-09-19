//TRANSMITTER BASE
class TransmitterBase extends ItemTransmitter
{
	//Sounds
	string SOUND_RADIO_TURNED_ON 		= "";

	protected EffectSound m_SoundLoop;
	
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
		
		//--- Transmitter data ---
		//load and set tuned frequency
		int tuned_frequency_idx;
		if ( !ctx.Read( tuned_frequency_idx ) )
		{
			SetFrequencyByIndex( 0 );		//set default
			return false;
		}
		SetFrequencyByIndex( tuned_frequency_idx );
		//---
		
		return true;
	}	
	
	override bool IsTransmitter()
	{
		return true;
	}
	
	//--- ACTIONS
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
	
	//--- HUD
	/*
	protected Hud GetHud( PlayerBase player )
	{
		if ( !player ) 
		{
			return NULL;
		}
		
		return player.m_Hud;
	}
	
	void DisplayRadioInfo( string message, PlayerBase player )
	{
		Hud hud;
		if ( player )
		{
			hud = GetHud( player );
		}
		
		if ( hud ) 
		{	
			hud.SetWalkieTalkieText( message );
			hud.ShowWalkieTalkie( 3 );
		}
	}
	*/
	
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
		
		//play sound
		SoundTurnedOnNoiseStart();
	}

	override void OnWorkStop()
	{
		//auto switch off (EM)
		GetCompEM().SwitchOff();
		
		//turn off broadcasting/receiving
		EnableBroadcast ( false );
		EnableReceive ( false );
		SwitchOn ( false );
		
		//stop sound
		SoundTurnedOnNoiseStop();		
	}
	
	//================================================================
	// SOUNDS
	//================================================================
	//Static noise when the radio is turned on
	protected void SoundTurnedOnNoiseStart()
	{
		PlaySoundSetLoop( m_SoundLoop, SOUND_RADIO_TURNED_ON, 1.0, 1.0 );
	}

	protected void SoundTurnedOnNoiseStop()
	{
		StopSoundSet( m_SoundLoop );
	}	
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnTransmitter);
		AddAction(ActionTurnOffTransmitter);
		AddAction(ActionTuneFrequency);
	}
}

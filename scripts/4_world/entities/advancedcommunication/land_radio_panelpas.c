class Land_Radio_PanelPAS extends PASBroadcaster
{
	//Sounds
	const string SOUND_PAS_TURN_ON 			= "pastransmitter_turnon_SoundSet";
	const string SOUND_PAS_TURN_OFF 		= "pastransmitter_turnoff_SoundSet";
	const string SOUND_PAS_TURNED_ON 		= "pastransmitter_staticnoise_SoundSet";

	protected EffectSound m_Sound;
	protected EffectSound m_SoundLoop;
	
	//--- BASE
	override bool IsStaticTransmitter()
	{
		return true;
	}
	
	override bool DisableVicinityIcon()
	{
		return true;
	}
	
	//--- POWER EVENTS
	override void OnSwitchOn()
	{
		super.OnSwitchOn();
		
		if ( !GetCompEM().CanWork() )
		{
			GetCompEM().SwitchOff();
		}
		
		//sound
		SoundTurnOn();
	}
	
	override void OnSwitchOff()
	{
		super.OnSwitchOff();
		
		//sound
		SoundTurnOff();
	}	
	
	override void OnWorkStart()
	{
		super.OnWorkStart();
		
		//turn off device
		SwitchOn ( true ); // start send/receive voice
		
		//sound
		SoundTurnedOnNoiseStart();
	}
	
	override void OnWorkStop()
	{
		super.OnWorkStop();
		
		//turn off device
		SwitchOn ( false ); // stop send/receive voice
		
		//sound
		SoundTurnedOnNoiseStop();		
	}
	
	//================================================================
	// SOUNDS
	//================================================================
	//Static noise when the radio is turned on
	protected void SoundTurnedOnNoiseStart()
	{
		PlaySoundSetLoop( m_SoundLoop, SOUND_PAS_TURNED_ON, 1.0, 1.0 );
	}

	protected void SoundTurnedOnNoiseStop()
	{
		StopSoundSet( m_SoundLoop );
	}
	
	protected void SoundTurnOn()
	{
		PlaySoundSet( m_Sound, SOUND_PAS_TURN_ON, 0, 0 );
	}
	
	protected void SoundTurnOff()
	{
		PlaySoundSet( m_Sound, SOUND_PAS_TURN_OFF, 0, 0 );
	}
}

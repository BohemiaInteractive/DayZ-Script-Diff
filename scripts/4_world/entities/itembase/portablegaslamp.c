class PortableGasLamp extends ItemBase
{
	PortableGasLampLight	m_Light;
	
	private const string GAS_LIGHT_MATERIAL_ON 		= "dz\\gear\\cooking\\data\\GasLightOn.rvmat";
	private const string GAS_LIGHT_MATERIAL_OFF 	= "dz\\data\\data\\default.rvmat";

	//sound
	const string SOUND_BURNING 		= "portablegaslamp_burn_SoundSet";
	const string SOUND_TURN_ON		= "portablegaslamp_turn_on_SoundSet";
	const string SOUND_TURN_OFF		= "portablegaslamp_turn_off_SoundSet";
	
	protected EffectSound m_SoundBurningLoop;
	protected EffectSound m_SoundTurnOn;
	protected EffectSound m_SoundTurnOff;
	
	//--- POWER EVENTS
	override void OnSwitchOn()
	{
		super.OnSwitchOn();
		
		//sound (client only)
		SoundTurnOn();
	}

	override void OnSwitchOff()
	{
		super.OnSwitchOff();
		
		//sound (client only)
		SoundTurnOff();
	}
	
	override void OnWorkStart()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
		{
			m_Light = PortableGasLampLight.Cast( ScriptedLightBase.CreateLight( PortableGasLampLight, "0 0 0") );
			m_Light.AttachOnMemoryPoint(this, "light");
		}
		
		//refresh visual
		SetObjectMaterial( 0, GAS_LIGHT_MATERIAL_ON );
		
		//sound (client only)
		SoundBurningStart();
	}

	override void OnWorkStop()
	{
		if (m_Light)
			m_Light.FadeOut();
		
		//refresh visual
		SetObjectMaterial( 0, GAS_LIGHT_MATERIAL_OFF );
		
		//sound (client only)
		SoundBurningStop();
	}
	
	//================================================================
	// SOUNDS
	//================================================================
	protected void SoundBurningStart()
	{
		PlaySoundSetLoop( m_SoundBurningLoop, SOUND_BURNING, 0.1, 0.3 );
	}
	
	protected void SoundBurningStop()
	{
		StopSoundSet( m_SoundBurningLoop );
	}	

	protected void SoundTurnOn()
	{
		PlaySoundSet( m_SoundTurnOn, SOUND_TURN_ON, 0.1, 0.1 );
	}
	
	protected void SoundTurnOff()
	{
		PlaySoundSet( m_SoundTurnOff, SOUND_TURN_OFF, 0.1, 0.1 );
	}	
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
		AddAction(ActionTurnOnWhileOnGround);
		AddAction(ActionTurnOffWhileOnGround);
	}
}
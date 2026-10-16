class PersonalRadio extends TransmitterBase
{
	void PersonalRadio()
	{
		SOUND_RADIO_TURNED_ON = "personalradio_staticnoise_SoundSet";
		SOUND_BUNKER_STATIC_NOISE = "bunkerbroadcast_staticnoise_SoundSet";
	}
	
	override void OnDebugSpawn()
	{
		GetInventory().CreateInInventory("Battery9V");
	}
}

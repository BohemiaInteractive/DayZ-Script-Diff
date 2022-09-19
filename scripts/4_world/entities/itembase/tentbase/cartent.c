class CarTent extends TentBase
{
	void CarTent()
	{		
		m_ToggleAnimations.Insert( new ToggleAnimations("EntranceO", "EntranceC", OPENING_0), 0 );
			
		m_ShowAnimationsWhenPitched.Insert( "Body" );
		//m_ShowAnimationsWhenPitched.Insert( "EntranceO" );
		m_ShowAnimationsWhenPitched.Insert( "Pack" );
		
		m_ShowAnimationsWhenPacked.Insert( "Inventory" );
		
		m_HalfExtents = Vector(1.8, 0.33, 3.4);
	}
		
	override void EEInit()
	{		
		super.EEInit();
	}
	
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{		
		super.OnItemLocationChanged(old_owner, new_owner);
	}
	
	override string GetSoundOpen()
	{
		return "CarTent_Door_Open_SoundSet";
	}
	
	override string GetSoundClose()
	{
		return "CarTent_Door_Close_SoundSet";
	}
	
	override bool HasClutterCutter()
	{
		return false;
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================

	override bool IsDeployable()
	{
		return true;
	}
	
	override string GetDeploySoundset()
	{
		return "placeCarTent_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "cartent_deploy_SoundSet";
	}
};

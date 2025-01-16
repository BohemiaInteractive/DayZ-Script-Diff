class InventoryItemType
{
	private ref map<int, ref AnimSoundEvent> m_AnimSoundEvents;

	private void InventoryItemType()
	{
		LoadSoundEvents();
	}
	
	private void ~InventoryItemType();
	
	void LoadSoundEvents()
	{
		string cfgPath = "CfgVehicles " + GetName() + " AnimEvents SoundWeapon";
		
		m_AnimSoundEvents = new map<int, ref AnimSoundEvent>();

		int soundCount = GetGame().ConfigGetChildrenCount(cfgPath);
		
		if (soundCount <= 0)//try other path
		{
			cfgPath = "CfgWeapons " + GetName() + " AnimEvents SoundWeapon";
			soundCount = GetGame().ConfigGetChildrenCount(cfgPath);
		}
		
		if (soundCount <= 0)//try other path
		{
			cfgPath = "CfgMagazines " + GetName() + " AnimEvents SoundWeapon";
			soundCount = GetGame().ConfigGetChildrenCount(cfgPath);
		}
		
		if (soundCount <= 0)//try other path
		{
			cfgPath = "CfgAmmo " + GetName() + " AnimEvents SoundWeapon";
			soundCount = GetGame().ConfigGetChildrenCount(cfgPath);
		}
		
		for (int i = 0; i < soundCount; i++)
		{
			string soundName;
			GetGame().ConfigGetChildName(cfgPath, i, soundName);			
			string soundPath = cfgPath + " " + soundName + " ";
			AnimSoundEvent soundEvent = new AnimSoundEvent(soundPath);
			if (soundEvent.IsValid())
				m_AnimSoundEvents.Set(soundEvent.m_iID, soundEvent);
		}
	}


	AnimSoundEvent GetSoundEvent(int event_id)
	{
		return m_AnimSoundEvents.Get(event_id);
	}

	proto native owned string GetName();
	
	//! DEPRECATED
	ref array<ref AnimSoundEvent> m_animSoundEvents;
}
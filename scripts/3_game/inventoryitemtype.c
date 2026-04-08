
//#define ITEM_SOUND_EVENTS_DEBUG
class InventoryItemSoundBase
{
	protected string m_Name;
	protected string m_CfgPath;
	protected ref map<int, string> m_AttachSoundSets;
	protected ref map<int, string> m_DetachSoundSets;
	
	void InventoryItemSoundBase(string name, string cfgPath)
	{
		m_Name = name;
		m_CfgPath = cfgPath;
				
		LoadSoundEvents();
	}
	
	protected void LoadSoundEvents();
	
	ref map<int, string> GetAttachSoundSets()
	{
		return m_AttachSoundSets;
	}
	
	ref map<int, string> GetDetachSoundSets()
	{
		return m_DetachSoundSets;
	}
}

class InventoryItemSoundAttach : InventoryItemSoundBase
{
	override void LoadSoundEvents()
	{
		TStringArray cfgSoundsetArray = new TStringArray();
		TStringArray cfgSlotArray = new TStringArray();
		g_Game.ConfigGetTextArray(m_CfgPath + " attachSoundSet", cfgSoundsetArray);
		g_Game.ConfigGetTextArray(m_CfgPath + " attachSoundSlot", cfgSlotArray);
		
		int soundSetCount = cfgSoundsetArray.Count();
		if (soundSetCount > 0 && soundSetCount == cfgSlotArray.Count())
		{
			m_AttachSoundSets = new map<int, string>();
			
			for (int i = 0; i < soundSetCount; ++i)
			{
				string soundSet = cfgSoundsetArray[i];
				if (soundSet == "")
				{
					ErrorEx(string.Format("[%1] Try to register attach sound set with empty string not allowed! Please check attachSoundSet configuration for item %2", i, m_Name));
					continue;
				}
				
				string slotName = cfgSlotArray[i];
				if (slotName == "")
				{
					ErrorEx(string.Format("[%1] Try to register attach sound set %2 with empty slot name. Please check attachSoundSlot configuration for item %3", i, soundSet, m_Name));
					continue;
				}
				
				int slotId = InventorySlots.GetSlotIdFromString(slotName);
				if (slotId == InventorySlots.INVALID)
				{
					ErrorEx(string.Format("[%1] Try to register attach sound set %2 with invalid slot ID %3 (slot name: %4). Please check attachSoundSlot configuration for item %5", i, soundSet, slotId, slotName, m_Name));
					continue;
				}
				
				if (!m_AttachSoundSets.Contains(slotId))
				{
					m_AttachSoundSets.Insert(slotId, soundSet);
				#ifdef ITEM_SOUND_EVENTS_DEBUG
					ErrorEx(string.Format("[%1] Registered attach sound set %2. Slot name: %3 | Slot ID: %4 | Item: %5", i, soundSet, slotName, slotId, m_Name), ErrorExSeverity.INFO);
				#endif
				}
			}
		}
	#ifdef ITEM_SOUND_EVENTS_DEBUG
		else
		{
			ErrorEx(string.Format("[%1] No attach sounds cofigurations found for item %1. Path: %2", m_Name, m_CfgPath + " attachSoundSet"), ErrorExSeverity.INFO);
		}
	#endif
	}
}

class InventoryItemSoundDetach : InventoryItemSoundBase
{
	override void LoadSoundEvents()
	{
		TStringArray cfgSoundsetArray = new TStringArray;
		TStringArray cfgSlotArray = new TStringArray;
		g_Game.ConfigGetTextArray(m_CfgPath + " detachSoundSet", cfgSoundsetArray);
		g_Game.ConfigGetTextArray(m_CfgPath + " detachSoundSlot", cfgSlotArray);
		
		if (cfgSoundsetArray.Count() > 0 && cfgSoundsetArray.Count() == cfgSlotArray.Count())
		{
			m_DetachSoundSets = new map<int, string>();
			
			for (int i = 0; i < cfgSoundsetArray.Count(); ++i)
			{
				string soundSet = cfgSoundsetArray[i];
				if (soundSet == "")
				{
					ErrorEx(string.Format("[%1] Try to register detach sound set %2 but sting is empty! Please check detachSoundSet configuration for item %3", i, soundSet, m_Name));
					continue;
				}
				
				string slotName = cfgSlotArray[i];
				if (slotName == "")
				{
					ErrorEx(string.Format("[%1] Try to register detach sound set %2 with invalid slot name %3. Please check detachSoundSlot configuration for item %4", i, soundSet, slotName, m_Name));
					continue;
				}
				
				int slotId = InventorySlots.GetSlotIdFromString(slotName);
				if (slotId == InventorySlots.INVALID)
				{
					ErrorEx(string.Format("[%1] Try to register detach sound set %2 with invalid slot ID %3 (slot name: %4). Please check detachSoundSlot configuration for item %5", i, soundSet, slotId, slotName, m_Name));
					continue;
				}
				
				if (!m_DetachSoundSets.Contains(slotId))
				{
					m_DetachSoundSets.Insert(slotId, soundSet);
				#ifdef ITEM_SOUND_EVENTS_DEBUG
					ErrorEx(string.Format("[%1] Registered detach sound set %2. Slot name: %3 | Slot ID: %4 | Item: %5", i, soundSet, slotName, slotId, m_Name), ErrorExSeverity.INFO);
				#endif
				}
			}
		}
	#ifdef ITEM_SOUND_EVENTS_DEBUG
		else
		{
			ErrorEx(string.Format("[%1] No detach sounds cofigurations found for item %1. Path: %2", m_Name, m_CfgPath + " detachSoundSet"), ErrorExSeverity.INFO);
		}
	#endif
	}
}

class InventoryItemType : EntityAIType
{
	private ref map<int, ref AnimSoundEvent> m_AnimSoundEvents;
	private ref map<int, ref InventoryItemSoundBase> m_ActionSoundEvents = new map<int, ref InventoryItemSoundBase>(); // id (SoundConstants), InventoryItemSoundBase
	private string m_DropSoundSet;
	
	void InventoryItemType()
	{
		LoadSoundEvents();
	}
	
	void ~InventoryItemType()
	{
	}
	
	void LoadSoundEvents()
	{
		string name = GetName();
		string cfgPath = CFG_VEHICLESPATH + " " + name + " AnimEvents SoundWeapon";
		
		m_AnimSoundEvents = new map<int, ref AnimSoundEvent>();

		int soundCount = g_Game.ConfigGetChildrenCount(cfgPath);
		
		if (soundCount <= 0)
		{
			cfgPath = CFG_WEAPONSPATH + " " + name + " AnimEvents SoundWeapon";
			soundCount = g_Game.ConfigGetChildrenCount(cfgPath);
		}
		
		if (soundCount <= 0)
		{
			cfgPath = CFG_MAGAZINESPATH + " " + name + " AnimEvents SoundWeapon";
			soundCount = g_Game.ConfigGetChildrenCount(cfgPath);
		}
		
		if (soundCount <= 0)
		{
			cfgPath = CFG_AMMO + " " + name + " AnimEvents SoundWeapon";
			soundCount = g_Game.ConfigGetChildrenCount(cfgPath);
		}
		
		for (int i = 0; i < soundCount; ++i)
		{
			string soundName;
			g_Game.ConfigGetChildName(cfgPath, i, soundName);			
			string soundPath = cfgPath + " " + soundName + " ";
			AnimSoundEvent soundEvent = new AnimSoundEvent(soundPath);
			if (soundEvent.IsValid())
			{
				m_AnimSoundEvents.Set(soundEvent.m_iID, soundEvent);
			#ifdef ITEM_SOUND_EVENTS_DEBUG
				ErrorEx(string.Format("[%1] Registered animation sound event for item %2. Sound name= %3 | Sound path= %4", i, name, soundName, soundPath), ErrorExSeverity.INFO);
			#endif
			}
		#ifdef ITEM_SOUND_EVENTS_DEBUG
			else
			{
				ErrorEx(string.Format("[%1] Skipped registration of animation sound event for item %2. Sound event invalid in configuration class! Sound name= %3 | Sound path= %4", i, name, soundName, soundPath), ErrorExSeverity.INFO);
			}
		#endif
		}
		
		SetDetachSoundEvent();	//! Detach sound
		SetAttachSoundEvent();	//! Attach sound
	}
		
	bool SetDetachSoundEvent()
	{
		TStringArray cfgDetachSoundsetArray = new TStringArray;
		string name = GetName();
		
		string cfgPath = CFG_VEHICLESPATH + " " + name;
		string soundPath = cfgPath + " detachSoundSet";
		g_Game.ConfigGetTextArray(soundPath, cfgDetachSoundsetArray);
		int soundCount = cfgDetachSoundsetArray.Count();
		
		if (soundCount <= 0)
		{
			cfgPath = CFG_WEAPONSPATH + " " + name;
			soundPath = cfgPath + " detachSoundSet";
			g_Game.ConfigGetTextArray(soundPath, cfgDetachSoundsetArray);
			soundCount = cfgDetachSoundsetArray.Count();
		}
		
		if (soundCount <= 0)
		{
			cfgPath = CFG_MAGAZINESPATH + " " + name;
			soundPath = cfgPath + " detachSoundSet";
			g_Game.ConfigGetTextArray(soundPath, cfgDetachSoundsetArray);
			soundCount = cfgDetachSoundsetArray.Count();
		}
		
		if (soundCount <= 0)
		{
			cfgPath = CFG_AMMO + " " + name;
			soundPath = cfgPath + " detachSoundSet";
			g_Game.ConfigGetTextArray(soundPath, cfgDetachSoundsetArray);
			soundCount = cfgDetachSoundsetArray.Count();
		}
		
		if (soundCount > 0)
		{
			m_ActionSoundEvents.Set(SoundConstants.ITEM_DETACH, new InventoryItemSoundDetach(name, cfgPath));
		#ifdef ITEM_SOUND_EVENTS_DEBUG
			ErrorEx(string.Format("Registered detach sound event instance for item %1", GetName()), ErrorExSeverity.INFO);
		#endif
			return true;
		}
	#ifdef ITEM_SOUND_EVENTS_DEBUG
		else
		{
			ErrorEx(string.Format("Skipped creation of detach sound event instance for item %1. No detach sound in configuration class!", GetName()), ErrorExSeverity.INFO);
		}
	#endif
		
		return false;
	}
	
	bool SetAttachSoundEvent()
	{
		TStringArray cfgAttachSoundsetArray = new TStringArray;
		string name = GetName();
		
		string cfgPath = CFG_VEHICLESPATH + " " + name;
		string soundPath = cfgPath + " attachSoundSet";
		g_Game.ConfigGetTextArray(soundPath, cfgAttachSoundsetArray);
		int soundCount = cfgAttachSoundsetArray.Count();
		
		if (soundCount <= 0)
		{
			cfgPath = CFG_WEAPONSPATH + " " + name;
			soundPath = cfgPath + " attachSoundSet";
			g_Game.ConfigGetTextArray(soundPath, cfgAttachSoundsetArray);
			soundCount = cfgAttachSoundsetArray.Count();
		}
		
		if (soundCount <= 0)
		{
			cfgPath = CFG_MAGAZINESPATH + " " + name;
			soundPath = cfgPath + " attachSoundSet";
			g_Game.ConfigGetTextArray(soundPath, cfgAttachSoundsetArray);
			soundCount = cfgAttachSoundsetArray.Count();
		}
		
		if (soundCount <= 0)
		{
			cfgPath = CFG_AMMO + " " + name;
			soundPath = cfgPath + " attachSoundSet";
			g_Game.ConfigGetTextArray(soundPath, cfgAttachSoundsetArray);
			soundCount = cfgAttachSoundsetArray.Count();
		}
		
		if (soundCount > 0)
		{
			m_ActionSoundEvents.Set(SoundConstants.ITEM_ATTACH, new InventoryItemSoundAttach(name, cfgPath));
		#ifdef ITEM_SOUND_EVENTS_DEBUG
			ErrorEx(string.Format("Registered attach sound event instance for item %1", name), ErrorExSeverity.INFO);
		#endif
			return true;
		}
	#ifdef ITEM_SOUND_EVENTS_DEBUG
		else
		{
			ErrorEx(string.Format("Skipped creation of attach sound event instance for item %1. No attach sound in configuration class!", name), ErrorExSeverity.INFO);
		}
	#endif
		
		return false;
	}

	AnimSoundEvent GetSoundEvent(int event_id)
	{
		return m_AnimSoundEvents.Get(event_id);
	}
	
	InventoryItemSoundAttach GetAttachSoundEvent()
	{
		return InventoryItemSoundAttach.Cast(m_ActionSoundEvents[SoundConstants.ITEM_ATTACH]);
	}
	
	InventoryItemSoundDetach GetDetachSoundEvent()
	{
		return InventoryItemSoundDetach.Cast(m_ActionSoundEvents[SoundConstants.ITEM_DETACH]);
	}
	
	string GetSlotAttachSoundSet(int slotId = InventorySlots.INVALID)
	{
		InventoryItemSoundAttach soundEvent = GetAttachSoundEvent();
		if (!soundEvent)
		{
		#ifdef ITEM_SOUND_EVENTS_DEBUG
			ErrorEx(string.Format("No attach sound event instance found for item %1", GetName()), ErrorExSeverity.INFO);
		#endif
			return "";
		}
		
		string soundSet;
		map<int, string> soundSets = soundEvent.GetAttachSoundSets();		
		if (soundSets && soundSets.Count() > 0)
		{
			if (slotId != InventorySlots.INVALID)
			{
				soundSets.Find(slotId, soundSet);
			}
			else
			{
				soundSet = soundSets.GetElement(0);
			}
		}
		return soundSet;
	}
	
	string GetSlotDetachSoundSet(int slotId = InventorySlots.INVALID)
	{
		InventoryItemSoundDetach soundEvent = GetDetachSoundEvent();
		if (!soundEvent)
		{
		#ifdef ITEM_SOUND_EVENTS_DEBUG
			ErrorEx(string.Format("No detach sound event instance found for item %1", GetName()), ErrorExSeverity.INFO);
		#endif
			return "";
		}
		
		string soundSet;
		map<int, string> soundSets = soundEvent.GetDetachSoundSets();		
		if (soundSets && soundSets.Count() > 0)
		{
			if (slotId != InventorySlots.INVALID)
			{
				soundSets.Find(slotId, soundSet);
			}
			else
			{
				soundSet = soundSets.GetElement(0);
			}
		}
		return soundSet;
	}
	
	//! DEPRECATED
	ref array<ref AnimSoundEvent> m_animSoundEvents;
}
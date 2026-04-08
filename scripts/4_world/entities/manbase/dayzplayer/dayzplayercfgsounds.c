class DayZPlayerTypeStepSoundLookupTableImpl extends DayZPlayerTypeStepSoundLookupTable
{
	void DayZPlayerTypeStepSoundLookupTableImpl()
	{
		m_pSoundTableInstances = new map<int, ref StepSoundLookupTable>;
		m_pSoundTables = new map<int, StepSoundLookupTable>;
		
		string stepsCfgPath = "CfgVehicles SurvivorBase AnimEvents Steps ";
		int stepsCount = g_Game.ConfigGetChildrenCount(stepsCfgPath);
		for(int i = 0; i < stepsCount; i++)
		{
			string stepName;
			g_Game.ConfigGetChildName(stepsCfgPath, i, stepName);
			string stepPath = stepsCfgPath + stepName + " ";
			int id = g_Game.ConfigGetInt(stepPath + "id");
			
			string tableName;
			g_Game.ConfigGetText(stepPath + "soundLookupTable", tableName);

			
			StepSoundLookupTable table = m_pSoundTableInstances.Get(tableName.Hash());
			if(table == NULL)
			{
				table = new StepSoundLookupTable();
				table.LoadTable(tableName);
				m_pSoundTableInstances.Insert(tableName.Hash(), table);
			}
			m_pSoundTables.Insert(id, table);
		}
	}

	override SoundObjectBuilder GetSoundBuilder(int eventId, int pMovement, int pSurfaceHash, AnimBootsType pBoots)
	{
		SoundLookupTable table = m_pSoundTables.Get(eventId);
		if(table == NULL)
			return NULL;
		
		SoundObjectBuilder soundBuilder = table.GetSoundBuilder(pSurfaceHash);
		if(soundBuilder == NULL)
			return NULL;
		
		if (pMovement == DayZPlayerConstants.MOVEMENTIDX_WALK)
		{
			soundBuilder.AddVariable("walk", 1);
			soundBuilder.AddVariable("run", 0);
			soundBuilder.AddVariable("sprint", 0);
		}
		else if (pMovement == DayZPlayerConstants.MOVEMENTIDX_RUN)
		{
			soundBuilder.AddVariable("walk", 0);
			soundBuilder.AddVariable("run", 1);
			soundBuilder.AddVariable("sprint", 0);
		}
		else if (pMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT)
		{
			soundBuilder.AddVariable("walk", 0);
			soundBuilder.AddVariable("run", 0);
			soundBuilder.AddVariable("sprint", 1);
		}
		else
		{
			soundBuilder.AddVariable("walk", 0);
			soundBuilder.AddVariable("run", 0);
			soundBuilder.AddVariable("sprint", 0);
		}
		
		if (pBoots == AnimBootsType.None)
		{
			soundBuilder.AddVariable("bare", 1);
			soundBuilder.AddVariable("sneakers", 0);
			soundBuilder.AddVariable("boots", 0);
		}
		else if (pBoots == AnimBootsType.Sneakers)
		{
			soundBuilder.AddVariable("bare", 0);
			soundBuilder.AddVariable("sneakers", 1);
			soundBuilder.AddVariable("boots", 0);
		}
		else if (pBoots == AnimBootsType.Boots)
		{
			soundBuilder.AddVariable("bare", 0);
			soundBuilder.AddVariable("sneakers", 0);
			soundBuilder.AddVariable("boots", 1);
		}
		
		return soundBuilder;
	}
	
	static DayZPlayerTypeStepSoundLookupTableImpl GetInstance()
	{
		if(m_instance == NULL)
			m_instance = new DayZPlayerTypeStepSoundLookupTableImpl();

		return m_instance;
	}

	private static ref DayZPlayerTypeStepSoundLookupTableImpl m_instance;
	private autoptr map<int, ref StepSoundLookupTable> m_pSoundTableInstances;//unique tables
	private autoptr map<int, StepSoundLookupTable> m_pSoundTables;//pointers to tables above
}


class DayZPlayerTypeAttachmentSoundLookupTableImpl extends DayZPlayerTypeAttachmentSoundLookupTable
{
	void DayZPlayerTypeAttachmentSoundLookupTableImpl()
	{
		m_pSoundTableInstances = new map<int, ref AttachmentSoundLookupTable>;
		m_pSoundTables = new map<int, AttachmentSoundLookupTable>();
		
		string attachCfgPath = "CfgVehicles SurvivorBase AnimEvents Attachments ";
		int attachCount = g_Game.ConfigGetChildrenCount(attachCfgPath);
		for(int i = 0; i < attachCount; i++)
		{
			string defName;
			g_Game.ConfigGetChildName(attachCfgPath, i, defName);
			string defPath = attachCfgPath + defName + " ";
			
			string slotName;
			g_Game.ConfigGetText(defPath + "slot", slotName);
			
			int id = g_Game.ConfigGetInt(defPath + "id");
			
			string tableName;
			g_Game.ConfigGetText(defPath + "soundLookupTable", tableName);
			
			AttachmentSoundLookupTable table = m_pSoundTableInstances.Get(tableName.Hash());
			if(table == NULL)
			{
				table = new AttachmentSoundLookupTable();
				table.LoadTable(tableName);
				m_pSoundTableInstances.Insert(tableName.Hash(), table);
			}

			m_pSoundTables.Insert((slotName + id).Hash(), table);
		}
	}
	
	override SoundObjectBuilder GetSoundBuilder(int eventId, string slotName, int attachmentHash)
	{
		SoundLookupTable table = m_pSoundTables.Get((slotName + eventId).Hash());
		if(table == NULL)
			return NULL;
		
		SoundObjectBuilder soundBuilder = table.GetSoundBuilder(attachmentHash);
		if(soundBuilder == NULL)
			return NULL;
		
		return soundBuilder;
	}
	
	static DayZPlayerTypeAttachmentSoundLookupTableImpl GetInstance()
	{
		if(m_instance == NULL)
			m_instance = new DayZPlayerTypeAttachmentSoundLookupTableImpl();

		return m_instance;
	}
	
	private static ref DayZPlayerTypeAttachmentSoundLookupTableImpl m_instance;
	private autoptr map<int, ref AttachmentSoundLookupTable> m_pSoundTableInstances;
	private autoptr map<int, AttachmentSoundLookupTable> m_pSoundTables;
}




class DayZPlayerTypeVoiceSoundLookupTableImpl extends DayZPlayerTypeVoiceSoundLookupTable
{
	void DayZPlayerTypeVoiceSoundLookupTableImpl()
	{
		// this produces 2 maps: 
		// 1) map where the key is 'ID' of anim event and the value is sound lookup table
		// 2) map of unique lookup table instances where the table name hash is a key, and a lookup table is the value
		m_pSoundTableInstances = new map<int, ref PlayerVoiceLookupTable>;
		m_pSoundTables = new map<int, PlayerVoiceLookupTable>();
		
		string cfgPath = "CfgVehicles SurvivorBase AnimEvents SoundVoice ";
		int childCount = g_Game.ConfigGetChildrenCount(cfgPath);
		//Print("childCount:" + childCount);
		for(int i = 0; i < childCount; i++)
		{
			string defName;
			g_Game.ConfigGetChildName(cfgPath, i, defName);
			string defPath = cfgPath + defName + " ";
			
			int id = g_Game.ConfigGetInt(defPath + "id");
			
			string tableName;
			g_Game.ConfigGetText(defPath + "soundLookupTable", tableName);
			
			PlayerVoiceLookupTable table = m_pSoundTableInstances.Get(tableName.Hash());
			if(table == NULL)
			{
				table = new PlayerVoiceLookupTable();
				table.LoadTable(tableName);
				m_pSoundTableInstances.Insert(tableName.Hash(), table);
				
				string noiseName;
				if(g_Game.ConfigGetText(defPath + "noise", noiseName))
				{
					NoiseParams np = new NoiseParams;
					np.Load(noiseName);
					table.SetNoiseParam(np);
				}
			}

			m_pSoundTables.Insert(id, table);
		}
	}
	
	override SoundObjectBuilder GetSoundBuilder(int eventId, int parameterHash)
	{
		PlayerVoiceLookupTable table = m_pSoundTables.Get(eventId);
		if(table == NULL)
			return NULL;
		
		SoundObjectBuilder soundBuilder = table.GetSoundBuilder(parameterHash);
		if(soundBuilder == NULL)
			return NULL;
		
		return soundBuilder;
	}
	
	override NoiseParams GetNoiseParams(int eventId)
	{
		PlayerVoiceLookupTable table = m_pSoundTables.Get(eventId);
		if(table == NULL)
			return NULL;
		
		return table.GetNoiseParam();
	}
	
	static DayZPlayerTypeVoiceSoundLookupTableImpl GetInstance()
	{
		if(m_instance == NULL)
			m_instance = new DayZPlayerTypeVoiceSoundLookupTableImpl();

		return m_instance;
	}
	
	private static ref DayZPlayerTypeVoiceSoundLookupTableImpl m_instance;
	private autoptr map<int, ref PlayerVoiceLookupTable> m_pSoundTableInstances;
	private autoptr map<int, PlayerVoiceLookupTable> m_pSoundTables;
}



class DayZPlayerTypeSoundTableImpl extends DayZPlayerTypeAnimTable
{	
	private static ref DayZPlayerTypeSoundTableImpl m_instance;
	private ref map<int, ref AnimSoundEvent> m_AnimSoundEvents;

	void DayZPlayerTypeSoundTableImpl()
	{
		m_AnimSoundEvents = new map<int, ref AnimSoundEvent>();
		
		string soundsCfgPath = "CfgVehicles SurvivorBase AnimEvents Sounds ";

		int soundCount = g_Game.ConfigGetChildrenCount(soundsCfgPath);
		for(int i = 0; i < soundCount; i++)
		{
			string soundName;
			g_Game.ConfigGetChildName(soundsCfgPath, i, soundName);			
			string soundPath = soundsCfgPath + soundName + " ";
			AnimSoundEvent soundEvent = new AnimSoundEvent(soundPath);
			if(soundEvent.IsValid())
				m_AnimSoundEvents.Set(soundEvent.m_iID, soundEvent);
		}
	}
	
	override AnimSoundEvent GetSoundEvent(int event_id)
	{
		return m_AnimSoundEvents.Get(event_id);
	}
	
	static DayZPlayerTypeSoundTableImpl GetInstance()
	{
		if(m_instance == null)
			m_instance = new DayZPlayerTypeSoundTableImpl();

		return m_instance;
	}

	//! DEPRECATED	
	private ref array<ref AnimSoundEvent> m_animSoundEvents;
}

/*
class DayZPlayerTypeSoundVoiceTableImpl extends DayZPlayerTypeAnimTable
{
	void DayZPlayerTypeSoundVoiceTableImpl()
	{
		m_animSoundEvents = new map<int, ref AnimSoundEvent>;
		
		string soundsCfgPath = "CfgVehicles SurvivorBase AnimEvents SoundVoice ";

		int soundCount = g_Game.ConfigGetChildrenCount(soundsCfgPath);
		for(int i = 0; i < soundCount; i++)
		{
			string soundName;
			g_Game.ConfigGetChildName(soundsCfgPath, i, soundName);			
			string soundPath = soundsCfgPath + soundName + " ";
			AnimSoundEvent soundEvent = new AnimSoundEvent(soundPath);
			if(soundEvent.IsValid())
				m_animSoundEvents.Insert(soundEvent.m_iID, soundEvent);
		}
	}
	
	override AnimSoundEvent GetSoundEvent(int event_id)
	{
		AnimSoundEvent soundEvent = m_animSoundEvents.Get(event_id);
		return soundEvent;
	}
	
	ref map<int, ref AnimSoundEvent> m_animSoundEvents;
}
*/

void DayZPlayerTypeRegisterSounds(DayZPlayerType pType)
{
	g_Game.ProfilerStart("DayZPlayerTypeRegisterSounds");
	//! register events
	pType.RegisterStepEvent("Step", 0.2);

	pType.RegisterSoundEvent("Sound", -1);
	pType.RegisterSoundEvent("SoundWeapon", 0.2);
	pType.RegisterSoundEvent("SoundVoice", -1);
	if(!g_Game.IsDedicatedServer())//attachments don't generate noise, so we can ignore them on server
		pType.RegisterSoundEvent("SoundAttachment", 0.2);
	
	
	DayZPlayerTypeVoiceSoundLookupTableImpl voiceTable2 = DayZPlayerTypeVoiceSoundLookupTableImpl.GetInstance();
	pType.RegisterVoiceSoundLookupTable(voiceTable2);
	
	if(!g_Game.IsDedicatedServer())//sounds are unnecessary on server
	{
		pType.RegisterParticleEvent("Particle", -1);
		//! load and register step sound lookup table
		DayZPlayerTypeStepSoundLookupTableImpl stepTable = DayZPlayerTypeStepSoundLookupTableImpl.GetInstance();
		pType.RegisterStepSoundLookupTable(stepTable);
		
		DayZPlayerTypeAttachmentSoundLookupTableImpl attachTable = DayZPlayerTypeAttachmentSoundLookupTableImpl.GetInstance();
		pType.RegisterAttachmentSoundLookupTable(attachTable);
		

		
		DayZPlayerTypeSoundTableImpl soundTable = DayZPlayerTypeSoundTableImpl.GetInstance();
		pType.RegisterSoundTable(soundTable);
		
		//DayZPlayerTypeSoundVoiceTableImpl voiceTable = new DayZPlayerTypeSoundVoiceTableImpl();
		//pType.RegisterSoundVoiceTable(voiceTable);
	}
	g_Game.ProfilerStop("DayZPlayerTypeRegisterSounds");
}
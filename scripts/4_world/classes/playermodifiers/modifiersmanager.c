//max 32 synced modifiers supported
enum eModifierSyncIDs 
{
	MODIFIER_SYNC_WOUND_INFECT_1 	= 0x00000001,
	MODIFIER_SYNC_WOUND_INFECT_2 	= 0x00000002,
	MODIFIER_SYNC_CONTAMINATION 	= 0x00000004,//stage1 
	MODIFIER_SYNC_CONTAMINATION2 	= 0x00000008,//stage2 and stage3 share the same sync id
	MODIFIER_SYNC_ZONE_EXPOSURE 	= 0x00000010,
	MODIFIER_SYNC_DROWNING 			= 0x00000020,
	MODIFIER_SYNC_FEVER				= 0x00000040,
	//0x00000080,
	//0x00000100,
	//0x00000200,
	//0x00000400,
	//0x00000800,
	//0x00001000,
	LAST_INDEX,
}

enum EActivationType
{
	TRIGGER_EVENT_OFF,
	TRIGGER_EVENT_ON_ACTIVATION,
	TRIGGER_EVENT_ON_CONNECT
}

const int DEFAULT_TICK_TIME_ACTIVE 			= 3;
const int DEFAULT_TICK_TIME_ACTIVE_SHORT 	= 1;
const int DEFAULT_TICK_TIME_INACTIVE 		= 3;
const int DEFAULT_TICK_TIME_INACTIVE_LONG 	= 10;

class ModifierDebugObj
{
	string 	m_Name;
	int 	m_ID;
	bool 	m_IsActive;
	bool	m_IsLocked;
	
	void ModifierDebugObj(int id, string name, bool active, bool locked)
	{
		m_Name = name;
		m_ID = id;
		m_IsActive = active;
		m_IsLocked = locked;
	}
	
	string GetName()
	{
		return m_Name;
	}
	
	int GetID()
	{
		return m_ID;
	}
	
	bool IsActive()
	{
		return m_IsActive;
	}
	
	bool IsLocked()
	{
		return m_IsLocked;
	}
}

class ModifiersManager
{
	const int STORAGE_VERSION = 121;

	bool m_AllowModifierTick;

	PlayerBase m_Player;

	ref map<int, ref ModifierBase> 	m_ModifierList;
	ref array< ref ModifierBase> 	m_ModifierListArray;
	ref array<ref Param> 			m_ParamList;

	void ModifiersManager(PlayerBase player)
	{
		m_ModifierList 		= new map<int, ref ModifierBase>();
		m_ModifierListArray = new array< ref ModifierBase>();
		m_ParamList 		= new array<ref Param>;
		m_Player 			= player;
		
		Init();
	}

	void Init()
	{
		#ifdef DEVELOPER
		AddModifier(new TestDiseaseMdfr);
		#endif
		AddModifier(new BloodRegenMdfr);
		AddModifier(new SalineMdfr);
		AddModifier(new HealthRegenMdfr);
		AddModifier(new HungerMdfr);
		AddModifier(new ImmuneSystemMdfr);
		AddModifier(new StomachMdfr);
		AddModifier(new HeatComfortMdfr);
		AddModifier(new ThirstMdfr);
		AddModifier(new BleedingCheckMdfr);
		AddModifier(new BrokenLegsMdfr);
		AddModifier(new VomitStuffedMdfr);
		AddModifier(new BurningMdfr);
		AddModifier(new FeverMdfr);
		AddModifier(new HeartAttackMdfr);
		AddModifier(new HemolyticReactionMdfr);
		AddModifier(new PoisoningMdfr);
		AddModifier(new StuffedStomachMdfr);
		AddModifier(new UnconsciousnessMdfr);
		AddModifier(new ShockDamageMdfr);
		AddModifier(new CommonColdMdfr);
		AddModifier(new CholeraMdfr);
		AddModifier(new InfluenzaMdfr);
		AddModifier(new SalmonellaMdfr);
		AddModifier(new BrainDiseaseMdfr);
		AddModifier(new WetMdfr);
		AddModifier(new ImmunityBoost);
		AddModifier(new AntibioticsMdfr);
		AddModifier(new ToxicityMdfr);
		AddModifier(new BreathVapourMdfr);
		AddModifier(new ShockMdfr);
		AddModifier(new WoundInfectStage1Mdfr);
		AddModifier(new WoundInfectStage2Mdfr);
		AddModifier(new CharcoalMdfr);
		AddModifier(new MorphineMdfr);
		AddModifier(new PainKillersMdfr);
		AddModifier(new EpinephrineMdfr);
		AddModifier(new HeatBufferMdfr);
		AddModifier(new DisinfectionMdfr);
		AddModifier(new FatigueMdfr);
		AddModifier(new ContaminationStage1Mdfr);
		AddModifier(new ContaminationStage2Mdfr);
		AddModifier(new ContaminationStage3Mdfr);
		AddModifier(new AreaExposureMdfr);
		AddModifier(new MaskMdfr);
		AddModifier(new FliesMdfr);
		AddModifier(new DrowningMdfr);
		AddModifier(new HeavyMetalPhase1Mdfr);
		AddModifier(new HeavyMetalPhase2Mdfr);
		AddModifier(new HeavyMetalPhase3Mdfr);
		AddModifier(new PneumoniaMdfr);
		AddModifier(new ChelationMdfr);
	}

	void SetModifiers(bool enable)
	{
		if (m_AllowModifierTick == enable)
			return;
		
		m_AllowModifierTick = enable;
		
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		DiagMenu.SetValue(DiagMenuIDs.CHEATS_MODIFIERS, enable);
		#endif
		#endif
		
		if (!enable)
		{
			for (int i = 0; i < m_ModifierList.Count(); i++)
			{
				m_ModifierList.GetElement(i).ResetLastTickTime();
			}
		}
	}

	int GetStorageVersion()
	{
		return STORAGE_VERSION;
	}
	
	bool IsModifiersEnable()
	{
		return m_AllowModifierTick;
	}
	
	void AddModifier(ModifierBase modifier)
	{
		modifier.InitBase(m_Player, this);
		int id = modifier.GetModifierID();
		
		if (id < 1)
			ErrorEx("modifiers ID must be 1 or higher(for debugging reasons)", ErrorExSeverity.WARNING);
		
		if (m_ModifierListArray.Find(modifier) == -1)
		{
			m_ModifierList.Insert(id, modifier);
			m_ModifierListArray.Insert(modifier);
		}
		else
		{
			ErrorEx(string.Format("Modifier '%1' is already registered", modifier.GetName()), ErrorExSeverity.WARNING);
		}
	}
	
	bool IsModifierActive(eModifiers modifier_id)
	{
		return m_ModifierList.Get(modifier_id).IsActive();
	}

	void OnScheduledTick(float delta_time)
	{
		if (!m_AllowModifierTick)
			return;

		foreach (ModifierBase modifier : m_ModifierListArray)
			modifier.Tick(delta_time);
	}
	
	void DeactivateAllModifiers()
	{
		foreach (ModifierBase modifier : m_ModifierList)
			modifier.Deactivate();
	}
	
	void ActivateModifier(int modifier_id, bool triggerEvent = EActivationType.TRIGGER_EVENT_ON_ACTIVATION)
	{
		m_ModifierList.Get(modifier_id).ActivateRequest(triggerEvent);
	}

	void DeactivateModifier(int modifier_id, bool triggerEvent = true)
	{
		m_ModifierList.Get(modifier_id).Deactivate(triggerEvent);
	}
	
	void OnStoreSave( ParamsWriteContext ctx )
	{
		CachedObjectsArrays.ARRAY_INT.Clear();
		
		int modifierCount;
		foreach (ModifierBase modifier : m_ModifierList)
		{
			if (modifier.IsActive() && modifier.IsPersistent())
			{
				modifierCount++;
				//save the modifier id
				CachedObjectsArrays.ARRAY_INT.Insert(modifier.GetModifierID());
				if (modifier.IsTrackAttachedTime())
				{
					//save the overall attached time
					CachedObjectsArrays.ARRAY_INT.Insert(modifier.GetAttachedTime());
				}
			}
		}

		ctx.Write(modifierCount);

		//write the individual modifiers and respective attached times
		for (int i = 0; i < CachedObjectsArrays.ARRAY_INT.Count(); i++)
		{
			int item = CachedObjectsArrays.ARRAY_INT.Get(i);
			ctx.Write(item);
		}

		for (int z = 0; z < m_ParamList.Count(); z++)
		{
			m_ParamList.Get(z).Serialize(ctx);
		}

	}

	bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		int modifierCount;
		if (!ctx.Read(modifierCount))
			return false;

		for (int i = 0; i < modifierCount; i++)
		{
			int modifierId;
			if (!ctx.Read(modifierId))
				return false;

			ModifierBase modifier = GetModifier(modifierId);
			if (modifier)
			{
				if (modifier.IsTrackAttachedTime())
				{
					int time;
					if (!ctx.Read(time))//get the attached time
						return false;

					modifier.SetAttachedTime(time);
				}
				
				ActivateModifier(modifierId, EActivationType.TRIGGER_EVENT_ON_CONNECT);
			}
			else
			{
				Debug.LogError("DB loading: non-existent modifier with id:"+modifierId.ToString());
			}
		}
		
		for (int x = 0; x < m_ParamList.Count(); x++)
		{
			if (!m_ParamList.Get(x).Deserializer(ctx))
				return false;
		}

		return true;
	}

	ModifierBase GetModifier(int modifier_id)
	{
		return m_ModifierList.Get(modifier_id);
	}
		
	PlayerBase GetPlayer()
	{
		return m_Player;
	}

	void SetModifierLock(int modifier_id, bool state)
	{
		m_ModifierList.Get(modifier_id).SetLock(state);
	}

	bool GetModifierLock(int modifier_id)
	{
		return m_ModifierList.Get(modifier_id).IsLocked();
	}

	void DbgGetModifiers(array<ref ModifierDebugObj> modifiers)
	{
		modifiers.Clear();
		for (int i = 1;i < eModifiers.COUNT; i++)
		{
			if (m_ModifierList.Contains(i))
			{
				ModifierBase modifier = m_ModifierList.Get(i);

				int modifierId 	= modifier.GetModifierID();
				bool isLocked 	= modifier.IsLocked();
				bool active 	= modifier.IsActive();

				string modifierName = modifier.GetName();
				string debugText 	= modifier.GetDebugTextSimple();

				if (active && debugText != "")
					modifierName += " | " + debugText; 

				ModifierDebugObj obj = new ModifierDebugObj(modifierId, modifierName, active, isLocked);

				modifiers.Insert(obj);
			}
		}
	}
	
	void ResetAll()
	{
		bool hasAreaExposureModifier 	= IsModifierActive(eModifiers.MDF_AREAEXPOSURE);
		bool hasMaksModifier 			= IsModifierActive(eModifiers.MDF_MASK);

		DeactivateAllModifiers();
		
		if (hasAreaExposureModifier)
			ActivateModifier(eModifiers.MDF_AREAEXPOSURE);
		
		if (hasMaksModifier)
			ActivateModifier(eModifiers.MDF_MASK);
	}
}

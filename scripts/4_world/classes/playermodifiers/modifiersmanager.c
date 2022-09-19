//max 32 synced modifiers supported
enum eModifierSyncIDs 
{

	MODIFIER_SYNC_WOUND_INFECT_1 	= 0x00000001,
	MODIFIER_SYNC_WOUND_INFECT_2 	= 0x00000002,
	MODIFIER_SYNC_CONTAMINATION 	= 0x00000004,//stage1 
	MODIFIER_SYNC_CONTAMINATION2 	= 0x00000008,//stage2 and stage3 share the same sync id
	MODIFIER_SYNC_ZONE_EXPOSURE 	= 0x00000010,
	MODIFIER_SYNC_DROWNING 			= 0x00000020,
	//0x00000040,
	//0x00000080,
	//0x00000100,
	//0x00000200,
	//0x00000400,
	//0x00000800,
	//0x00001000,
	LAST_INDEX,
};




enum EActivationType {

	TRIGGER_EVENT_OFF,
	TRIGGER_EVENT_ON_ACTIVATION,
	TRIGGER_EVENT_ON_CONNECT
};

const int DEFAULT_TICK_TIME_ACTIVE = 3;
const int DEFAULT_TICK_TIME_ACTIVE_SHORT = 1;
const int DEFAULT_TICK_TIME_INACTIVE = 3;
const int DEFAULT_TICK_TIME_INACTIVE_LONG = 10;

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
	PlayerBase m_Player;
	ref map<int, ref ModifierBase> m_ModifierList;
	ref array< ref ModifierBase> m_ModifierListArray = new array< ref ModifierBase>;
	ref array<ref Param> m_ParamList;
	bool m_AllowModifierTick = false;
	const int STORAGE_VERSION = 121;
	void ModifiersManager(PlayerBase player)
	{
		m_ModifierList 	= new map<int, ref ModifierBase>;
		m_ParamList 	= new array<ref Param>;
		m_Player 		= player;
		
		Init();
	}

	void Init()
	{
		#ifdef DEVELOPER
		AddModifier(new TestDiseaseMdfr);
		#endif
		AddModifier(new BloodRegenMdfr);
		//AddModifier(new BoneRegen);
		//AddModifier(new Health);
		AddModifier(new SalineMdfr);
		AddModifier(new HealthRegenMdfr);
		AddModifier(new HungerMdfr);
		//AddModifier(new Shock);
		AddModifier(new ImmuneSystemMdfr);
		AddModifier(new StomachMdfr);
		AddModifier(new HeatComfortMdfr);
		AddModifier(new ThirstMdfr);
		AddModifier(new BleedingCheckMdfr);
		//AddModifier(new Blinded);
		//AddModifier(new BrokenArms);
		AddModifier(new BrokenLegsMdfr); // <-- Broken legs are here
		AddModifier(new VomitStuffedMdfr);
		AddModifier(new BurningMdfr);
		AddModifier(new FeverMdfr);
		AddModifier(new HeartAttackMdfr);
		AddModifier(new HemolyticReactionMdfr);
		AddModifier(new PoisoningMdfr);
		AddModifier(new StuffedStomachMdfr);
		//AddModifier(new Tremor);
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
	}

	void SetModifiers(bool enable)
	{
		m_AllowModifierTick = enable;
		
		#ifdef DEVELOPER
		DiagMenu.SetValue(DiagMenuIDs.DM_CHEATS_MODIFIERS_ENABLE, enable);
		#endif
		
		if ( !enable )
		{
			for(int i = 0; i < m_ModifierList.Count(); i++)
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
		modifier.InitBase(m_Player,this);
		int id = modifier.GetModifierID();
		
		if(id < 1)
		{
			Error("modifiers ID must be 1 or higher(for debugging reasons)");			
		}
		
		//TODO: add a check for duplicity
		m_ModifierList.Insert(id, modifier);
		m_ModifierListArray.Insert(modifier);
	}
	
	bool IsModifierActive(eModifiers modifier_id)
	{
		return m_ModifierList.Get(modifier_id).IsActive();
	}

	void OnScheduledTick(float delta_time)
	{
		if(!m_AllowModifierTick) return;

		foreach(ModifierBase m: m_ModifierListArray)
		{
			m.Tick(delta_time);
		}

	}
	
	void DeactivateAllModifiers()
	{
		for(int i = 0; i < m_ModifierList.Count(); i++)
		{
			m_ModifierList.GetElement(i).Deactivate();
		}
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
		
		int modifier_count;
		for(int x = 0; x < m_ModifierList.Count(); x++)
		{
			ModifierBase mdfr = m_ModifierList.GetElement(x);
			if( mdfr.IsActive() && mdfr.IsPersistent() ) 
			{
				modifier_count++;
				//save the modifier id
				CachedObjectsArrays.ARRAY_INT.Insert( mdfr.GetModifierID() );
				if( mdfr.IsTrackAttachedTime() )
				{
					//save the overall attached time
					CachedObjectsArrays.ARRAY_INT.Insert( mdfr.GetAttachedTime() );
				}
			}
		}

		//write the count
		//CachedObjectsParams.PARAM1_INT.param1 = modifier_count;
		//PrintString("Saving modifiers count: "+ modifier_count);
		ctx.Write(modifier_count);

		//write the individual modifiers and respective attached times
		for(int i = 0; i < CachedObjectsArrays.ARRAY_INT.Count(); i++)
		{
			int item = CachedObjectsArrays.ARRAY_INT.Get(i);
			//PrintString( "saving item: "+item );
			ctx.Write(item);
		}

		for (int z = 0; z < m_ParamList.Count(); z++)
		{
			m_ParamList.Get(z).Serialize(ctx);
		}

	}

	bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		int modifier_count;
		if(!ctx.Read(modifier_count))
			return false;
		//PrintString("Loading modifiers count: "+ modifier_count);
		for(int i = 0; i < modifier_count; i++)
		{
			int modifier_id;
			if(!ctx.Read(modifier_id))
				return false;
			//PrintString( "loading item: "+modifier_id );
			//int modifier_id = CachedObjectsParams.PARAM1_INT.param1;
			ModifierBase modifier = GetModifier(modifier_id);
			if( modifier )
			{
				if( modifier.IsTrackAttachedTime() )
				{
					int time;
					if(!ctx.Read(time))//get the attached time
						return false;
					modifier.SetAttachedTime( time );
				}
				
				ActivateModifier(modifier_id, EActivationType.TRIGGER_EVENT_ON_CONNECT);
			}
			else
			{
				Debug.LogError("DB loading: non-existent modifier with id:"+modifier_id.ToString());
			}
		}
		
		for (int x = 0; x < m_ParamList.Count(); x++)
		{
			if(!m_ParamList.Get(x).Deserializer(ctx))
			{
				return false;
			}
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
		for(int i = 1;i < eModifiers.COUNT; i++)
		{
			if(m_ModifierList.Contains(i))
			{
				ModifierBase modifier = m_ModifierList.Get(i);
				int modifier_id = modifier.GetModifierID();
				string modifier_name = modifier.GetName();
				bool active = modifier.IsActive();
				string debug_text = modifier.GetDebugTextSimple();
				bool is_locked = modifier.IsLocked();

				if(active && debug_text != "")
				{
					modifier_name +=" | "+debug_text; 
				}
				ModifierDebugObj obj = new ModifierDebugObj(modifier_id, modifier_name, active, is_locked);
	
				modifiers.Insert( obj );
			}
		}
	}
}

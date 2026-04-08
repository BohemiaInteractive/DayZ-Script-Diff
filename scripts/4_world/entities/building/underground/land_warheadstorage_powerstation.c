class Land_WarheadStorage_PowerStation : House
{
	protected bool m_InitBunkerState;	// if generator is running already, used for initial power setup of related bunker
	protected static ref set<Land_WarheadStorage_Main> 	m_Bunkers;
	protected ref Timer m_UpdateTimer;
	protected PowerGeneratorStatic m_PowerGenerator;
	protected bool m_IsPowerGeneratorRunning;
	protected int m_PlaySparks;
	protected int m_LastActivatedLeverId;
	protected int m_LeverStatesBits;	// is persistent through power generator
	
	protected static ref map<Land_WarheadStorage_PowerStation, Land_WarheadStorage_Main> m_BunkerStationMap = new map<Land_WarheadStorage_PowerStation, Land_WarheadStorage_Main>();
	
	protected const string LEVERS_POS_MEMPOINT 		= "leverup_axis";
	protected const string GENERATOR_POS_MEMPOINT 	= "generator_pos";
	
	protected const string SWITCH_UP_SOUND 		= "Power_Station_Switch_Up_SoundSet";
	protected const string SWITCH_DOWN_SOUND 	= "Power_Station_Switch_Down_SoundSet";
	protected const string SPARKLES_SOUND	 	= "Power_Station_generator_overpowered_SoundSet";
		
	void Land_WarheadStorage_PowerStation()
	{
		m_PlaySparks = 0;
		
		RegisterNetSyncVariableInt("m_PlaySparks", 0, 4);
	}
	
	override void DeferredInit()
	{
		if (g_Game.IsServer())
			LinkPowerGeneratorServer();
		
		g_Game.RegisterNetworkStaticObject(this);
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
				
		if (m_PlaySparks != 0)
		{
			EffectSound sound;
			string mempoint = "lever" + m_PlaySparks.ToString() + "_sparks_pos";
			sound =	SEffectManager.PlaySoundCachedParams(SPARKLES_SOUND, ModelToWorld(GetMemoryPointPos(mempoint)));
			sound.SetAutodestroy(true);
			
			ParticleManager.GetInstance().PlayInWorld(ParticleList.LEVER_SPARKS, ModelToWorld(GetMemoryPointPos(mempoint)));
			
			m_PlaySparks = 0;
		}
	}
	
	vector GetPowerGeneratorSpawnPos()
	{
		vector generatorPosLocal = GetMemoryPointPos(GENERATOR_POS_MEMPOINT);
		return ModelToWorld(generatorPosLocal);
	}

	protected void LinkPowerGeneratorServer()
	{
		m_PowerGenerator = PowerGeneratorStatic.GetClosestGenerator(GetPowerGeneratorSpawnPos(), 5);
			
		if (!m_PowerGenerator)
		{
			int flags = ECE_SETUP | ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH | ECE_NOLIFETIME;

			vector thisOrientation = GetOrientation();
			m_PowerGenerator = PowerGeneratorStatic.Cast(g_Game.CreateObjectEx("PowerGeneratorStatic", GetPowerGeneratorSpawnPos(), flags, RF_IGNORE));
			m_PowerGenerator.SetOrientation(thisOrientation + "180 0 0");
		}

		m_PowerGenerator.SetParent(this);
		
		if (g_Game.IsServer())
			OnAfterLoadUpdate();
		
		//DebugPrepareGenerator();
		
		if (m_PowerGenerator.GetCompEM().IsWorking())
			OnGeneratorStart();//the generator can be already running when we link to it as it has a persistent running state
	}

	PowerGeneratorStatic GetPowerGenerator()
	{
		return m_PowerGenerator;
	}
	
	
	// 'state' -1 is for automatic behaviour ON -> OFF, OFF -> ON, 'state' > -1 forces specific state, 0 for OFF state 1 for ON
	void AnimateLever(int index, int state = -1)
	{
		string leverName = GetLeverComponentNameByLeverIndex(index);
				
		if (!g_Game.IsServer())
			return;

		StartTimer();
		
		float animPhase = state;

		if (state == -1)
		{
			if (GetAnimationPhase(leverName) > 0)
				animPhase = 0;
			else
				animPhase = 1;			
		}

		SetAnimationPhase(leverName, animPhase);
	}
	
	void PlayLeverSound(string leverName, int state)
	{
		EffectSound sound;

		if (state == 0)
			PlaySoundSetAtMemoryPoint(sound, SWITCH_DOWN_SOUND, LEVERS_POS_MEMPOINT, false, 0, 0);
		else if (state == 1)
			PlaySoundSetAtMemoryPoint(sound, SWITCH_UP_SOUND, LEVERS_POS_MEMPOINT, false, 0, 0);
		
		sound.SetAutodestroy(true);
	}
	
	protected void StartTimer()
	{
		if (!m_UpdateTimer)
			m_UpdateTimer = new Timer();
		m_UpdateTimer.Run(0.1, this, "Tick", NULL, true);
	}
	
	protected void CheckStopTimer()
	{
		if (m_UpdateTimer.GetRunTime() > 10)
		{
			m_UpdateTimer = null;
		}
	}
	
	void Tick()
	{
		bool updated = false;
		for (int index = 1; index <= 4; index++)
		{
			float animPhase = GetAnimationPhase(GetLeverComponentNameByLeverIndex(index));
			int bit = 1 << (index - 1);

			if (animPhase == 0)
			{
				if ((bit & m_LeverStatesBits) != 0)
				{
					// in the bitmask, this lever is set as ON, but the anim phase is saying it should be OFF, turn the bit off
					m_LeverStatesBits = m_LeverStatesBits & ~bit;
					updated = true;
				}
			}
			else if (animPhase == 1)
			{
				if ((bit & m_LeverStatesBits) == 0)
				{
					// in the bitmask, this lever is set as OFF, but the anim phase is saying it should be ON, turn the bit on
					m_LeverStatesBits = m_LeverStatesBits | bit;
					updated = true;
					
					m_LastActivatedLeverId = index;
				}
			}
		}
		if (updated)
		{
			m_PowerGenerator.StoreLeverStates(m_LeverStatesBits);
			OnLeverToggled();
			UpdateLeverStatesServer();
		}
		
		CheckStopTimer();	
	}
	
	protected void OnLeverToggled()
	{
		int leversActivatedCount = Math.GetNumberOfSetBits(m_LeverStatesBits);
		if (leversActivatedCount > 1 || (leversActivatedCount > 0 && !m_IsPowerGeneratorRunning))
		{
			//doors are being opened/closed only when we are opening/closing a single door, or when none of the doors are supposed to be open
			//so here, we only force all levers to OFF, and that in turn will make the doors to be closed later on
			
			if (leversActivatedCount > 1 && m_PlaySparks == 0)
			{
				m_PlaySparks = m_LastActivatedLeverId;
				SetSynchDirty();
			}
			
			if (m_IsPowerGeneratorRunning)
			{
				m_PowerGenerator.GetCompEM().SwitchOff();
				m_PowerGenerator.GetCompEM().InteractBranch(m_PowerGenerator);
			}
			
			TurnAllLeversOff();
		}
	}
	
	protected void TurnAllLeversOff()
	{
		string leverName;
		
		for (int i = 1; i < 5; i++)
		{
			leverName = GetLeverComponentNameByLeverIndex(i);
			if (GetAnimationPhase(leverName) > 0)
				AnimateLever(i, 0);	// will update the lever states through tick
		}
	}
	
	
	// 1 through 4
	string GetLeverComponentNameByLeverIndex(int leverIndex)
	{
		switch (leverIndex)
		{
			case 1:
				return "lever1";
			case 2:
				return "lever2";
			case 3:
				return "lever3";
			case 4:
				return "lever4";
		}
		return "lever1";
	}
	
	int GetLeverIndexByComponentName(string name)
	{
		name.ToLower();
		switch (name)
		{
			case "storagedoor1":
				return 1;
			case "storagedoor2":
				return 2;
			case "storagedoor3":
				return 3;
			case "storagedoor4":
				return 4;
		}
		return 0;
	}
		
	protected void UpdateLeverStatesServer()
	{
		Land_WarheadStorage_Main closestBunker = GetClosestBunker();
		if (!closestBunker)
			return;
		
		closestBunker.SetLeverStatesServer(m_LeverStatesBits);
	}
	
	void OnGeneratorStart()
	{
		if (g_Game.IsServer())
		{
			m_IsPowerGeneratorRunning = true;
			
			Land_WarheadStorage_Main closestBunker = GetClosestBunker();
			if (closestBunker)
				closestBunker.SetPowerServer(true);
			
			m_PlaySparks = 0;
			SetSynchDirty();
		}
	}
	
	void OnGeneratorStop()
	{
		if (g_Game.IsServer())
		{
			m_IsPowerGeneratorRunning = false;
			
			TurnAllLeversOff();
			
			Land_WarheadStorage_Main closestBunker = GetClosestBunker();
			
			if (closestBunker)
				closestBunker.SetPowerServer(false);
		}
	}
	
	override void OnAnimationPhaseStarted(string animSource, float phase)
	{
		#ifndef SERVER
		if (animSource.Contains("lever"))
		{
			if (phase > 0)
				PlayLeverSound(animSource, 0);
			else 
				PlayLeverSound(animSource, 1);
		}
		#endif
	}
		
	//--------------------------------------------------------------------------------------------------
	//-------------------------------------- Remote bunker get/set -------------------------------------
	//--------------------------------------------------------------------------------------------------
	
	Land_WarheadStorage_Main GetClosestBunker()
	{
		Land_WarheadStorage_Main closestBunker;
		closestBunker = m_BunkerStationMap.Get(this);
		if (closestBunker)
			return closestBunker;
		
		float smallestDist = float.MAX;
		vector thisPos = GetPosition();
		
		if (!m_Bunkers)
			return null;
		
		foreach (Land_WarheadStorage_Main bunker:m_Bunkers)
		{
			float dist = vector.DistanceSq(bunker.GetPosition(),thisPos);
			if (dist < smallestDist)
			{
				closestBunker = bunker;
				smallestDist =dist;
			}
		}
		
		if (!m_InitBunkerState)
		{
			m_InitBunkerState = true;
			
			if (g_Game.IsServer() && m_PowerGenerator && m_PowerGenerator.GetCompEM().IsWorking())
				closestBunker.SetPowerServer(true);
		}
		
		m_BunkerStationMap.Insert(this, closestBunker);
		
		return closestBunker;
	}
		
	static void RegisterBunker(Land_WarheadStorage_Main bunker)
	{
		if (!m_Bunkers)
		{
			m_Bunkers = new set<Land_WarheadStorage_Main>();
		}
		m_Bunkers.Insert(bunker);
	}
	
	static void UnregisterBunker(Land_WarheadStorage_Main bunker)
	{
		if (m_Bunkers)
		{
			int index = m_Bunkers.Find(bunker);
			if (index != -1)
			{
				m_Bunkers.Remove(index);
			}
			else
			{
				ErrorEx("Attempted to unregister non-registered bunker");
			}
		}
	}
	
	protected void DebugPrepareGenerator()
	{
		if (g_Game.IsServer())
		{
			m_PowerGenerator.GetCompEM().SetEnergy(10000);
			m_PowerGenerator.GetInventory().CreateInInventory("GlowPlug");
		}
	}
			
	// animates levers based on synched lever state from linked generator entity
	void OnAfterLoadUpdate()
	{
		int leverBits = m_PowerGenerator.GetStoredLeverBits();
		
		for (int index = 1; index <= 4; index++)
		{
			int bit = 1 << (index - 1);
			
			if ( (bit & leverBits) != 0 )
			{
				float animPhase = GetAnimationPhase(GetLeverComponentNameByLeverIndex(index));
				if (animPhase == 0)
					AnimateLever(index, 1);			
			}
		}
	}
}
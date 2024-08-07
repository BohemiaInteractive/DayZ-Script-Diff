class Land_WarheadStorage_PowerStation : House
{
	protected static ref set<Land_WarheadStorage_Main> 	m_Bunkers;
	protected ref Timer m_UpdateTimer;
	protected int m_LeverStatesBits;
	protected PowerGeneratorStatic m_PowerGenerator;
	protected bool m_IsPowerGeneratorRunning;
	
	protected const string LEVERS_POS_MEMPOINT 		= "levers";
	protected const string GENERATOR_POS_MEMPOINT 	= "generator_pos";
	
	protected const string SWITCH_UP_SOUND 		= "Power_Station_Switch_Up_SoundSet";
	protected const string SWITCH_DOWN_SOUND 	= "Power_Station_Switch_Down_SoundSet";
	protected const string SPARKLES_SOUND	 	= "Power_Station_generator_overpowered_SoundSet";
		
	override void DeferredInit()
	{
		if (GetGame().IsServer())
			LinkPowerGeneratorServer();
	}
	
	vector GetPowerGeneratorSpawnPos()
	{
		vector generatorPosLocal = GetMemoryPointPos("generator_pos");
		return ModelToWorld(generatorPosLocal);
	}

	protected void LinkPowerGeneratorServer()
	{
		m_PowerGenerator = PowerGeneratorStatic.GetClosestGenerator(GetPowerGeneratorSpawnPos(), 5);
			
		if (!m_PowerGenerator)
		{
			int flags = ECE_SETUP | ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH | ECE_NOLIFETIME;

			vector thisOrientation = GetOrientation();
			m_PowerGenerator = PowerGeneratorStatic.Cast(GetGame().CreateObjectEx("PowerGeneratorStatic", GetPowerGeneratorSpawnPos(), flags, RF_IGNORE));
			m_PowerGenerator.SetOrientation(thisOrientation + "180 0 0");
		}

		m_PowerGenerator.SetParent(this);
		
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
				
		if (!GetGame().IsServer())
			return;
		
		GetGame().RegisterNetworkStaticObject(this);
		
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
	
	void PlayLeverSound(int index, int state = -1)
	{
		string leverName = GetLeverComponentNameByLeverIndex(index);
		EffectSound sound;

		if (state == 0)
			PlaySoundSetAtMemoryPoint(sound, SWITCH_DOWN_SOUND, LEVERS_POS_MEMPOINT, false, 0, 0);
		else if (state == 1)
			PlaySoundSetAtMemoryPoint(sound, SWITCH_UP_SOUND, LEVERS_POS_MEMPOINT, false, 0, 0);
		else 
		{
			if (GetAnimationPhase(leverName) > 0)
				PlaySoundSetAtMemoryPoint(sound, SWITCH_DOWN_SOUND, LEVERS_POS_MEMPOINT, false, 0, 0);
			else
				PlaySoundSetAtMemoryPoint(sound, SWITCH_UP_SOUND, LEVERS_POS_MEMPOINT, false, 0, 0);	
		}
		
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
				}
			}
		}
		if (updated)
			OnLeverToggled();
		
		CheckStopTimer();	
	}
	
	protected void OnLeverToggled()
	{
		int leversActivatedCount = Math.GetNumberOfSetBits(m_LeverStatesBits);
		if (leversActivatedCount > 1 || (leversActivatedCount > 0 && !m_IsPowerGeneratorRunning))
		{
			//doors are being opened/closed only when we are opening/closing a single door, or when none of the doors are supposed to be open
			//so here, we only force all levers to OFF, and that in turn will make the doors to be closed later on
			
			//EffectSound soundSparkles;
			//PlaySoundSetAtMemoryPoint(soundSparkles, SPARKLES_SOUND, GENERATOR_POS_MEMPOINT, false, 0, 0); //will not work - server only method, to be moved
			
			TurnAllLeversOff();
		}
		else
		{
			UpdateDoorsRemotely();
		}
	}
	
	protected void TurnAllLeversOff()
	{
		AnimateLever(1,0);
		AnimateLever(2,0);
		AnimateLever(3,0);
		AnimateLever(4,0);
	}
	
	
	// 1 through 4
	protected string GetLeverComponentNameByLeverIndex(int leverIndex)
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
	
	
	
	protected void UpdateDoorsRemotely()
	{
		Land_WarheadStorage_Main closestBunker = GetClosestBunker();

		if (!closestBunker)
			return;
		
		closestBunker.UpdateDoorState(m_LeverStatesBits);
	}
	
	void OnGeneratorStart()
	{
		m_IsPowerGeneratorRunning = true;
		
		if (GetGame().IsServer())
		{
			Land_WarheadStorage_Main closestBunker = GetClosestBunker();
	
			if (closestBunker)
				closestBunker.SetPowerServer(true);
		}
	}
	
	void OnGeneratorStop()
	{
		m_IsPowerGeneratorRunning = false;
		
		TurnAllLeversOff();
		
		if (GetGame().IsServer())
		{
			Land_WarheadStorage_Main closestBunker = GetClosestBunker();
			
			if (closestBunker)
				closestBunker.SetPowerServer(false);
		}
	}
	
	override void OnAnimationPhaseStarted(string animSource, float phase)
	{
	}
	
	//--------------------------------------------------------------------------------------------------
	//-------------------------------------- Remote bunker get/set -------------------------------------
	//--------------------------------------------------------------------------------------------------
	
	protected Land_WarheadStorage_Main GetClosestBunker()
	{
		float smallestDist = float.MAX;
		Land_WarheadStorage_Main closestBunker;
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
		if (GetGame().IsServer())
		{
			m_PowerGenerator.GetCompEM().SetEnergy(10000);
			m_PowerGenerator.GetInventory().CreateInInventory("GlowPlug");
		}
	}
}
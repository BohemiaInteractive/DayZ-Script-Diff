class OwnershipTestDummyClass
{
	// Simply exists
}

class PMTCreationAndCleanup : PMTF
{
	int m_EventTestManagerID;
	bool m_bTestEventsPassed = false;
	
	int m_OwnershipTestManagerID;
	
	//---------------------------------------------------------------------------
	// Ctor - Decides the tests to run
	//---------------------------------------------------------------------------
	void PMTCreationAndCleanup()
	{
		//AddInitTest("TestInvalidSize");
		AddInitTest("TestCCSB");
		AddInitTest("TestEvents");
		AddInitTest("TestOwnership");
	}
	
	//---------------------------------------------------------------------------
	// Tests
	//---------------------------------------------------------------------------
	// Test invalid size VMEs
	TFResult TestInvalidSize()
	{
		Debug.ParticleLog("Expecting VME: Invalid size. (0)", this, "TestInvalidSize");
		TestCreationSmallBlocking(0, false);
		
		Debug.ParticleLog("Expecting VME: Invalid size. (-3)", this, "TestInvalidSize");
		TestCreationSmallBlocking(-3, false);
		
		return NTFR(TFR.SUCCESS); // No way to check if a VME popped or not...
	}
	
	//---------------------------------------------------------------------------
	// TestCreationCleanupSmallBlocking
	TFResult TestCCSB()
	{
		return TestCleanup("TestMultiCreation", 3000);
	}
	
	//---------------------------------------------------------------------------
	// Test if events are called properly
	TFResult TestEvents()
	{
		ParticleManager pm = new ParticleManager(new ParticleManagerSettings(3000));
		bool success = !pm.IsFinishedAllocating(); // We need it to not be done in the same frame
		if (Assert(success))
		{
			pm.GetEvents().Event_OnAllocationEnd.Insert(PassCheckEvents);
		
			m_EventTestManagerID = InsertManager(pm);
			AddFrameTest("CheckTestEvents");
			
			return NTFR(TFR.SUCCESS);
		}
		
		return NTFR(TFR.FAIL);
	}
	
	//---------------------------------------------------------------------------
	// Test ownership
	TFResult TestOwnership()
	{
		ParticleManager pm = CreatePMFixedBlocking(1);
		bool success = pm.IsFinishedAllocating(); // We need it to be done in the same frame
		if (Assert(success))
		{		
			m_OwnershipTestManagerID = InsertManager(pm);
			OwnershipTestDummyClass dummy = new OwnershipTestDummyClass();
			
			ParticleProperties pp = new ParticleProperties(GetGame().GetPlayer().GetPosition(), ParticlePropertiesFlags.NONE, null, vector.Zero, dummy);
			string particlePath = ParticleList.GetParticleFullPath(ParticleList.EXPLOSION_LANDMINE);
			
			bool result = Assert(pm.CreateParticleByPath(particlePath, pp) != null);
			Debug.ParticleLog("Expecting VME: All particles in pool are already used.", this, "TestOwnership");
			result &= Assert(pm.CreateParticleByPath(particlePath, pp) == null);
			delete dummy;
			result &= Assert(pm.CreateParticleByPath(particlePath, pp) != null);		
			
			return BTFR(result);
		}
		
		return NTFR(TFR.FAIL);
	}
	
	//---------------------------------------------------------------------------
	// OnFrame Checks
	//---------------------------------------------------------------------------
	TFResult CheckTestEvents()
	{
		ParticleManager pm;
		if (GetManager(m_EventTestManagerID, pm))
		{
			if (pm)
			{
				if (pm.IsFinishedAllocating())
				{
					return BTFR(Assert(m_bTestEventsPassed));
				}
			}
			else
			{
				return BTFR(Assert(false));
			}
		}
		else
		{
			return BTFR(Assert(false));
		}
		
		return NTFR(TFR.PENDING);
	}
	
	//---------------------------------------------------------------------------
	// Passes
	//---------------------------------------------------------------------------
	void PassCheckEvents(ParticleManager pm)
	{
		Assert(pm.IsFinishedAllocating());
		m_bTestEventsPassed = true;
	}
	
	//---------------------------------------------------------------------------
	// Helpers
	//---------------------------------------------------------------------------
	TFResult TestCreationSmallBlocking(int size, bool enableAsserts = true)
	{
		// Blocking, so that we can test AllocatedCount
		ParticleManager pm = CreatePMFixedBlocking(size);
		PrintPMStats(pm);
		
		bool success = true;
		
		if (enableAsserts)
		{
			success &= Assert(pm.GetPoolSize() == size);
			success &= Assert(pm.GetAllocatedCount() == size);
			success &= Assert(pm.GetEvents() != null);
		}
		
		return BTFR(success);
	}
	
	TFResult TestCleanup(string f, int p1 = 0)
	{	
		int pmTotal = ParticleManager.GetStaticActiveCount();
		int psTotal = ParticleSource.GetStaticActiveCount();
		
		PrintActiveStats();
		
		TFResult res = CTFR();
		
		GetGame().GameScript.CallFunction(this, f, res, p1);

		int pmTotalPost = ParticleManager.GetStaticActiveCount();
		int psTotalPost = ParticleSource.GetStaticActiveCount();
		
		PrintActiveStats();
		
		bool success = Assert(pmTotal == pmTotalPost);
		success &= Assert(psTotal == psTotalPost);
		
		return res.And(BTFR(success));
	}
	
	TFResult TestMultiCreation(int instances)
	{
		TFResult res = CTFR();
		for (int i = 0; i < instances; ++i)
		{
			res.And(TestCreationSmallBlocking(1));
		}
		return res;
	}
}
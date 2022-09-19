class PMTF : TestFramework
{
	private static int PM_CREATED = 0;
	private ref map<int, ref ParticleManager> m_Managers = new map<int, ref ParticleManager>();
	
	//---------------------------------------------------------------------------
	// Manager management
	//---------------------------------------------------------------------------
	int InsertManager(ParticleManager pm)
	{
		Assert(m_Managers.Insert(PM_CREATED, pm));
		++PM_CREATED;
		
		return PM_CREATED - 1;
	}
	
	bool GetManager(int id, out ParticleManager pm)
	{
		return m_Managers.Find(id, pm);
	}
	
	//---------------------------------------------------------------------------
	// Prints
	//---------------------------------------------------------------------------
	protected void PrintPMStats(notnull ParticleManager pm)
	{
		Debug.ParticleLog(string.Format(
			"Poolsize: %1 | Allocated: %2 | Virtual: %3 | Playing: %4", pm.GetPoolSize(), pm.GetAllocatedCount(), pm.GetVirtualCount(), pm.GetPlayingCount()),
		this, "PrintPMStats", pm);
	}
	
	protected void PrintActiveStats()
	{
		Debug.ParticleLog(string.Format(
			"Active ParticleManagers: %1 | Active ParticleSources: %2", ParticleManager.GetStaticActiveCount(), ParticleSource.GetStaticActiveCount()),
		this, "PrintActiveStats");
	}
	
	//---------------------------------------------------------------------------
	// Helpers
	//---------------------------------------------------------------------------
	protected ParticleManager CreatePMFixedBlocking(int size)
	{
		return new ParticleManager(new ParticleManagerSettings(size, ParticleManagerSettingsFlags.BLOCKING));
	}
}
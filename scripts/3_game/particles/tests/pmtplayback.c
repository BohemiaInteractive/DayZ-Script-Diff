class PMTPlayback : PMTF
{
	ref array<ParticleSource> m_ParticleSources = new array<ParticleSource>();
	
	// TestOnePlaying
	int m_OnePlayingManagerID;
	bool m_bOnePlayingTestSuccess = false;
	
	// TestOnePlayingStandAloneAutoDestroy
	int m_OnePlayingSAADPSID;
	bool m_bOnePlayingSAADEnded = false;
	
	// TestOnePlayingStandAlone
	int m_OnePlayingSAPSID;
	bool m_bOnePlayingSAEnded = false;
	
	// TestStop
	int m_StopPSID;
	float m_StopAccumulatedTime;
	static const float STOP_ACCUMULATED_TIME_STOP_CUTOFF = 2;
	static const float STOP_ACCUMULATED_TIME_PLAY_CUTOFF = 3;
	bool m_bStopWasStopped = false;
	bool m_bStopWasResumed = false;
	bool m_bStopEnded = false;
	
	//---------------------------------------------------------------------------
	// Ctor - Decides the tests to run
	//---------------------------------------------------------------------------
	void PMTPlayback()
	{
		//AddInitTest("TestOnePlaying");
		//AddInitTest("TestOnePlayingStandAloneAutoDestroy");
		//AddInitTest("TestOnePlayingStandAlone");
		//AddInitTest("TestWiggleStress");
		AddInitTest("TestStopping");
	}
	
	//---------------------------------------------------------------------------
	// Tests
	//---------------------------------------------------------------------------
	// Test one particle playing
	TFResult TestOnePlaying()
	{
		ParticleManager pm = CreatePMFixedBlocking(1);
		m_OnePlayingManagerID = InsertManager(pm);

		DayZPlayer player = GetGame().GetPlayer();
		
		ParticleSource p = pm.CreateParticleById(ParticleList.EXPLOSION_LANDMINE, new ParticleProperties(player.GetPosition() + player.GetDirection() * 3, true));
		p.GetEvents().Event_OnParticleEnd.Insert(PassOnePlaying);
		
		AddFrameTest("CheckOnePlaying");

		return BTFR(p.IsParticlePlaying());
	}
	
	//---------------------------------------------------------------------------
	// Test one standalone particle playing which will auto destroy
	TFResult TestOnePlayingStandAloneAutoDestroy()
	{
		DayZPlayer player = GetGame().GetPlayer();
		
		ParticleSource p = ParticleSource.CreateParticle(ParticleList.EXPLOSION_LANDMINE, player.GetPosition() + player.GetDirection() * 3 + player.GetDirectionAside() * 3, true);
		p.GetEvents().Event_OnParticleEnd.Insert(OnePlayingSAADEnded);
		
		m_OnePlayingSAADPSID = m_ParticleSources.Insert(p);
		
		AddFrameTest("CheckOnePlayingSAAD");

		return BTFR(p.IsParticlePlaying());
	}
	
	//---------------------------------------------------------------------------
	// Test one standalone particle playing which will not auto destroy
	TFResult TestOnePlayingStandAlone()
	{
		DayZPlayer player = GetGame().GetPlayer();
		
		ParticleSource p = ParticleSource.CreateParticle(ParticleList.EXPLOSION_LANDMINE, player.GetPosition() + player.GetDirection() * 3 - player.GetDirectionAside() * 3, true);
		p.GetEvents().Event_OnParticleEnd.Insert(OnePlayingSAEnded);
		p.DisableAutoDestroy();
		
		m_OnePlayingSAPSID = m_ParticleSources.Insert(p);
		
		AddFrameTest("CheckOnePlayingSA");

		return BTFR(p.IsParticlePlaying());
	}
	
	//---------------------------------------------------------------------------
	// Test wiggling
	TFResult TestWiggleStress()
	{
		DayZPlayer player = GetGame().GetPlayer();
		
		ParticleSource p = ParticleSource.CreateParticle(ParticleList.ROADFLARE_BURNING_MAIN, player.GetPosition() + player.GetDirection() * 4, true);
		p.SetWiggle(90, 0.1);

		return BTFR(p.IsParticlePlaying());
	}
	
	//---------------------------------------------------------------------------
	// Test stop
	TFResult TestStopping()
	{
		DayZPlayer player = GetGame().GetPlayer();
		
		//ParticleSource p = ParticleSource.CreateParticle(ParticleList.EXPLOSION_LANDMINE, player.GetPosition() + player.GetDirection() * 4, true);
		ParticleSource p = ParticleSource.CreateParticle(ParticleList.EXPLOSION_LANDMINE, vector.Zero, true, player);
		p.GetEvents().Event_OnParticleEnd.Insert(StopEnded);
		p.DisableAutoDestroy();
		
		m_StopPSID = m_ParticleSources.Insert(p);
		
		AddFrameTest("CheckStop");

		return BTFR(p.IsParticlePlaying());
	}
	
	//---------------------------------------------------------------------------
	// OnFrame Checks
	//---------------------------------------------------------------------------
	TFResult CheckOnePlaying()
	{
		ParticleManager pm;
		if (GetManager(m_OnePlayingManagerID, pm))
		{
			if (pm)
			{
				ParticleSource p = pm.GetParticle(0);
				if (p.IsParticlePlaying())
				{
					return NTFR(TFR.PENDING);
				}
				else
				{
					return BTFR(m_bOnePlayingTestSuccess);
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
	}
	
	//---------------------------------------------------------------------------
	TFResult CheckOnePlayingSAAD()
	{
		if (m_ParticleSources.IsValidIndex(m_OnePlayingSAADPSID))
		{
			ParticleSource p = m_ParticleSources[m_OnePlayingSAADPSID];
			if (p)
			{
				if (p.IsParticlePlaying())
				{
					return NTFR(TFR.PENDING);
				}
				else
				{
					if (m_bOnePlayingSAADEnded)
					{
						// There might be one frame where it is still alive before getting cleaned up
						return NTFR(TFR.PENDING);
					}
					else
					{
						// Should be gone when no longer playing
						return BTFR(Assert(false));
					}
				}
			}
			else
			{
				// Make sure the particle ended, if it did, then success!
				return BTFR(m_bOnePlayingSAADEnded);
			}
		}
		else
		{
			return BTFR(Assert(false));
		}
	}
	
	//---------------------------------------------------------------------------
	TFResult CheckOnePlayingSA()
	{
		if (m_ParticleSources.IsValidIndex(m_OnePlayingSAPSID))
		{
			ParticleSource p = m_ParticleSources[m_OnePlayingSAPSID];
			if (p)
			{
				if (p.IsParticlePlaying())
				{
					return NTFR(TFR.PENDING);
				}
				else
				{
					// Clean up the Particle, no leaking from tests!
					GetGame().ObjectDelete(p);
					// Make sure the particle ended, if it did, then success!
					return BTFR(m_bOnePlayingSAEnded);
				}
			}
			else
			{
				// It should not be gone, no autodestroy
				return BTFR(Assert(false));
			}
		}
		else
		{
			return BTFR(Assert(false));
		}
	}
	
	//---------------------------------------------------------------------------
	TFResult CheckStop(float dt)
	{
		m_StopAccumulatedTime += dt;
		
		if (m_ParticleSources.IsValidIndex(m_StopPSID))
		{
			ParticleSource p = m_ParticleSources[m_StopPSID];
			if (p)
			{
				if (p.IsParticlePlaying())
				{
					if (!m_bStopWasStopped && m_StopAccumulatedTime > STOP_ACCUMULATED_TIME_STOP_CUTOFF)
					{
						p.StopParticle();
						m_StopAccumulatedTime = 0;
						m_bStopWasStopped = true;
					}
					
					return NTFR(TFR.PENDING);
				}
				else
				{
					// Means it was stopped before it was supposed to
					// Possibly because particle length is shorter than STOP_ACCUMULATED_TIME_CUTOFF
					if (!Assert(m_bStopWasStopped))
					{
						return BTFR(false);
					}
					else if (!m_bStopWasResumed && m_StopAccumulatedTime > STOP_ACCUMULATED_TIME_PLAY_CUTOFF)
					{
						p.PlayParticle();
						m_bStopWasResumed = true;						
					}
					else if (m_bStopEnded)
					{
						// Clean up the Particle, no leaking from tests!
						GetGame().ObjectDelete(p);
						return BTFR(true);
					}	
					
					return NTFR(TFR.PENDING);						
				}
			}
			else
			{
				// It should not be gone, no autodestroy
				return BTFR(Assert(false));
			}
		}
		else
		{
			return BTFR(Assert(false));
		}
	}
	
	//---------------------------------------------------------------------------
	// Passes
	//---------------------------------------------------------------------------
	void PassOnePlaying(ParticleSource p)
	{
		m_bOnePlayingTestSuccess = true;
	}
	
	//---------------------------------------------------------------------------
	// Helpers
	//---------------------------------------------------------------------------
	void OnePlayingSAADEnded(ParticleSource p)
	{
		m_bOnePlayingSAADEnded = true;
	}

	//---------------------------------------------------------------------------
	void OnePlayingSAEnded(ParticleSource p)
	{
		m_bOnePlayingSAEnded = true;
	}

	//---------------------------------------------------------------------------
	void StopEnded(ParticleSource p)
	{
		m_bStopEnded = true;
	}
}
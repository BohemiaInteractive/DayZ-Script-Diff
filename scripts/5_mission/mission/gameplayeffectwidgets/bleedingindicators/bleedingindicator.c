
//! indicator wrapper for a specific bleeding source.
class BleedingIndicator extends Managed
{
	protected bool m_Initialized;
	protected bool m_Terminating = false; //doesn't spawn more drops and ends when the last one does
	protected bool m_EndNow = false;
	protected bool m_IsRunning = false;
	protected int m_DropSpawnsQueued;
	protected int m_ActiveDropsCount;
	protected int m_Severity;
	protected int m_SourceID; //pairs this with the 'BleedingSource' bit/ID
	protected GameplayEffectsDataBleeding m_ParentMetaData;
	protected array<float> m_DropProbabilityArray;
	
	protected float m_AverageFrequency; //average drops per interval. NOT changeable on the fly, just a helper value!
	protected float m_SequenceTick;
	protected float m_SequenceDuration;
	protected float m_TimeElapsedTotal;
	protected float m_TimeElapsedSequence;
	protected float m_LastDropSpawnTime; //relative to the TOTAL time, not the sequence!
	protected float m_DropSpawnMinDelay;
	protected float m_DropSpawnMaxDelay;
	protected int m_CurrentDropProbabilityStep;
	protected int m_DropProbabilityRollsCount;
	protected vector m_BasePosition;
	
	ref set<ref BleedingIndicatorDropData> m_ActiveDrops;
	ref set<int> m_CleanupQueue;
	
	void BleedingIndicator(int source_ID, int severity, GameplayEffectsDataBleeding parent)
	{
		m_Initialized = false;
		m_SourceID = source_ID;
		m_Severity = severity;
		m_ParentMetaData = parent;
		m_CurrentDropProbabilityStep = 0;
		m_ActiveDrops = new set<ref BleedingIndicatorDropData>;
		m_CleanupQueue = new set<int>;
		m_DropProbabilityArray = m_ParentMetaData.GetProbabilities(m_Severity);
		m_DropProbabilityRollsCount = m_DropProbabilityArray.Count();
		
		switch (m_Severity)
		{
			case BleedingIndicationConstants.INDICATOR_SEVERITY_LOW:
			{
				m_SequenceDuration = BleedingIndicationConstants.SEQUENCE_DURATION_LOW;
				m_AverageFrequency = BleedingIndicationConstants.SEQUENCE_DROP_AVERAGE_LOW;
				m_DropSpawnMinDelay = BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MIN_LOW;
				m_DropSpawnMaxDelay = BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MAX_LOW;
				break;
			}
			case BleedingIndicationConstants.INDICATOR_SEVERITY_MEDIUM:
			{
				m_SequenceDuration = BleedingIndicationConstants.SEQUENCE_DURATION_MEDIUM;
				m_AverageFrequency = BleedingIndicationConstants.SEQUENCE_DROP_AVERAGE_MEDIUM;
				m_DropSpawnMinDelay = BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MIN_MEDIUM;
				m_DropSpawnMaxDelay = BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MAX_MEDIUM;
				break;
			}
			case BleedingIndicationConstants.INDICATOR_SEVERITY_HIGH:
			{
				m_SequenceDuration = BleedingIndicationConstants.SEQUENCE_DURATION_HIGH;
				m_AverageFrequency = BleedingIndicationConstants.SEQUENCE_DROP_AVERAGE_HIGH;
				m_DropSpawnMinDelay = BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MIN_HIGH;
				m_DropSpawnMaxDelay = BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MAX_HIGH;
				break;
			}
			
			default:
			{
				m_AverageFrequency = BleedingIndicationConstants.SEQUENCE_DROP_AVERAGE_LOW;
				m_DropSpawnMinDelay = BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MIN_LOW;
				m_DropSpawnMaxDelay = BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MAX_LOW;
				Debug.Log("Unknown severity value!");
			}
#ifdef DIAG_DEVELOPER
			if (DbgBleedingIndicationStaticInfo.m_DbgUseOverrideValues)
			{
				m_SequenceDuration = DbgBleedingIndicationStaticInfo.m_DbgSequenceDuration;
				m_DropSpawnMinDelay = DbgBleedingIndicationStaticInfo.m_DbgDropMinDelay;
				m_DropSpawnMaxDelay = DbgBleedingIndicationStaticInfo.m_DbgDropMaxDelay;
			}
#endif
		}
		
		m_TimeElapsedTotal = 0;
		m_TimeElapsedSequence = 0;
	}
	
	void InitIndicator(vector position)
	{
		m_BasePosition = position;
		ResetIndicator();
		m_Initialized = true;
	}
	
	void StopIndicator(bool instant = false)
	{
		if (!m_Terminating)
		{
			m_IsRunning = false;
			
			if (instant)
			{
				m_EndNow = true;
			}
			m_Terminating = true;
		}
	}
	
	protected void StartRunningDrops()
	{
		m_ActiveDropsCount = 0;
		m_IsRunning = true;
		TrySpawnNextDrop();
		m_DropSpawnsQueued = 0;
		
		m_CurrentDropProbabilityStep = (int)(m_DropProbabilityRollsCount - (m_DropProbabilityRollsCount / m_AverageFrequency));
	}
	
	//! Are any drops currently being animated?
	protected bool IsRunningDrops()
	{
		return m_ActiveDropsCount > 0;
	}
	
	void TrySpawnNextDrop()
	{
		ImageWidget dropImage;
		if (Class.CastTo(dropImage,m_ParentMetaData.GetNextDropImage()) && !dropImage.IsVisible()) //IsVisible false means the drop is free to be used
		{
			BleedingIndicatorDropData data = new BleedingIndicatorDropData(dropImage,m_Severity);
			data.SetBasePosition(m_BasePosition);
			data.StartDrop();
			
			m_ActiveDrops.Insert(data);
			m_ActiveDropsCount++;
		}
		
		m_LastDropSpawnTime = m_TimeElapsedTotal;
		m_DropSpawnsQueued--;
	}
	
	protected void ResetSequence()
	{
		m_CurrentDropProbabilityStep = 0;
		m_TimeElapsedSequence = 0;
		m_DropSpawnsQueued = 0;
	}
	
	void ResetIndicator()
	{
		m_Terminating = false;
		m_EndNow = false;
		m_TimeElapsedTotal = 0;
		m_CurrentDropProbabilityStep = 0;
		m_TimeElapsedSequence = 0;
		m_LastDropSpawnTime = 0;
		m_DropSpawnsQueued = 0;
	}
	
	void Update(float timeSlice)
	{
		if ( !m_Initialized )
		{
			return;
		}
		
#ifdef DEVELOPER
		if (DbgBleedingIndicationStaticInfo.m_DbgUseOverrideValues)
		{
			m_SequenceDuration = DbgBleedingIndicationStaticInfo.m_DbgSequenceDuration;
		}
#endif
		m_SequenceTick = m_SequenceDuration / m_DropProbabilityRollsCount;
		
		//run drops, if possible
		if (!m_Terminating)
		{
			if (m_IsRunning)
			{
				if (m_TimeElapsedSequence > (m_SequenceTick * m_CurrentDropProbabilityStep))
				{
					if (m_CurrentDropProbabilityStep < m_DropProbabilityRollsCount)
					{
						float rnd = Math.RandomFloat01();
						if (rnd < m_DropProbabilityArray[m_CurrentDropProbabilityStep])
						{
							m_DropSpawnsQueued++;
						}
						
						m_CurrentDropProbabilityStep++;
					}
				}
				
				float sinceLastDrop = m_TimeElapsedTotal - m_LastDropSpawnTime;
				if (m_TimeElapsedSequence > m_SequenceDuration)
				{
					ResetSequence();
				}
				else if (m_DropSpawnsQueued > 0) //spawn queued drop
				{
					if (sinceLastDrop >= m_DropSpawnMinDelay)
					{
						TrySpawnNextDrop();
					}
				}
				else if (sinceLastDrop > m_DropSpawnMaxDelay)
				{
					TrySpawnNextDrop(); //guaranteed drop
					m_CurrentDropProbabilityStep++; //substitutes a regular roll..
				}
			}
			else //1st drop ignores delay
			{
				StartRunningDrops();
			}
		}
		
		for (int i = 0; i < m_ActiveDropsCount; i++)
		{
			if (!m_EndNow)
			{
				//Simulate drops
				m_ActiveDrops[i].Update(timeSlice);
			}
			else
			{
				m_ActiveDrops[i].StopDrop();
			}
			
			if (!m_ActiveDrops[i].IsRunning())
			{
				m_CleanupQueue.Insert(i);
			}
		}
		
		//Cleanup drops
		for (i = m_CleanupQueue.Count() - 1; i >= 0; i--)
		{
			m_ActiveDrops.Remove(m_CleanupQueue[i]);
			m_ActiveDropsCount--;
		}
		m_CleanupQueue.Clear();
		
		if (m_Terminating && !IsRunningDrops())
		{
			m_EndNow = true;
		}
		
		m_TimeElapsedTotal += timeSlice;
		m_TimeElapsedSequence += timeSlice;
	}
	
	bool GetEndNow()
	{
		return m_EndNow;
	}
	
	int GetSeverity()
	{
		return m_Severity;
	}
}

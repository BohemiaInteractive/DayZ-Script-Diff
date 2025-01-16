class RainProcurementHandler : Managed
{
	protected MissionBaseWorld m_Mission;
	protected ref set<RainProcurementComponentBase> m_ActiveComponents;
	protected ref set<RainProcurementComponentBase> m_ChangedQueue;
	protected ref set<int> m_CleanupQueue;
	protected const int UPDATE_BATCH_SIZE = 20; //Tweak this to adjust max batch size
	const int UPDATE_TIME = 10; //seconds
	protected bool m_Update;
	protected bool m_ProcessComponents;
	protected bool m_ProcessingFinished;
	protected int m_NextToProcessIdx;
	protected float m_UpdateTimer;
	protected float m_LiquidAmountCoef;
	
	void RainProcurementHandler(MissionBaseWorld mission)
	{
		m_Mission = mission;
		m_Update = false;
		m_ProcessComponents = false;
		m_ProcessingFinished = true;
		m_ActiveComponents = new set<RainProcurementComponentBase>;
		m_ChangedQueue = new set<RainProcurementComponentBase>;
		m_CleanupQueue = new set<int>;
		m_NextToProcessIdx = 0;
		m_UpdateTimer = 0;
	}
	
	void QueueStart(RainProcurementComponentBase component)
	{
		m_ChangedQueue.Insert(component);
		m_Update = true;
	}
	
	void QueueStop(RainProcurementComponentBase component)
	{
		m_ChangedQueue.Insert(component);
		m_Update = true;
	}
	
	void Update(float timeslice)
	{
		if (!m_Update)
			return;
		
		if (m_ProcessComponents)
		{
			if (m_ProcessingFinished) //do on start and after the batch is finished
			{
				m_LiquidAmountCoef = DetermineAmountCoef();
				if (m_LiquidAmountCoef == 0) //skip processing when not raining
				{
					Reset();
					return;
				}
			}
			
			m_ProcessingFinished = ProcessBatch();
			
			if (m_ProcessingFinished)
			{
				Reset();
			}
		}
		else
		{
			m_UpdateTimer += timeslice;
			if (m_UpdateTimer >= UPDATE_TIME)
			{
				HandleChangedComponents();
				Cleanup();
				CheckUpdating();
				m_ProcessComponents = m_Update;
				if (!m_Update)
					Reset();
			}
		}
	}
	
	//! returns 'true' when all the batches are finished
	protected bool ProcessBatch()
	{
		bool ret = false;
		int count = m_ActiveComponents.Count();
		int target = (int)Math.Clamp(m_NextToProcessIdx + UPDATE_BATCH_SIZE,0,count);
		
		for (m_NextToProcessIdx; m_NextToProcessIdx < target; m_NextToProcessIdx++)
		{
			if (m_ActiveComponents[m_NextToProcessIdx])
				m_ActiveComponents[m_NextToProcessIdx].OnUpdate(m_UpdateTimer, m_LiquidAmountCoef);
			else
				m_CleanupQueue.Insert(m_NextToProcessIdx);
		}
		
		ret = target == count;
		if (ret)
		{
			m_NextToProcessIdx = 0;
		}
		
		return ret;
	}
	
	protected void Cleanup()
	{
		int count = m_CleanupQueue.Count();
		if (count == 0)
			return;
		
		for (int i = count - 1; i > -1; i--)
		{
			m_ActiveComponents.Remove(m_CleanupQueue[i]);
		}
		m_CleanupQueue.Clear();
	}
	
	protected void Reset()
	{
		m_ProcessComponents = false;
		m_ProcessingFinished = true;
		m_UpdateTimer = 0;
	}
	
	protected void HandleChangedComponents()
	{
		RainProcurementComponentBase component;
		int count = m_ChangedQueue.Count();
		int idx;
		
		for (int  i = 0; i < count; i++)
		{
			component = m_ChangedQueue[i];
			if (!component)
				continue;
			
			if (component.IsActive())
			{
				m_ActiveComponents.Insert(component);
			}
			else
			{
				idx = m_ActiveComponents.Find(component);
				if (idx != -1)
					m_ActiveComponents.Remove(idx);
			}
		}
		m_ChangedQueue.Clear();
	}
	
	protected void CheckUpdating()
	{
		m_Update = m_ActiveComponents.Count() != 0;
	}
	
	float GetLiquidAmountCoef()
	{
		return m_LiquidAmountCoef;
	}
	
	//! scalable on handler level. Now also handles SNOW, total value can exceed 1.0
	float DetermineAmountCoef()
	{
		return GetGame().GetWeather().GetRain().GetActual() + GetGame().GetWeather().GetSnowfall().GetActual();
	}
};
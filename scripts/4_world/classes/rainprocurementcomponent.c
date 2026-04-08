class RainProcurementComponentBase : Managed
{
	protected ItemBase 					m_ProcuringItem;	
	protected RainProcurementHandler 	m_Handler;
	protected bool 						m_IsUnderRoof;
	protected bool 						m_IsActive;
	protected int  						m_UpdateCounter;
	protected const int 				UPDATE_ROOFCHECK_COUNT = 3; //do roofcheck every n updates
	
	void RainProcurementComponentBase(ItemBase procuringItem)
	{
		m_ProcuringItem = procuringItem;
		Reset();
		m_Handler = MissionBaseWorld.Cast(g_Game.GetMission()).GetRainProcurementHandler();
	}
	
	//! Called on server to queue rain procurement (on next cycle end)
	void StartRainProcurement()
	{
		#ifdef SERVER
		Reset();
		SetActive(true);
		m_Handler.QueueStart(this);
		#endif
	}
	
	//! Called on server to queue rain procurement removal (on next cycle end)
	void StopRainProcurement()
	{
		#ifdef SERVER
		if (IsActive())
		{
			SetActive(false);
			m_Handler.QueueStop(this);
		}
		#endif
	}
	
	void OnUpdate(float deltaTime, float amount)
	{
		m_UpdateCounter++;
		if (m_UpdateCounter == 0 || m_UpdateCounter == UPDATE_ROOFCHECK_COUNT)
		{
			m_UpdateCounter = 0;
			UpdateIsUnderRoof();
		}
		ProcureLiquid(amount);
	}
	
	protected void Reset()
	{
		m_UpdateCounter = -1;
	}
	
	protected void ProcureLiquid(float amountBase, int liquidType = LIQUID_CLEANWATER)
	{
		if (!m_IsUnderRoof)
		{
			float actualAmount = amountBase * GetBaseLiquidAmount();
			Liquid.FillContainerEnviro(m_ProcuringItem, liquidType, actualAmount);
		}
	}
	
	protected void UpdateIsUnderRoof() 
	{
		m_IsUnderRoof = MiscGameplayFunctions.IsUnderRoof(m_ProcuringItem);
	}
	
	//! override this to get different amount per ProcureLiquid cycle
	float GetBaseLiquidAmount()
	{
		return GameConstants.LIQUID_RAIN_AMOUNT_COEF_BASE;
	}
	
	bool IsActive()
	{
		return m_IsActive;
	}
	
	void SetActive(bool run)
	{
		//resets times on start
		if (run)
			Reset();
		m_IsActive = run;
	}
};

class RainProcurementComponentBarrel : RainProcurementComponentBase {};
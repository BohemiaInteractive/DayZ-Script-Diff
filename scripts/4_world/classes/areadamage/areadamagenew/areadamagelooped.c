class AreaDamageLooped : AreaDamageManager
{
	//! Current start time in seconds
	protected float				m_CurrentTime = 0.0;
	//! Previous start time in seconds
	protected float				m_PreviousTime = 0.0;	
	//! How much time has accumulated
	protected float 			m_AccumulatedTime = 0.0;	
	//! Loop interval in seconds
	protected float				m_LoopInterval = 1.0;
	
	//! Caching of the amount of loops that will be performed in this frame
	protected int 				m_AmountOfLoops = 0;
	
	//! Decides if the looping will be using the Object as reference or the time since last update loop
	protected bool				m_LoopByObject = true;
	
	void AreaDamageLooped(EntityAI parent, bool loopByObject = true)
	{
		m_LoopByObject = loopByObject;
	}
	
	//! Gets called when the trigger is spawned, so is the start and also a reset
	override void OnTriggerCreated()
	{
		super.OnTriggerCreated();
		
		m_PreviousTime = g_Game.GetTickTime();
		m_AccumulatedTime = 0.0;
	}
	
	override void SetLoopInterval( float time )
	{
		m_LoopInterval = time;
	}
	
	override void OnEnterServerEvent(TriggerInsider insider)
	{
		super.OnEnterServerEvent(insider);
		
		// Do direct damaging first time on entering
		if (m_LoopByObject)
			OnEvaluateDamageEx(insider, 1);
	}
	
	override void OnStayStartServerEvent(int nrOfInsiders)
	{
		super.OnStayStartServerEvent(nrOfInsiders);
		
		m_CurrentTime = g_Game.GetTickTime();		
		m_AccumulatedTime += m_CurrentTime - m_PreviousTime;		
		m_PreviousTime = m_CurrentTime;
		
		m_AmountOfLoops = m_AccumulatedTime / m_LoopInterval;
	}
	
	override void OnStayServerEvent(TriggerInsider insider, float deltaTime)
	{
		super.OnStayServerEvent(insider, deltaTime);
		
		// As we are using the Ex variant, we can put the logic in CalculateDamageScale
		OnEvaluateDamageEx(insider, deltaTime);
	}
	
	override void OnStayFinishServerEvent()
	{
		super.OnStayFinishServerEvent();
		
		m_AccumulatedTime -= m_AmountOfLoops * m_LoopInterval;
	}
	
	// TODO: EMH: OnLeaveServerEvent -> perform the remaining buffered damage to be done?
	
	override protected float CalculateDamageScale(TriggerInsider insider, float deltaTime)
	{	
		// If we want the LoopInterval to be dependant on the Object, we will need their lastDamaged time
		if (m_LoopByObject)
		{
			float lastDamaged = 0;
			
			AreaDamageTriggerInsider dInsider;
			if ( CastTo( dInsider, insider ) )
				lastDamaged = dInsider.lastDamaged;
			
			// First time it is damaged, do full damage
			if (lastDamaged == 0)
				return 1;
			
			// This way, in case the server lags, it will scale the damage to catch up if necessary
			// We want to wait until it is at least above 1, to not spam the network with hit messages
			float damageCoeff = (m_CurrentTime - lastDamaged) / m_LoopInterval;
			if ( damageCoeff >= 1 )
				return damageCoeff;
			
			return 0;
		}
		else
		{
			return m_AmountOfLoops;
		}
	}
}
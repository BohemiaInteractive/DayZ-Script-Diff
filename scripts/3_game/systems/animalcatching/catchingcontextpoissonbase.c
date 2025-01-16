class CatchingContextPoissonBase : CatchingContextBase
{
	protected float m_SignalPoissonMean; //usually, only the smallest value is considered
	protected int m_SignalCurrent;
	
	override protected void ClearCatchingItemData()
	{
		super.ClearCatchingItemData();
		
		m_SignalPoissonMean = AnimalCatchingConstants.POISSON_CYCLE_MEAN_DEFAULT;
		m_SignalCurrent = 0;
	}
	
	float GetSignalPoissonMean()
	{
		return m_SignalPoissonMean;
	}
	
	//! maximum number of signals in a single cycle
	int GetSignalMax()
	{
		//m_SignalPoissonMean * 2 + 1
		return Math.Ceil(m_SignalPoissonMean) + Math.Ceil(m_SignalPoissonMean/5); //TODO?
	}
	
	override bool ModifySignalProbability(inout float probability)
	{
		if (m_SignalCurrent > GetSignalMax() || m_SignalCurrent > 12) //necessary reset
			m_SignalCurrent = 0;
		
		probability = Math.Poisson(m_SignalPoissonMean,m_SignalCurrent) * GetChanceCoef();
		
		return true;
	}
	
	override bool RollCatch()
	{
		bool ret = super.RollCatch();
		m_SignalCurrent++;
		
		return ret;
	}
	
	float GetChanceCoef()
	{
		return 1.0;
	}
}
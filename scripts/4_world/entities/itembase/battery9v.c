class Battery9V extends ItemBase
{
	private int				m_Efficiency0To10; // Synchronized variable
	static private float 	m_EfficiencyDecayStart = 0.1; // At this % of maximum energy the output of the battery starts to weaken.
	
	void Battery9V()
	{
		m_Efficiency0To10 = 10;
		RegisterNetSyncVariableInt("m_Efficiency0To10");
	}
	
	//Update Battery energy level before it overrides quantity
	override void SetCEBasedQuantity()
	{
		super.SetCEBasedQuantity();
		if ( GetCompEM() )
			GetCompEM().SetEnergy( GetCompEM().GetEnergyMax() * ( GetQuantity() / GetQuantityMax() ) );
	}
	
	//! Returns efficiency of this battery. The value is synchronized from server to all clients and is accurate down to 0.1 units.
	float GetEfficiency0To1()
	{
		return m_Efficiency0To10 / 10;
	}
	
	//! Returns efficiency of this battery. The value is synchronized from server to all clients and is accurate down to 0.1 unit.
	float GetEfficiencyDecayStart()
	{
		return m_EfficiencyDecayStart;
	}
	
	
	override void OnEnergyConsumed()
	{
		super.OnEnergyConsumed();
		
		if ( GetGame().IsServer() )
		{
			float energy_coef = GetCompEM().GetEnergy0To1();
			
			if ( energy_coef < m_EfficiencyDecayStart  &&  m_EfficiencyDecayStart > 0 )
			{
				m_Efficiency0To10 = Math.Round(  (energy_coef / m_EfficiencyDecayStart) * 10  );
				SetSynchDirty();
			}
		}
	}
	
	// Not needed right now, but it will be useful if we add rechargable batteries.
	override void OnEnergyAdded()
	{
		super.OnEnergyAdded();
		
		if ( GetGame().IsServer() )
		{
			float energy_coef = GetCompEM().GetEnergy0To1();
			
			if ( energy_coef < m_EfficiencyDecayStart  &&  m_EfficiencyDecayStart > 0)
			{
				m_Efficiency0To10 = Math.Round(  (energy_coef / m_EfficiencyDecayStart) * 10  );
				SetSynchDirty();
			}
			else
			{
				m_Efficiency0To10 = 10;
				SetSynchDirty();
			}
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionMeasureBattery);
	}
}
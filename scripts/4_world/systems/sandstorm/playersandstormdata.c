class PlayerSandstormData
{
	// State bit flags - using bitmap for boolean states
	private static const int STATE_WAS_IN_VEHICLE = 1;	 // Bit 0
	private static const int STATE_WAS_UNDER_ROOF = 2;	 // Bit 1  
	private static const int STATE_WAS_IN_INTERIOR = 4;	 // Bit 2
	
	// Combined state storage
	protected int m_StateBits = 0;					// All boolean states in one integer
	float m_VehicleTransitionTimer = 0.0;			// Vehicle transition timer
	float m_InteriorTransitionTimer = 0.0;			// Interior transition timer
	float m_UndergroundTransitionTimer = 0.0;		// Underground transition timer
	
	// Helper methods for state management
	protected bool GetState(int stateBit)
	{
		return (m_StateBits & stateBit) != 0;
	}
	
	protected void SetState(int stateBit, bool value)
	{
		if (value)
			m_StateBits |= stateBit;   // Set bit
		else
			m_StateBits &= ~stateBit;  // Clear bit
	}
	
	bool GetWasInVehicle() 
	{
		return GetState(STATE_WAS_IN_VEHICLE);
	}
	
	void SetWasInVehicle(bool value) 
	{
		SetState(STATE_WAS_IN_VEHICLE, value);
	}
	
	bool GetWasUnderRoof() 
	{ 
		return GetState(STATE_WAS_UNDER_ROOF);
	}
	
	void SetWasUnderRoof(bool value) 
	{
		 SetState(STATE_WAS_UNDER_ROOF, value);
	}
	
	bool GetWasInInterior() 
	{ 
		return GetState(STATE_WAS_IN_INTERIOR); 
	}
	
	void SetWasInInterior(bool value) 
	{
		SetState(STATE_WAS_IN_INTERIOR, value);
	}
}
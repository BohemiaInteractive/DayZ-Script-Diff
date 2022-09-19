class RemotelyActivatedItemBehaviour
{
	protected EntityAI		m_Parent;
	protected EntityAI		m_PairDevice;
	protected int			m_PairDeviceNetIdLow;
	protected int			m_PairDeviceNetIdHigh;

	void RemotelyActivatedItemBehaviour(notnull EntityAI pParent)
	{
		m_Parent = pParent;
		m_PairDeviceNetIdLow = -1;
		m_PairDeviceNetIdHigh = -1;
	}
	
	void OnVariableSynchronized()
	{
		Pair();
	}
	
	void Pair()
	{
		EntityAI device = EntityAI.Cast(GetGame().GetObjectByNetworkId(GetPairDeviceNetIdLow(), GetPairDeviceNetIdHigh()));
		if (device)
		{
			Pair(device);
		}
	}

	void Pair(notnull EntityAI device)
	{
		m_PairDevice = device;
		SetPairDeviceNetIds(device);
		
		if (!m_Parent.GetPairDevice() || m_Parent.GetPairDevice() != m_PairDevice)
		{
			m_PairDevice.PairRemote(m_Parent);
		}

		m_PairDevice.SetSynchDirty();
		m_Parent.SetSynchDirty();
	}
	
	void Unpair()
	{
		m_PairDeviceNetIdLow 	= -1;
		m_PairDeviceNetIdHigh 	= -1;

		if (m_PairDevice)
		{
			m_PairDevice.SetSynchDirty();
			m_PairDevice = null;
		}

		m_Parent.SetSynchDirty();
	}
	
	EntityAI GetPairDevice()
	{
		return m_PairDevice;
	}
	
	bool IsPaired()
	{
		return m_PairDevice != null;
	}
	
	void SetPairDeviceNetIds(notnull EntityAI device)
	{
		device.GetNetworkID(m_PairDeviceNetIdLow, m_PairDeviceNetIdHigh);
	}
	
	int GetPairDeviceNetIdLow()
	{
		return m_PairDeviceNetIdLow;
	}

	int GetPairDeviceNetIdHigh()
	{
		return m_PairDeviceNetIdHigh;
	}
}

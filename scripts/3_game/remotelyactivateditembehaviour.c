class RemotelyActivatedItemBehaviour
{
	protected EntityAI		m_Parent;
	protected bool			m_IsTrigger;
	protected EntityAI		m_PairDevice;
	protected int			m_PairDeviceNetIdLow;
	protected int			m_PairDeviceNetIdHigh;
	protected int			m_PersistentPairID = int.MIN;
	
	protected static ref map<int,EntityAI> m_RemoteReceivers = new map<int,EntityAI>();

	void RemotelyActivatedItemBehaviour(notnull EntityAI pParent)
	{
		m_Parent = pParent;

		m_PairDeviceNetIdLow 	= -1;
		m_PairDeviceNetIdHigh 	= -1;
	}
	
	void ~RemotelyActivatedItemBehaviour()
	{
		if (m_Parent)
			Unpair();
	}
	
	void SetTrigger()
	{
		m_IsTrigger = true;
	}
	
	void OnVariableSynchronized()
	{
		Pair();
	}
	
	void OnStoreSave(ParamsWriteContext ctx)
	{
		ctx.Write(m_PersistentPairID);
	}	
	
	bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!ctx.Read(m_PersistentPairID))
			return false;

		if (m_PersistentPairID == int.MIN)//default value, no point in going further
			return true;

		if (m_IsTrigger)//trigger
		{
			EntityAI receiver = m_RemoteReceivers.Get(m_PersistentPairID);
			if (receiver)
				Pair(receiver);
		}
		else // remotely controlled device
		{
			m_RemoteReceivers.Insert(m_PersistentPairID, m_Parent);//receivers will register themselves upon being loaded from the storage
		}

		return true;
	}
	
	void OnAfterLoad()
	{
		if (m_IsTrigger && m_PersistentPairID != int.MIN && !m_PairDevice)
		{
			EntityAI receiver = m_RemoteReceivers.Get(m_PersistentPairID);
			if (receiver)
			{
				//if both the receiver and trigger are somewhere in the world outside of the player inventory, there is no guarantee 
				//that the pairing is going to be succesful during the 'OnStoreLoad' event as that requires the receiver to already be loaded when the trigger is being loaded
				//therefore, it's necessary to also perform pairing in this event, which happens at the end of the storage loading process
				//do note that this event is not called when the entity is being loaded as part of the player inventory, therefore, we need to pair both in 'OnStoreLoad' and here to handle both situations
				//(when the trigger is in the player's inventory, it always loads correctly after the receiver, which is always in the world)
				Pair(receiver);
			}
		}
	}
	
	
	static int GeneratePersistentID()
	{
		int randomID = Math.RandomInt(0, int.MAX);
		if (m_RemoteReceivers.Contains(randomID))
		{
			//it's very unlikely to have a collision here, but lets handle it anyway
			return GeneratePersistentID();
		}
		else
			return randomID;
	}
	
	void SetPersistentPairID(int id)
	{
		m_PersistentPairID = id;
		
		if (!m_IsTrigger)
			m_RemoteReceivers.Insert(id,m_Parent);
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
		
		if (device != m_Parent && (!m_Parent.GetPairDevice() || m_Parent.GetPairDevice() != m_PairDevice))
			m_PairDevice.PairRemote(m_Parent);

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
		
		if (m_PersistentPairID != int.MIN)
		{
			if (m_RemoteReceivers.Contains(m_PersistentPairID))
				m_RemoteReceivers.Remove(m_PersistentPairID);
		}

		m_PersistentPairID = int.MIN;
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

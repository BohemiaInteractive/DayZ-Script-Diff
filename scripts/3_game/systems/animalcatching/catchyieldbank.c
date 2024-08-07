typedef map<int,ref YieldItemBase> YieldsMap; //<<typename hash, yield data>> //TODO: why hash? Evaluate

class CatchYieldBank
{
	protected ref YieldsMap m_AllYieldsMap;
	private ref array<int> m_OrderedHashes; //for sync purposes
	
	void CatchYieldBank()
	{
		Init();
	}
	
	protected void Init()
	{
		m_AllYieldsMap = new YieldsMap();
		m_OrderedHashes = new array<int>;
	}
	
	YieldsMap GetYieldsMap()
	{
		return m_AllYieldsMap;
	}
	
	void RegisterYieldItem(YieldItemBase data)
	{
		data.SetRegistrationIdx(m_OrderedHashes.Insert(data.GetType().Hash()));
		m_AllYieldsMap.Set(data.GetType().Hash(),data);
	}
	
	YieldItemBase GetYieldItemByIdx(int idx)
	{
		return m_AllYieldsMap.Get(m_OrderedHashes[idx]);
	}
}


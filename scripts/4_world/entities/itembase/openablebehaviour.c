class OpenableBehaviour
{
	protected bool m_IsOpened;
	
	void OpenableBehaviour(bool bState = true)
	{
		m_IsOpened = bState; //! initial value
	}

	void Open()
	{
		SetState(true);
	}

	void Close()
	{
		SetState(false);
	}
	
	bool IsOpened()
	{
		return GetState();
	}
	
	protected void SetState(bool bState)
	{
		m_IsOpened = bState;
	}
	
	protected bool GetState()
	{
		return m_IsOpened;
	}
}
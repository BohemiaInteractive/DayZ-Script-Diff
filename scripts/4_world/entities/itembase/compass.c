class Compass : ItemCompass
{
	protected ref OpenableBehaviour m_Openable;
	
	void Compass()
	{
		m_Openable = new OpenableBehaviour(false);
		
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
		
		UpdateVisualState();
	}

	override void Open()
	{
		m_Openable.Open();
		SetSynchDirty();

		UpdateVisualState();
	}

	override void Close()
	{
		m_Openable.Close();
		SetSynchDirty();

		UpdateVisualState();
	}
	
	override bool IsOpen()
	{
		return m_Openable.IsOpened();
	}

	protected void UpdateVisualState()
	{
		if ( IsOpen() )
		{
			SetAnimationPhase("cover", 1.0);
		}
		else
		{
			SetAnimationPhase("cover", 0.0);
		}
	}
	
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        UpdateVisualState();
    }
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
		AddAction(ActionClose);
		AddAction(ActionViewCompass);
	}
}

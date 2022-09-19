class TelescopicBaton : ItemBase
{
	protected int m_MeleeMode;
	protected int m_MeleeHeavyMode;
	protected int m_MeleeSprintMode;
	
	protected ref OpenableBehaviour m_Openable;
	
	void TelescopicBaton()
	{
		//!set default melee modes on init
		m_MeleeMode = 0;
		m_MeleeHeavyMode = 1;
		m_MeleeSprintMode = 2;

		m_Openable = new OpenableBehaviour(false);
		
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");

		UpdateVisualState();
	}
	
	override void Open()
	{
		m_Openable.Open();
		SetSynchDirty();

		//! sets different set of melee modes for opened state
		m_MeleeMode = 3;
		m_MeleeHeavyMode = 4;
		m_MeleeSprintMode = 5;

		UpdateVisualState();
	}

	override void Close()
	{
		m_Openable.Close();
		SetSynchDirty();

		//! sets different set of melee modes for closed state
		m_MeleeMode = 0;
		m_MeleeHeavyMode = 1;
		m_MeleeSprintMode = 2;

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
			SetAnimationPhase("Doors1", 0.0);
		}
		else
		{
			SetAnimationPhase("Doors1", 1.0);
		}
	}
	
	override int GetMeleeMode()
	{
		return m_MeleeMode;
	}

	override int GetMeleeHeavyMode()
	{
		return m_MeleeHeavyMode;
	}
	
	override int GetMeleeSprintMode()
	{
		return m_MeleeSprintMode;
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
	}
}

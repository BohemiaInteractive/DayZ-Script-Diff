class Land_Underground_Panel_Lever : Land_Underground_Panel
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionUseUndergroundLever);
	}

	void ResetPhase(EntityAI target)
	{
		target.SetAnimationPhaseNow("PanelLever", 0);
	}
	
	override void OnPanelUsedSynchronized()
	{
		super.OnPanelUsedSynchronized();
		SetAnimationPhase("PanelLever", 1);
		g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( ResetPhase, 3000, false, this);
		OnLEDStateChanged();
	}
	
	override void Interact()
	{
		super.Interact();
		g_Game.RegisterNetworkStaticObject(this);
		g_Game.RegisterNetworkStaticObject(GetLinkedDoor());
	}
}

class DayZInfectedInputController : DayZCreatureAIInputController
{
	//! Same as 'DayZCreatureAIInputController::IsJump'
	proto native bool IsVault();

	//! Same as 'DayZCreatureAIInputController::GetJumpHeight'
	proto native float GetVaultHeight();
	
	proto native int GetMindState();
	proto native EntityAI GetTargetEntity();
	
	//--------------------------------------------------------------
	//! never created by script
	private void DayZInfectedInputController()
	{
	}

	//! never destroyed by script
	private void ~DayZInfectedInputController()
	{
	}	
};

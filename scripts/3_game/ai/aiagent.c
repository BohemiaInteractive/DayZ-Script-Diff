class AIAgent : Managed
{
	private void AIAgent();
	private void ~AIAgent();
	
	proto native void SetKeepInIdle(bool enable);
	proto native AIGroup GetGroup();
}
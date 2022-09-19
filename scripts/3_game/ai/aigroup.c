class AIGroup : Managed
{
	private void AIGroup();
	private void ~AIGroup();
	
	/*!
	Manual group management - add / remove agents from group.
	Don't forget to remove agent from old group.
	Caution! Empty groups are deleted on the next frame.
	*/
 	proto native void AddAgent(notnull AIAgent agent);
 	proto native void RemoveAgent(notnull AIAgent agent);
	
	proto native AIGroupBehaviour GetBehaviour();
};
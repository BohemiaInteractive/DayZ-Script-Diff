//! base class of all commands exposed to script to provide common functionality over animations
class AnimCommandBase
{
	// These would be private if not for the scripted commands
	// As other commands lifetime are handled internally
	protected void AnimCommandBase() {}
	protected void ~AnimCommandBase() {}
	
	//! returns entity that this command is bount to
	proto native IEntity GetEntity();

	//! called when command starts
	void OnActivate() {}

	//! called when command ends
	void OnDeactivate() {}

	//! called before any animation is processed
	//! here change animation values, add animation commands
	void PreAnimUpdate(float pDt) {}

	//! after animation is processed, before physics is processed
	//! here you can listen to various events
	void PrePhysUpdate(float pDt) {}

	//! functions usable only from OnActivate or PreAnimUpdate
	proto native void PreAnim_CallCommand(int pCommand, int pParamInt, float pParamFloat);
	proto native void PreAnim_SetFloat(int pVar, float pFlt);
	proto native void PreAnim_SetInt(int pVar, int pInt);
	proto native void PreAnim_SetBool(int pVar, bool pBool);

	//! functions usable in PrePhysUpdate
	proto native bool PrePhys_IsEvent(int pEvent);
	proto native bool PrePhys_IsTag(int pTag);
}

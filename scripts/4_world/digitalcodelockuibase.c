class DigitalCodeLockUIBase : UIScriptedMenu 
{
	void SetTargetLock(CodeLockComponent lock);
	void KeyPress(int key);
	void CodeResponse(bool response);
}

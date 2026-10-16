#ifdef ROBOCLIENT
class RoboclientUtils
{
	static proto native bool ReadStateMachines();
	static proto native void GetStateMachines(out array<BotStateMachineData> stateMachines);
	static proto native bool GetDefaultRoboRespawn();
	static proto native int GetDefaultRoboRespawnTime();
}
#endif
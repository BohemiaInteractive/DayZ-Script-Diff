#ifdef ROBOCLIENT
class BotStateMachineData
{
	proto string GetName();
	proto string GetInitialState();
	proto bool GetActive();
	proto bool GetRoboRespawn();
	proto float GetRoboRespawnTime();
	proto native void GetFromStates(TStringArray values);
	proto native void GetToStates(TStringArray values);
	proto native void GetEvents(TStringArray values);
	proto native void GetActions(TStringArray values);
	proto native void GetGuards(TStringArray values, TStringArray valuesDecorator);
	
	proto void SetName(string name);
	proto void SetRoboRespawn(bool respawn);
	proto void SetRoboRespawnTime(bool respawnTime);

	void BotStateMachineData();
	void ~BotStateMachineData();
}
#endif
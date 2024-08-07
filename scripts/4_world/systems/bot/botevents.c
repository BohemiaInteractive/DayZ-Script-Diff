/**@class	BotEventBase
 * @brief	represents event that triggers transition from state to state
 **/
class BotEventBase
{
	PlayerBase m_Player;
	EntityAI m_Entity;

	void BotEventBase (PlayerBase p = NULL, EntityAI e = NULL) { m_Entity = e; }
	string DumpToString () {}
};

//class BotEventXXX : BotEventBase { void BotEventXXX (PlayerBase p = NULL) { } };


class BotEventStart : BotEventBase { };
class BotEventStop : BotEventBase { };

class BotEventEndOK : BotEventBase { };
class BotEventEndFail : BotEventBase { };
class BotEventEndTimeout : BotEventBase { };
class BotEventOnItemInHandsChanged : BotEventBase { };

class BotEventStartDebug : BotEventBase
{
	int m_Id;

	void BotEventStartDebug(PlayerBase p = NULL, EntityAI e = NULL, int id = 0)
	{
		m_Id = id;
	}
	
	override string DumpToString () { return "id: " + m_Id; }
};

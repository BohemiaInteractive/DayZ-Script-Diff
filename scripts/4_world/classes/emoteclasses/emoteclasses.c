
class EmoteGreeting extends EmoteBase
{
	void EmoteGreeting()
	{
		m_ID = EmoteConstants.ID_EMOTE_GREETING;
		m_InputActionName = "EmoteGreeting";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_GREETING;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_GREETING;
		m_HideItemInHands = false;
	}
}

class EmoteSOS extends EmoteBase
{
	void EmoteSOS()
	{
		m_ID = EmoteConstants.ID_EMOTE_SOS;
		m_InputActionName = "EmoteSOS";
		m_StanceMaskAdditive = 0;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_ERECT;
		m_AdditiveCallbackUID = 0;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_SOS;
		m_HideItemInHands = true;
	}
}

class EmoteHeart extends EmoteBase
{
	void EmoteHeart()
	{
		m_ID = EmoteConstants.ID_EMOTE_HEART;
		m_InputActionName = "EmoteHeart";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_HEART;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_HEART;
		m_HideItemInHands = true;
	}
}

class EmoteTaunt extends EmoteBase
{
	void EmoteTaunt()
	{
		m_ID = EmoteConstants.ID_EMOTE_TAUNT;
		m_InputActionName = "EmoteTaunt";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_TAUNT;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_TAUNT;
		m_HideItemInHands = false;
	}
}

class EmoteLyingDown extends EmoteBase
{
	protected const float 	WATER_DEPTH = 0.15;
	void EmoteLyingDown()
	{
		m_ID = EmoteConstants.ID_EMOTE_LYINGDOWN;
		m_InputActionName = "EmoteLyingDown";
		m_StanceMaskAdditive = 0;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_AdditiveCallbackUID = 0;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_LYINGDOWN;
		m_HideItemInHands = true;
	}
	
	override bool EmoteCondition(int stancemask)
	{
		vector water_info = HumanCommandSwim.WaterLevelCheck( m_Player, m_Player.GetPosition() - (m_Player.GetDirection() * 0.9) );
		if ( water_info[0] < WATER_DEPTH ) //is player able to lay down without "drowning"?
			return true;
		return false;
	}
}

class EmoteTauntKiss extends EmoteBase
{
	void EmoteTauntKiss()
	{
		m_ID = EmoteConstants.ID_EMOTE_TAUNTKISS;
		m_InputActionName = "EmoteTauntKiss";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_TAUNTKISS;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_TAUNTKISS;
		m_HideItemInHands = false;
	}
}

class EmotePoint extends EmoteBase
{
	void EmotePoint()
	{
		m_ID = EmoteConstants.ID_EMOTE_POINT;
		m_InputActionName = "EmotePoint";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_POINT;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_POINT;
		m_HideItemInHands = false;
	}
}

class EmoteTauntElbow extends EmoteBase
{
	void EmoteTauntElbow()
	{
		m_ID = EmoteConstants.ID_EMOTE_TAUNTELBOW;
		m_InputActionName = "EmoteTauntElbow";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = 0;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_TAUNTELBOW;
		m_FullbodyCallbackUID = 0;
		m_HideItemInHands = true;
	}
}

class EmoteThumb extends EmoteBase
{
	void EmoteThumb()
	{
		m_ID = EmoteConstants.ID_EMOTE_THUMB;
		m_InputActionName = "EmoteThumb";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_THUMB;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_THUMB;
		m_HideItemInHands = false;
	}
}

class EmoteThumbDown extends EmoteBase
{
	void EmoteThumbDown()
	{
		m_ID = EmoteConstants.ID_EMOTE_THUMBDOWN;
		m_InputActionName = "EmoteThumbDown";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_THUMBDOWN;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_THUMBDOWN;
		m_HideItemInHands = false;
	}
}

class EmoteThroat extends EmoteBase
{
	void EmoteThroat()
	{
		m_ID = EmoteConstants.ID_EMOTE_THROAT;
		m_InputActionName = "EmoteThroat";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_THROAT;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_THROAT;
		m_HideItemInHands = false;
	}
}

class EmoteDance extends EmoteBase
{
	void EmoteDance()
	{
		m_ID = EmoteConstants.ID_EMOTE_DANCE;
		m_InputActionName = "EmoteDance";
		m_StanceMaskAdditive = 0;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_ERECT;
		m_AdditiveCallbackUID = 0;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_DANCE;
		m_HideItemInHands = true;
	}
	
	override bool EmoteCondition(int stancemask)
	{
		if (m_Player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		return true;
	}
}

class EmoteSalute extends EmoteBase
{
	void EmoteSalute()
	{
		m_ID = EmoteConstants.ID_EMOTE_SALUTE;
		m_InputActionName = "EmoteSalute";
		m_StanceMaskAdditive = 0;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_ERECT;
		m_AdditiveCallbackUID = 0;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_SALUTE;
		m_HideItemInHands = true;
	}
}

class EmoteTimeout extends EmoteBase
{
	void EmoteTimeout()
	{
		m_ID = EmoteConstants.ID_EMOTE_TIMEOUT;
		m_InputActionName = "EmoteTimeout";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_TIMEOUT;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_TIMEOUT;
		m_HideItemInHands = true;
	}
}

class EmoteDabbing extends EmoteBase
{
	void EmoteDabbing()
	{
		m_ID = EmoteConstants.ID_EMOTE_DABBING;
		//m_InputActionName = "EmoteDabbing";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = 0;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_DABBING;
		m_FullbodyCallbackUID = 0;
		m_HideItemInHands = true;
	}
}

class EmoteFacepalm extends EmoteBase
{
	void EmoteFacepalm()
	{
		m_ID = EmoteConstants.ID_EMOTE_FACEPALM;
		m_InputActionName = "EmoteFacepalm";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = 0;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_FACEPALM;
		m_FullbodyCallbackUID = 0;
		m_HideItemInHands = false;
	}
}

class EmoteClap extends EmoteBase
{
	void EmoteClap()
	{
		m_ID = EmoteConstants.ID_EMOTE_CLAP;
		m_InputActionName = "EmoteClap";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = 0;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_CLAP;
		m_FullbodyCallbackUID = 0;
		m_HideItemInHands = true;
	}
}

class EmoteSilent extends EmoteBase
{
	void EmoteSilent()
	{
		m_ID = EmoteConstants.ID_EMOTE_SILENT;
		m_InputActionName = "EmoteSilent";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_SILENCE;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_SILENCE;
		m_HideItemInHands = false;
	}
}

class EmoteWatching extends EmoteBase
{
	void EmoteWatching()
	{
		m_ID = EmoteConstants.ID_EMOTE_WATCHING;
		m_InputActionName = "EmoteWatching";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_WATCHING;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_WATCHING;
		m_HideItemInHands = false;
	}
}

class EmoteHold extends EmoteBase
{
	void EmoteHold()
	{
		m_ID = EmoteConstants.ID_EMOTE_HOLD;
		m_InputActionName = "EmoteHold";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_HOLD;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_HOLD;
		m_HideItemInHands = false;
	}
}

class EmoteListening extends EmoteBase
{
	void EmoteListening()
	{
		m_ID = EmoteConstants.ID_EMOTE_LISTENING;
		m_InputActionName = "EmoteListening";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_LISTENING;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_LISTENING;
		m_HideItemInHands = false;
	}
}

class EmotePointSelf extends EmoteBase
{
	void EmotePointSelf()
	{
		m_ID = EmoteConstants.ID_EMOTE_POINTSELF;
		m_InputActionName = "EmotePointSelf";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_POINTSELF;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_POINTSELF;
		m_HideItemInHands = false;
	}
}

class EmoteLookAtMe extends EmoteBase
{
	void EmoteLookAtMe()
	{
		m_ID = EmoteConstants.ID_EMOTE_LOOKATME;
		m_InputActionName = "EmoteLookAtMe";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_LOOKATME;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_LOOKATME;
		m_HideItemInHands = false;
	}
}

class EmoteTauntThink extends EmoteBase
{
	void EmoteTauntThink()
	{
		m_ID = EmoteConstants.ID_EMOTE_TAUNTTHINK;
		m_InputActionName = "EmoteTauntThink";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_TAUNTTHINK;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_TAUNTTHINK;
		m_HideItemInHands = false;
	}
}

class EmoteMove extends EmoteBase
{
	void EmoteMove()
	{
		m_ID = EmoteConstants.ID_EMOTE_MOVE;
		m_InputActionName = "EmoteMove";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_MOVE;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_MOVE;
		m_HideItemInHands = false;
	}
}

class EmoteGetDown extends EmoteBase
{
	void EmoteGetDown()
	{
		m_ID = EmoteConstants.ID_EMOTE_DOWN;
		m_InputActionName = "EmoteGetDown";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_DOWN;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_DOWN;
		m_HideItemInHands = false;
	}
}

class EmoteCome extends EmoteBase
{
	void EmoteCome()
	{
		m_ID = EmoteConstants.ID_EMOTE_COME;
		m_InputActionName = "EmoteCome";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_COME;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_COME;
		m_HideItemInHands = false;
	}
}

//Handled directly via EmoteManager!
class EmoteSurrender extends EmoteBase
{
	void EmoteSurrender()
	{
		m_ID = EmoteConstants.ID_EMOTE_SURRENDER;
		m_InputActionName = "EmoteSurrender";
		m_HideItemInHands = false;
	}
	
	override bool DetermineOverride(out int callback_ID, out int stancemask, out bool is_fullbody)
	{
		stancemask = DayZPlayerConstants.STANCEMASK_ALL;
		return true;
	}
	
	override bool EmoteStartOverride(typename callbacktype, int id, int mask, bool fullbody)
	{
		bool surrendered = m_Player.GetEmoteManager().m_IsSurrendered;
		if (!surrendered)
		{
			m_Player.GetEmoteManager().PlaySurrenderInOut(true);
		}
		else
		{
			if (m_Player.GetItemInHands())
				m_Player.GetItemInHands().Delete();
		}
		
		return true;
	}
}

class EmoteCampfireSit extends EmoteBase
{
	void EmoteCampfireSit()
	{
		m_ID = EmoteConstants.ID_EMOTE_CAMPFIRE;
		m_InputActionName = "EmoteCampfireSit";
		m_StanceMaskAdditive = 0;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_AdditiveCallbackUID = 0;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_CAMPFIRE;
		m_HideItemInHands = true;
	}
}

class EmoteSitA extends EmoteBase
{
	void EmoteSitA()
	{
		m_ID = EmoteConstants.ID_EMOTE_SITA;
		m_InputActionName = "EmoteSitA";
		m_StanceMaskAdditive = 0;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_AdditiveCallbackUID = 0;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_SITA;
		m_HideItemInHands = false;
	}
}

class EmoteSitB extends EmoteBase
{
	void EmoteSitB()
	{
		m_ID = EmoteConstants.ID_EMOTE_SITB;
		m_InputActionName = "EmoteSitB";
		m_StanceMaskAdditive = 0;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_AdditiveCallbackUID = 0;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_SITB;
		m_HideItemInHands = false;
	}
}

class EmoteRPSRandom extends EmoteBase
{
	void EmoteRPSRandom()
	{
		m_ID = EmoteConstants.ID_EMOTE_RPS;
		m_InputActionName = "EmoteRPSRandom";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = 0;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_RPS;
		m_FullbodyCallbackUID = 0;
		m_HideItemInHands = false;
	}
}

class EmoteRPSRock extends EmoteBase
{
	void EmoteRPSRock()
	{
		m_ID = EmoteConstants.ID_EMOTE_RPS_R;
		m_InputActionName = "EmoteRPSRock";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = 0;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_RPS;
		m_FullbodyCallbackUID = 0;
		m_HideItemInHands = false;
	}
}

class EmoteRPSPaper extends EmoteBase
{
	void EmoteRPSPaper()
	{
		m_ID = EmoteConstants.ID_EMOTE_RPS_P;
		m_InputActionName = "EmoteRPSPaper";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = 0;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_RPS;
		m_FullbodyCallbackUID = 0;
		m_HideItemInHands = false;
	}
}

class EmoteRPSScisors extends EmoteBase
{
	void EmoteRPSScisors()
	{
		m_ID = EmoteConstants.ID_EMOTE_RPS_S;
		m_InputActionName = "EmoteRPSScisors";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = 0;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_RPS;
		m_FullbodyCallbackUID = 0;
		m_HideItemInHands = false;
	}
}

class EmoteNod extends EmoteBase
{
	void EmoteNod()
	{
		m_ID = EmoteConstants.ID_EMOTE_NOD;
		m_InputActionName = "EmoteNod";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_NODHEAD;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_NODHEAD;
		m_HideItemInHands = false;
	}
}

class EmoteShake extends EmoteBase
{
	void EmoteShake()
	{
		m_ID = EmoteConstants.ID_EMOTE_SHAKE;
		m_InputActionName = "EmoteShake";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_SHAKEHEAD;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_SHAKEHEAD;
		m_HideItemInHands = false;
	}
}

class EmoteShrug extends EmoteBase
{
	void EmoteShrug()
	{
		m_ID = EmoteConstants.ID_EMOTE_SHRUG;
		m_InputActionName = "EmoteShrug";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_SHRUG;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_SHRUG;
		m_HideItemInHands = false;
	}
}

class EmoteSuicide extends EmoteBase
{
	void EmoteSuicide()
	{
		m_ID = EmoteConstants.ID_EMOTE_SUICIDE;
		m_InputActionName = "EmoteSuicide";
		m_StanceMaskAdditive = 0;
		m_StanceMaskFullbody = 0;
		m_AdditiveCallbackUID = 0;
		m_FullbodyCallbackUID = 0;
		m_HideItemInHands = false;
	}
	
	override bool DetermineOverride(out int callback_ID, out int stancemask, out bool is_fullbody)
	{
		is_fullbody = true;
		callback_ID = -1;
		stancemask = 0;
		string suicideStr;
		ItemBase weapon;
		weapon = m_Player.GetItemInHands();
		if (weapon)
		{
			if (weapon.ConfigIsExisting("suicideAnim"))
			{
				suicideStr = weapon.ConfigGetString("suicideAnim");
			}
			
			if (weapon.IsKindOf("Pistol_Base"))
			{
				callback_ID = DayZPlayerConstants.CMD_SUICIDEFB_PISTOL;
				stancemask = DayZPlayerConstants.STANCEMASK_CROUCH;
				//m_Player.OverrideShootFromCamera(false);
			}
			else if (Weapon_Base.Cast(weapon)) //long firearms
			{
				callback_ID = DayZPlayerConstants.CMD_SUICIDEFB_RIFLE;
				stancemask = DayZPlayerConstants.STANCEMASK_CROUCH;
				//m_Player.OverrideShootFromCamera(false);
			}
			else
			{
				switch (suicideStr)
				{
					case "onehanded":
						callback_ID = DayZPlayerConstants.CMD_SUICIDEFB_1HD;
						stancemask = DayZPlayerConstants.STANCEMASK_CROUCH;
					break;
					
					case "fireaxe":
						callback_ID = DayZPlayerConstants.CMD_SUICIDEFB_FIREAXE;
						stancemask = DayZPlayerConstants.STANCEMASK_ERECT;
					break;
					
					case "pitchfork":
						callback_ID = DayZPlayerConstants.CMD_SUICIDEFB_PITCHFORK;
						stancemask = DayZPlayerConstants.STANCEMASK_ERECT;
					break;
					
					case "sword":
						callback_ID = DayZPlayerConstants.CMD_SUICIDEFB_SWORD;
						stancemask = DayZPlayerConstants.STANCEMASK_ERECT;
					break;
					
					case "spear":
						callback_ID = DayZPlayerConstants.CMD_SUICIDEFB_SPEAR;
						stancemask = DayZPlayerConstants.STANCEMASK_ERECT;
					break;
					
					case "woodaxe":
						callback_ID = DayZPlayerConstants.CMD_SUICIDEFB_WOODAXE;
						stancemask = DayZPlayerConstants.STANCEMASK_ERECT;
					break;
					
					case "sickle":
						callback_ID = DayZPlayerConstants.CMD_SUICIDEFB_SICKLE;
						stancemask = DayZPlayerConstants.STANCEMASK_ERECT;
					break;
					
					case "hoe":
						callback_ID = DayZPlayerConstants.CMD_SUICIDEFB_HOE;
						stancemask = DayZPlayerConstants.STANCEMASK_ERECT;
					break;
				}
			}
		}
		if (callback_ID > -1)
		{
			return true;
		}
		return false;
	}
	
	override bool EmoteCondition(int stancemask)
	{
		Weapon_Base weapon;
		weapon = Weapon_Base.Cast(m_Player.GetItemInHands());
		
		if (weapon)
			return weapon.ConfigGetBool("isSuicideWeapon");
		
		return true;
	}
	
	override void OnBeforeStandardCallbackCreated(int callback_ID, int stancemask, bool is_fullbody)
	{
		if (callback_ID > -1)
		{
			if (callback_ID == DayZPlayerConstants.CMD_SUICIDEFB_PISTOL || callback_ID == DayZPlayerConstants.CMD_SUICIDEFB_RIFLE)
				m_Player.OverrideShootFromCamera(false);
		}
		else if (callback_ID < 0)
		{
			m_Player.SetInventorySoftLock(false);
		}
	}
}

class EmoteVomit extends EmoteBase
{
	void EmoteVomit()
	{
		m_ID = EmoteConstants.ID_EMOTE_VOMIT;
		m_InputActionName = "EmoteVomit";
		m_StanceMaskAdditive = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_StanceMaskFullbody = DayZPlayerConstants.STANCEMASK_PRONE;
		m_AdditiveCallbackUID = DayZPlayerConstants.CMD_GESTUREMOD_NODHEAD;
		m_FullbodyCallbackUID = DayZPlayerConstants.CMD_GESTUREFB_NODHEAD;
		m_HideItemInHands = false;
	}
	
	override bool EmoteCondition(int stancemask)
	{
		return DayZPlayerUtils.PlayerCanChangeStance(m_Player,DayZPlayerConstants.STANCEIDX_CROUCH);
	}
	
	override bool EmoteStartOverride(typename callbacktype, int id, int mask, bool fullbody)
	{
		if ( m_Player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER || !GetGame().IsMultiplayer() )
		{
			SymptomBase symptom = m_Player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
			
			if ( symptom )
			{
			    symptom.SetDuration(Math.RandomIntInclusive(4,8));
			}
		}
		
		m_Player.GetEmoteManager().SetEmoteLockState(false);
		
		return true;
	}
}
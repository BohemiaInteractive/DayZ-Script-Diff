class EmoteCB : HumanCommandActionCallback
{
	bool 			m_IsFullbody;
	int 			m_callbackID; //Animation cmd ID
	PlayerBase 		m_player;
	EmoteManager 	m_Manager;
	
	void ~EmoteCB()
	{
		if (m_Manager)
		{
			m_Manager.OnCallbackEnd();
		}
		
		if (GetGame() && m_player)
			m_player.RequestHandAnimationStateRefresh();
	}
	
	bool CancelCondition()
	{
		return false;
	}
	
	bool IsEmoteCallback()
	{
		return IsGestureCallback();
	}
	
	override void OnAnimationEvent(int pEventID)
	{
		switch (pEventID)
		{
			case EmoteConstants.EMOTE_SUICIDE_DEATH :
				
				if (GetGame().IsServer())
					m_Manager.KillPlayer();
				
				m_Manager.LogSuicide();
			break;
			
			case UA_ANIM_EVENT :
				if (m_callbackID == DayZPlayerConstants.CMD_GESTUREFB_SURRENDERIN)
					m_Manager.m_ItemToBeCreated = true;
			break;
			
			case EmoteConstants.EMOTE_SUICIDE_BLEED :
				if (GetGame().IsServer())
					m_Manager.CreateBleedingEffect(m_callbackID);
			break;
			
			case EmoteConstants.EMOTE_SUICIDE_SIMULATION_END :
				if (GetGame().IsServer())
				{
					EntityAI itemInHands = m_player.GetHumanInventory().GetEntityInHands();
					if (itemInHands)
					{
						vector m4[4];
						itemInHands.GetTransform(m4);
						m_player.GetInventory().DropEntityWithTransform(InventoryMode.SERVER, m_player, itemInHands, m4);
					}
				}
				
				m_player.StartDeath();
			break;
		}
	}
	
	override bool IsGestureCallback()
	{
		return true;
	}
};

class EmoteLauncher
{
	static const int FORCE_NONE = 0;
	static const int FORCE_DIFFERENT = 1;
	static const int FORCE_ALL = 2;
	
	protected bool m_InterruptsSameIDEmote;
	protected int m_ForcePlayEmote;
	protected int m_ID;
	
	void EmoteLauncher(int emoteID, bool interrupts_same)
	{
		m_ID = emoteID;
		m_InterruptsSameIDEmote = interrupts_same;
		m_ForcePlayEmote = FORCE_NONE;
	}
	
	void SetForced(int mode)
	{
		m_ForcePlayEmote = mode;
	}
	
	int GetForced()
	{
		return m_ForcePlayEmote;
	}
	
	int GetID()
	{
		return m_ID;
	}
}

class EmoteManager
{
	PlayerBase 				m_Player;
	ItemBase				m_item;
	EmoteCB					m_Callback;
	HumanInputController 	m_HIC;
	ref array<string> 		m_InterruptInputs;
	ref array<UAInput> 		m_InterruptInputDirect;
	ref InventoryLocation 	m_HandInventoryLocation;
	ref EmoteLauncher 		m_MenuEmote;
	bool					m_bEmoteIsRequestPending;
	bool					m_bEmoteIsPlaying;
	bool 					m_IsSurrendered;
	bool 					m_ItemToBeCreated;
	bool 					m_CancelEmote;
	bool 					m_InstantCancelEmote;
	bool 					m_GestureInterruptInput;
	protected bool			m_ItemToHands; //deprecated
	protected bool			m_ItemIsOn;
	protected bool			m_MouseButtonPressed;
	protected bool 			m_PlayerDies;
	protected bool 			m_controllsLocked;
	protected bool 			m_InventoryAccessLocked;
	protected bool 			m_EmoteLockState;
	protected int 			m_DeferredEmoteExecution;
	protected int  			m_GestureID;
	protected int			m_PreviousGestureID;
	protected int			m_CurrentGestureID;
	protected int 			m_LastMask;
	protected int 			m_RPSOutcome;
	protected int 			m_InterruptInputsCount;
	protected const int 	CALLBACK_CMD_INVALID = -1;
	protected const int 	CALLBACK_CMD_END = -2;
	protected const int 	CALLBACK_CMD_GESTURE_INTERRUPT = -3;
	protected const int 	CALLBACK_CMD_INSTACANCEL = -4;
	PluginAdminLog 			m_AdminLog;
	protected ref Timer 	m_ReservationTimer;

	protected ref map<int, ref EmoteBase>		m_NameEmoteMap; //<emote_ID,EmoteBase>
	protected ref array<ref EmoteBase>			m_EmoteClassArray; //registered 'EmoteBase' object refs
	protected ref array<int>					m_EmoteIDs; //IDs of registered emotes (found in 'EmoteConstants'). For quick access.
	protected ref array<int>					m_EmoteInputIDs; //input IDs for the registered 'EmoteBase' objects
	protected SHumanCommandSwimSettings 		m_HumanSwimSettings;
	
	void EmoteManager(PlayerBase player) 
	{
		m_Player = player;
		m_HIC = m_Player.GetInputController();
		m_ItemIsOn = false;
		m_controllsLocked = false;
		m_InventoryAccessLocked = false;
		m_RPSOutcome = -1;
		m_DeferredEmoteExecution = CALLBACK_CMD_INVALID;
		
		m_InterruptInputs = new array<string>;
		m_InterruptInputs.Insert("UAMoveForward");
		m_InterruptInputs.Insert("UAMoveBack");
		m_InterruptInputs.Insert("UATurnLeft");
		m_InterruptInputs.Insert("UATurnRight");
		m_InterruptInputs.Insert("UAMoveLeft");
		m_InterruptInputs.Insert("UAMoveRight");
		m_InterruptInputs.Insert("UAStand");
		m_InterruptInputs.Insert("UACrouch");
		m_InterruptInputs.Insert("UAProne");
		m_InterruptInputs.Insert("UAGetOver");
		
		m_HandInventoryLocation = new InventoryLocation();
		m_HandInventoryLocation.SetHands(m_Player, null);
		
		if (GetGame().IsServer())
		{
			m_AdminLog = PluginAdminLog.Cast(GetPlugin(PluginAdminLog));
		}
		
		m_ReservationTimer = new Timer();
		m_ReservationTimer.Run(8, this, "CheckEmoteLockedState", null, true);
		
		m_HumanSwimSettings = m_Player.GetDayZPlayerType().CommandSwimSettingsW();
		
		ConstructData();
	}
	
	void ~EmoteManager()
	{
		if (m_ReservationTimer && m_ReservationTimer.IsRunning())
			m_ReservationTimer.Stop();
	}
	
	void ConstructData()
	{
		EmoteConstructor ec = new EmoteConstructor;
		if (!m_NameEmoteMap)
		{
			ec.ConstructEmotes(m_Player,m_NameEmoteMap);
			m_EmoteClassArray = new array<ref EmoteBase>;
			m_EmoteIDs = new array<int>;
			m_EmoteInputIDs = new array<int>;
			int inputID;
			int count = m_NameEmoteMap.Count();
			UAInput inp;
			
			for (int i = 0; i < count; i++)
			{
				m_EmoteClassArray.Insert(m_NameEmoteMap.GetElement(i));
				m_EmoteIDs.Insert(m_NameEmoteMap.GetElement(i).GetID());
				inp = GetUApi().GetInputByName(m_NameEmoteMap.GetElement(i).GetInputActionName());
				inputID = inp.ID();
				m_EmoteInputIDs.Insert(inputID);
			}
			
			if (m_EmoteInputIDs.Count() != m_EmoteClassArray.Count())
			{
				ErrorEx("Faulty emote data detected in 'ConstructData' method!",ErrorExSeverity.WARNING);
			}
		}
	}
	
	void SetGesture(int id)
	{
		m_GestureID = id;
	}
	
	int GetGesture()
	{
		return m_GestureID;
	}
	
	int DetermineGestureIndex()
	{
		int count = m_EmoteInputIDs.Count();
		for (int i = 0; i < count; ++i)
 		{
			if (GetUApi().GetInputByID(m_EmoteInputIDs[i]).LocalPress())
 			{
 				return m_EmoteIDs[i];
 			}
 		}
		
		return 0;
	}
	
	//! Also includes a stance check for FB callbacks
	bool DetermineEmoteData(EmoteBase emote, out int callback_ID, out int stancemask, out bool is_fullbody)
	{
		if (emote.DetermineOverride(callback_ID, stancemask, is_fullbody))
		{
			return emote.EmoteFBStanceCheck(stancemask);
		}
		else if (emote.GetAdditiveCallbackUID() != 0 && m_Player.IsPlayerInStance(emote.GetStanceMaskAdditive()))
		{
			callback_ID = emote.GetAdditiveCallbackUID();
			stancemask = emote.GetStanceMaskAdditive();
			is_fullbody = false;
			return true;
		}
		else if (emote.GetFullbodyCallbackUID() != 0 && emote.EmoteFBStanceCheck(emote.GetStanceMaskFullbody()))
		{
			callback_ID = emote.GetFullbodyCallbackUID();
			stancemask = emote.GetStanceMaskFullbody();
			is_fullbody = true;
			return true;
		}
		
		return false;
	}
	
	//Called from players commandhandler each frame, checks input
	void Update(float deltaT)
	{
		// no updates on restrained characters
		if (m_Player.IsRestrained())
			return;
		
		if (m_ItemToBeCreated)
		{
			if (!m_Player.GetItemInHands() && GetGame().IsServer())
			{
				m_Player.GetHumanInventory().CreateInHands("SurrenderDummyItem");
			}
			m_ItemToBeCreated = false;
		}
		
		int gestureSlot = 0;
		#ifndef SERVER
			gestureSlot = DetermineGestureIndex();
		#endif
		//deferred emote cancel
		if (m_InstantCancelEmote) //'hard' cancel
		{
			if (m_Callback)
			{
				m_Callback.Cancel();
			}
			
			if (m_MenuEmote)
			{
				m_MenuEmote = null;
			}
			m_DeferredEmoteExecution = CALLBACK_CMD_INVALID;
			m_InstantCancelEmote = false;
			m_bEmoteIsRequestPending = false;
			SetEmoteLockState(false);
		}
		else if (m_CancelEmote) //'soft' cancel
		{
			if (m_IsSurrendered)
			{
				EndSurrenderRequest(new SurrenderData);
			}
			else if (m_Callback)
			{
				m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
			}
			
			m_bEmoteIsRequestPending = false;
			m_CancelEmote = false;
		}
		
		if (m_MenuEmote && m_MenuEmote.GetForced() > EmoteLauncher.FORCE_NONE && !GetGame().IsDedicatedServer()) //forced emote playing
		{
			SendEmoteRequestSync(m_MenuEmote.GetID());
		}
		else if (m_Callback)
		{
			bool uiGesture = false;
			if (!GetGame().IsDedicatedServer())
			{
				uiGesture = GetGame().GetUIManager().IsMenuOpen(MENU_GESTURES);
				
				if (InterruptGestureCheck())
				{
					SendEmoteRequestSync(CALLBACK_CMD_END);
				}
				
				if (InterruptWaterCheck())
				{
					SendEmoteRequestSync(CALLBACK_CMD_INSTACANCEL);
				}
			}
			
			if (gestureSlot > 0 || m_GestureInterruptInput || (m_HIC.IsSingleUse() && !uiGesture) || (m_HIC.IsContinuousUseStart() && !uiGesture) || (m_Callback.m_IsFullbody && !uiGesture && m_HIC.IsWeaponRaised())) 
			{
				if (m_CurrentGestureID == EmoteConstants.ID_EMOTE_SUICIDE && m_HIC.IsSingleUse())
				{
					if (m_Callback.GetState() == m_Callback.STATE_LOOP_LOOP)
					{
						CommitSuicide();
					}
					else
					{
						return;
					}
				}
				else if ((m_CurrentGestureID == EmoteConstants.ID_EMOTE_THUMB || m_CurrentGestureID == EmoteConstants.ID_EMOTE_THUMBDOWN) && m_HIC.IsSingleUse())
				{
					m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_ACTION);
				}
				else if (m_HIC.IsSingleUse() && (m_CurrentGestureID == EmoteConstants.ID_EMOTE_RPS || m_CurrentGestureID == EmoteConstants.ID_EMOTE_RPS_R || m_CurrentGestureID == EmoteConstants.ID_EMOTE_RPS_P || m_CurrentGestureID == EmoteConstants.ID_EMOTE_RPS_S))
				{
					if (m_RPSOutcome != -1)
					{
						if (m_RPSOutcome == 0)
						{
							m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_RPS_ROCK);
						}
						else if (m_RPSOutcome == 1)
						{
							m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_RPS_PAPER);
						}
						else if (m_RPSOutcome == 2)
						{
							m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_RPS_SCISSORS);
						}
						m_RPSOutcome = -1;
					}
					else
					{
						m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
					}
				}
				else if (m_CurrentGestureID != EmoteConstants.ID_EMOTE_SUICIDE || (m_CurrentGestureID == EmoteConstants.ID_EMOTE_SUICIDE && m_Callback.GetState() < 3))
				{
					SendEmoteRequestSync(CALLBACK_CMD_END);
				}
				else if (m_CurrentGestureID == EmoteConstants.ID_EMOTE_SUICIDE)
				{
					SendEmoteRequestSync(CALLBACK_CMD_END);
				}
			}
			
			if (m_LastMask != -1 && m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
			{
				m_Callback.Cancel();
			}
			
			if (m_MenuEmote && m_bEmoteIsPlaying)
			{
				SendEmoteRequestSync(CALLBACK_CMD_END);
			}
			
			if (m_DeferredEmoteExecution == CALLBACK_CMD_END)
			{
				EndCallbackCommand();
			}
			
			if (m_DeferredEmoteExecution == CALLBACK_CMD_GESTURE_INTERRUPT)
			{
				InterruptCallbackCommand();
			}
		}
		//no m_Callback exists
		else
		{
			if (m_bEmoteIsRequestPending && (m_Player.IsUnconscious() || !m_Player.IsAlive()))
			{
				m_bEmoteIsRequestPending = false;
			}
			
			if (m_bEmoteIsPlaying)
			{
				OnEmoteEnd();
			}
			else if (!m_Player.GetItemInHands() && m_IsSurrendered && !m_ItemToBeCreated && (!m_MenuEmote || m_MenuEmote.GetForced() != EmoteLauncher.FORCE_NONE)) //play only when there is time to do so
			{
				PlaySurrenderInOut(false);
				return;
			}
			// getting out of surrender state - hard cancel
			else if (m_IsSurrendered && (m_HIC.IsSingleUse() || m_HIC.IsContinuousUseStart() || m_HIC.IsWeaponRaised()))
			{
				if (m_Player.GetItemInHands())
					m_Player.GetItemInHands().DeleteSafe();//Note, this keeps item 'alive' until it is released by all the systems (inventory swapping etc.)
				return;
			}
			// fallback in case lock does not end properly
			else if (m_IsSurrendered && (!m_Player.GetItemInHands() || (m_Player.GetItemInHands() && m_Player.GetItemInHands().GetType() != "SurrenderDummyItem" && m_EmoteLockState)))
			{
				m_IsSurrendered = false;
				SetEmoteLockState(false);
				return;
			}
			//actual emote launch
			else if (m_DeferredEmoteExecution > CALLBACK_CMD_INVALID)
			{
				PlayEmote(m_DeferredEmoteExecution);
			}
			//client-side emote launcher
			else if (!m_bEmoteIsPlaying && m_MenuEmote && !GetGame().IsDedicatedServer())
			{
				SendEmoteRequestSync(m_MenuEmote.GetID());
			}
			else if (!m_MenuEmote && gestureSlot > 0)
			{
				CreateEmoteCBFromMenu(gestureSlot,true); //translation no longer needed
			}
		}
	}
	
	void OnEmoteEnd()
	{
		ShowItemInHands();
		
		if (m_PlayerDies)
		{
			m_Player.SetHealth(0.0);
			return;
		}
		
		//surrender "state" switch
		if (m_CurrentGestureID == EmoteConstants.ID_EMOTE_SURRENDER)
		{
			m_IsSurrendered = !m_IsSurrendered;
			SetEmoteLockState(m_IsSurrendered);
		}
		
		m_CurrentGestureID = 0;
		
		m_bEmoteIsPlaying = false;
		m_bEmoteIsRequestPending = false;
		
		if (m_IsSurrendered)
		{
			return;
		}
		m_GestureInterruptInput = false;
		SetEmoteLockState(false);

		// back to the default - shoot from camera - if not set already
		if (!m_Player.IsShootingFromCamera())
			m_Player.OverrideShootFromCamera(true);
	}
	
	//server-side
	bool OnInputUserDataProcess(int userDataType, ParamsReadContext ctx)
	{
		if (userDataType == INPUT_UDT_GESTURE)
		{
			int forced = EmoteLauncher.FORCE_NONE;
			int gestureID = -1;
			int random = -1;
			
			if (ctx.Read(gestureID))
			{
				ctx.Read(forced);
				if (ctx.Read(random))
				{
					m_RPSOutcome = random;
				}
				
				//server-side check, sends CALLBACK_CMD_INSTACANCEL as a fail
				if (forced == EmoteLauncher.FORCE_NONE && !CanPlayEmote(gestureID))
					gestureID = CALLBACK_CMD_INSTACANCEL;
				
				ScriptJunctureData pCtx = new ScriptJunctureData;
				pCtx.Write(gestureID);
				pCtx.Write(forced);
				m_Player.SendSyncJuncture(DayZPlayerSyncJunctures.SJ_GESTURE_REQUEST, pCtx);
				m_bEmoteIsRequestPending = true;
			}
			return true;
		}
		return false;
	}
	
	//server and client
	void OnSyncJuncture(int pJunctureID, ParamsReadContext pCtx)
	{
		int forced;
		int gesture_id;
		if (!m_CancelEmote)
		{
			pCtx.Read(gesture_id);
			pCtx.Read(forced);
			
			EmoteBase emoteData;
			if ((m_Callback || m_IsSurrendered) && (forced == EmoteLauncher.FORCE_ALL || (forced == EmoteLauncher.FORCE_DIFFERENT && m_CurrentGestureID != gesture_id)))
			{	
				if (m_Callback)
				{
					if (m_NameEmoteMap.Find(m_CurrentGestureID,emoteData) && emoteData.CanBeCanceledNormally(m_Callback))
						m_CancelEmote = true;
					else
						return;
				}
				m_CancelEmote = true;
			}
			
			if (gesture_id == CALLBACK_CMD_INSTACANCEL)
			{
				if (m_Callback)
				{
					if (m_NameEmoteMap.Find(m_CurrentGestureID,emoteData) && !emoteData.CanBeCanceledNormally(m_Callback))
					{
						m_InstantCancelEmote = false;
						return;
					}
				}
				m_InstantCancelEmote = true;
			}
			
			m_DeferredEmoteExecution = gesture_id;
		}
		else
			m_CancelEmote = false;
	}
	
	void OnCallbackEnd()
	{
		EmoteBase emoteData;
		if (m_NameEmoteMap.Find(m_CurrentGestureID,emoteData))
			emoteData.OnCallbackEnd();
	}
	
	void AfterStoreLoad()
	{
		if (m_Player.GetItemInHands() && SurrenderDummyItem.Cast(m_Player.GetItemInHands()))
			m_Player.GetItemInHands().Delete();
	}
	
	bool PlayEmote(int id)
	{
		m_DeferredEmoteExecution = CALLBACK_CMD_INVALID;
		m_bEmoteIsRequestPending = false;
		
		if (CanPlayEmote(id))
		{
			EmoteBase emote;
			m_NameEmoteMap.Find(id,emote);
			
			if (m_AdminLog)
				m_AdminLog.OnEmote(m_Player, emote);
			
			m_PreviousGestureID = m_CurrentGestureID;
			m_CurrentGestureID = id;
			if (id > 0)
			{
				if (emote)
				{
					int callback_ID;
					int stancemask;
					bool is_fullbody;
					if (DetermineEmoteData(emote,callback_ID,stancemask,is_fullbody))
					{
						if (!emote.EmoteStartOverride(EmoteCB,callback_ID,stancemask,is_fullbody))
						{
							emote.OnBeforeStandardCallbackCreated(callback_ID,stancemask,is_fullbody);
							CreateEmoteCallback(EmoteCB,callback_ID,stancemask,is_fullbody);
						}
						
						if (emote.GetHideItemInHands())
						{
							HideItemInHands();
						}
					}
					else
					{
						ErrorEx("EmoteManager | DetermineEmoteData failed!");
					}
				}
			}
		}
		
		SetEmoteLockState(m_bEmoteIsPlaying);
		return m_bEmoteIsPlaying;
	}
	
	//creates Emote callback
	protected void CreateEmoteCallback(typename callbacktype, int id, int mask, bool fullbody)
	{
		if (m_Player)
		{
			m_LastMask = -1;

			if (fullbody)
			{
				Class.CastTo(m_Callback, m_Player.StartCommand_Action(id,callbacktype,mask));
				m_Callback.m_IsFullbody = true;
				m_Callback.EnableCancelCondition(true);
			}
			else if (m_Player.IsPlayerInStance(mask))
			{
				m_LastMask = mask;  //character is probably not prone now
				Class.CastTo(m_Callback, m_Player.AddCommandModifier_Action(id,callbacktype));
			}
			
			if (m_Callback)
			{
				m_bEmoteIsPlaying = true;
				m_Callback.m_callbackID = id;
				m_Callback.m_player = m_Player;
				m_Callback.m_Manager = this;
			}
		}
	}
	
	protected void HideItemInHands()
	{
		m_item = m_Player.GetItemInHands();
		if (m_Callback && m_item)
		{
			m_Player.TryHideItemInHands(true);
		}
	}
	
	protected void ShowItemInHands()
	{
		if (m_item)
		{
			m_Player.TryHideItemInHands(false);
		}
	}
	
	void RequestCommitSuicide()
	{
		if (!GetGame().IsClient())
			CommitSuicide();
	}
	
	protected void CommitSuicide()
	{
		Weapon_Base weapon;
		WeaponEventBase weapon_event = new WeaponEventTrigger;
		
		if (Weapon_Base.CastTo(weapon,m_Player.GetItemInHands()))
		{
			if (weapon.CanFire())
			{
				m_Callback.RegisterAnimationEvent("Simulation_End",EmoteConstants.EMOTE_SUICIDE_SIMULATION_END);
				m_Player.SetSuicide(true);
				weapon.ProcessWeaponEvent(weapon_event);
				m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
				if (m_Player.IsAlive()) 
				{
					EntityAI helm = m_Player.FindAttachmentBySlotName("Headgear");
					if (helm && GetGame().IsServer())
					{
						float damage = helm.GetMaxHealth("","");
						helm.AddHealth("","", -damage/2);
					}
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.KillPlayer);
					if (m_AdminLog)
					{
						m_AdminLog.Suicide(m_Player);
					}
				}
			}
			else
			{
				if (!weapon.IsDamageDestroyed())
				{
					weapon.ProcessWeaponEvent(weapon_event);
				}
				m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			}
		}
		else if (m_Player.GetItemInHands() && m_Player.GetItemInHands().ConfigIsExisting("suicideAnim"))
		{
			m_Callback.RegisterAnimationEvent("Death",EmoteConstants.EMOTE_SUICIDE_DEATH);
			m_Callback.RegisterAnimationEvent("Bleed",EmoteConstants.EMOTE_SUICIDE_BLEED);
			m_Callback.RegisterAnimationEvent("Simulation_End",EmoteConstants.EMOTE_SUICIDE_SIMULATION_END);
			m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
			m_Player.SetSuicide(true);
		}
		else
		{
			m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
		}
	}

	void KillPlayer()
	{
		if (GetGame().IsServer())
		{
			m_Player.SetHealth(0);
		}
	}
	
	void LogSuicide()
	{
		if (GetGame().IsServer())
		{
			PlayerIdentity identity = m_Player.GetIdentity();
			if (identity)
			{
				if (m_AdminLog)
					m_AdminLog.Suicide(m_Player);
			}
		}
	}
	
	void CreateEmoteCBFromMenu(int id, bool interrupts_same = false)
	{
		m_MenuEmote = new EmoteLauncher(id,interrupts_same);
	}
	
	EmoteLauncher GetEmoteLauncher()
	{
		return m_MenuEmote;
	}
	
	void InterruptCallbackCommand()
	{
		m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
		
		if (m_MenuEmote)
			m_MenuEmote = null;
		
		m_DeferredEmoteExecution = CALLBACK_CMD_INVALID;
	}
	
	void EndCallbackCommand()
	{
		if (m_CurrentGestureID == EmoteConstants.ID_EMOTE_DANCE)
		{
			m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
		}
		else
		{
			m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		}
		
		if (m_MenuEmote)
			m_MenuEmote = null;
		m_DeferredEmoteExecution = CALLBACK_CMD_INVALID;
	}
	
	//sends request (client)
	void SendEmoteRequestSync(int id)
	{
		int forced = EmoteLauncher.FORCE_NONE;
		if (m_MenuEmote)
		{
			forced = m_MenuEmote.GetForced();
		}
		
		m_RPSOutcome = -1;
		switch (id)
		{
			case EmoteConstants.ID_EMOTE_RPS :
				m_RPSOutcome = Math.RandomInt(0,3);
			break;
			
			case EmoteConstants.ID_EMOTE_RPS_R :
				m_RPSOutcome = 0;
			break;
			
			case EmoteConstants.ID_EMOTE_RPS_P :
				m_RPSOutcome = 1;
			break;
			
			case EmoteConstants.ID_EMOTE_RPS_S :
				m_RPSOutcome = 2;
			break;
		}
		
		ScriptInputUserData ctx = new ScriptInputUserData;
		if (GetGame().IsMultiplayer() && GetGame().IsClient())
		{
			bool canProceed = true; //running callbacks in certain state can block additional actions
			EmoteBase emoteData;
			if (m_Callback && m_NameEmoteMap.Find(m_CurrentGestureID,emoteData))
			{
				canProceed = emoteData.CanBeCanceledNormally(m_Callback);
			}
			
			if (ctx.CanStoreInputUserData() && ((CanPlayEmote(id) && CanPlayEmoteClientCheck(id)) || forced) && canProceed)
			{
				ctx.Write(INPUT_UDT_GESTURE);
				ctx.Write(id);
				ctx.Write(forced);
				if (m_RPSOutcome != -1)
				{
					ctx.Write(m_RPSOutcome);
				}
				ctx.Send();
				m_bEmoteIsRequestPending = true;
			}
			else
			{
				m_bEmoteIsRequestPending = false;
			}
			m_MenuEmote = NULL;
			SetEmoteLockState(IsEmotePlaying());
		}
		else if (!GetGame().IsMultiplayer())
		{
			if (id == CALLBACK_CMD_END)
			{
				EndCallbackCommand();
			}
			else if (id == CALLBACK_CMD_GESTURE_INTERRUPT)
			{
				m_DeferredEmoteExecution = CALLBACK_CMD_GESTURE_INTERRUPT;
			}
			else if (CanPlayEmote(id) && CanPlayEmoteClientCheck(id))
			{
				PlayEmote(id);
			}
			else
			{
				SetEmoteLockState(IsEmotePlaying());
			}
			m_MenuEmote = NULL;
		}
	}
	
	bool IsControllsLocked()
	{
		return m_controllsLocked;
	}
	
	bool CanPlayEmote(int id)
	{
		//special cases
		if (id == CALLBACK_CMD_END || id == CALLBACK_CMD_GESTURE_INTERRUPT || id == CALLBACK_CMD_INSTACANCEL)
		{
			return true;
		}
		
		if (!m_Player || !m_Player.IsAlive() || (!IsEmotePlaying() && (m_Player.GetCommand_Action() || m_Player.GetCommandModifier_Action())) || m_Player.GetThrowing().IsThrowingModeEnabled())
		{	
			//Debug.Log("!CanPlayEmote | reason:  1");
			return false;
		}
		
		ItemBase item = m_Player.GetItemInHands();
		if (item)
		{
			if (item.IsHeavyBehaviour() && id != EmoteConstants.ID_EMOTE_SURRENDER)
			{
				//Debug.Log("!CanPlayEmote | reason:  2");
				return false;
			}
			
			SurrenderDummyItem sda;
			if (m_Player.IsItemsToDelete() && Class.CastTo(sda,item) && !sda.IsSetForDeletion())
			{
				//Debug.Log("!CanPlayEmote | reason:  3");
				return false;
			}
		}
		
		if ((m_Player.GetWeaponManager() && m_Player.GetWeaponManager().IsRunning()) || (m_Player.GetActionManager() && m_Player.GetActionManager().GetRunningAction()))
		{
			//Debug.Log("!CanPlayEmote | reason:  4");
			return false;
		}
		
		if (m_HIC.IsWeaponRaised() || m_Player.IsRolling() || m_Player.IsClimbing() || m_Player.IsRestrainStarted() || m_Player.IsFighting() || m_Player.IsSwimming() || m_Player.IsClimbingLadder() || m_Player.IsFalling() || m_Player.IsUnconscious() || m_Player.IsJumpInProgress() || m_Player.IsRestrained()) 	// rework conditions into something better?
		{
			//Debug.Log("!CanPlayEmote | reason:  5");
			return false;
		}
		
		if (m_Player.GetCommand_Vehicle())
		{
			//Debug.Log("!CanPlayEmote | reason:  6");
			return false;
		}
		
		HumanCommandMove cm = m_Player.GetCommand_Move();
		if (cm)
		{
			if (cm.IsOnBack() && id != EmoteConstants.ID_EMOTE_SURRENDER)
				return false;
			if (cm.IsChangingStance())
				return false;
		}
		
		//"locks" player in surrender state
		if (m_IsSurrendered && (id != EmoteConstants.ID_EMOTE_SURRENDER))
		{
			//Debug.Log("!CanPlayEmote | reason:  8");
			return false;
		}
		
		if (m_Player.GetDayZPlayerInventory().IsProcessing())
		{
			//Debug.Log("!CanPlayEmote | reason:  9");
			return false;
		}
		
		EmoteBase emote;
		if (m_NameEmoteMap.Find(id,emote))
		{
			int callback_ID;
			int stancemask;
			bool is_fullbody;
			if (DetermineEmoteData(emote,callback_ID,stancemask,is_fullbody) && emote.EmoteCondition(stancemask))
			{
				return true;
			}
			//Debug.Log("!CanPlayEmote | reason:  10");
		}
		
		return false;
	}
	
	bool CanPlayEmoteClientCheck(int id)
	{
		if (!GetGame().IsClient())
			return true;
		
		if (GetGame().GetUIManager().FindMenu(MENU_INVENTORY))
		{
			//Debug.Log("!CanPlayEmoteClientCheck | reason:  1");
			return false;
		}
		
		return true;
	}
	
	void PlaySurrenderInOut(bool state)
	{
		m_PreviousGestureID = m_CurrentGestureID;
		m_CurrentGestureID = EmoteConstants.ID_EMOTE_SURRENDER;
		if (state)
		{
			if (m_Player.GetItemInHands() && !m_Player.CanDropEntity(m_Player.GetItemInHands()))
				return;
			
			if (m_Player.GetItemInHands() && GetGame().IsClient())
			{
				if (m_Player.GetInventory().HasInventoryReservation(null, m_HandInventoryLocation))
					m_Player.GetInventory().ClearInventoryReservationEx(null, m_HandInventoryLocation);
				m_Player.PhysicalPredictiveDropItem(m_Player.GetItemInHands());
			}
			
			CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SURRENDERIN,DayZPlayerConstants.STANCEMASK_ALL,true);
			
			if (m_Callback)
				m_Callback.RegisterAnimationEvent("ActionExec", UA_ANIM_EVENT);
		}
		else
		{
			if (m_Player.IsAlive() && !m_Player.IsUnconscious())
			{
				CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SURRENDEROUT,DayZPlayerConstants.STANCEMASK_ALL,true);
			}
			else
			{
				OnEmoteEnd();
			}
		}
	}
	
	//!
	void SetEmoteLockState(bool state)
	{
		//separate inventory access locking
		if (state != m_InventoryAccessLocked)
		{
			m_Player.SetInventorySoftLock(state);
			m_InventoryAccessLocked = state;
		}
		
		//Movement lock in fullbody anims
		if (state && m_Callback && m_Callback.m_IsFullbody)
			m_controllsLocked = true;
		else
			m_controllsLocked = false;
		
		if (state == m_EmoteLockState)
			return;
		
		if (!m_HandInventoryLocation)
		{
			m_HandInventoryLocation = new InventoryLocation;
			m_HandInventoryLocation.SetHands(m_Player,null);
		}
		
		if (!state)
		{
			if (m_Player.GetInventory().HasInventoryReservation(null, m_HandInventoryLocation))
				m_Player.GetInventory().ClearInventoryReservationEx(null, m_HandInventoryLocation);
			
			if (m_Player.GetActionManager())
				m_Player.GetActionManager().EnableActions(true);
		}
		else
		{
			if (!m_Player.GetInventory().HasInventoryReservation(null, m_HandInventoryLocation))
				m_Player.GetInventory().AddInventoryReservationEx(null, m_HandInventoryLocation, GameInventory.c_InventoryReservationTimeoutMS);
				
			if (m_Player.GetActionManager())
				m_Player.GetActionManager().EnableActions(false);
		}
		m_EmoteLockState = state;
	}
	
	void CheckEmoteLockedState()
	{
		if (!m_Player.GetItemInHands() || (m_Player.GetItemInHands() && !SurrenderDummyItem.Cast(m_Player.GetItemInHands())))
			return;
		
		//refreshes reservation in case of unwanted timeout
		if (m_EmoteLockState && m_HandInventoryLocation)
		{
			m_Player.GetInventory().ExtendInventoryReservationEx(null, m_HandInventoryLocation, 10000);
		}
	}
	
	//! directly force-ends surrender state from outside of normal flow
	void EndSurrenderRequest(SurrenderData data = null)
	{
		if (m_IsSurrendered && data)
		{
			if (m_Player.GetItemInHands())
				m_Player.GetItemInHands().DeleteSafe();//Note, this keeps item 'alive' until it is released by all the systems (inventory swapping etc.)
			
			m_IsSurrendered = false;
			SetEmoteLockState(IsEmotePlaying());
			data.End();
		}
	}
	
	//sent from server to cancel generic emote callback
	void ServerRequestEmoteCancel()
	{
		ScriptJunctureData pCtx = new ScriptJunctureData;
		pCtx.Write(CALLBACK_CMD_GESTURE_INTERRUPT);
		
		m_Player.SendSyncJuncture(DayZPlayerSyncJunctures.SJ_GESTURE_REQUEST, pCtx);
	}
	
	bool InterruptGestureCheck()
	{
		//init pass
		if (!m_InterruptInputDirect)
		{
			m_InterruptInputDirect = new array<UAInput>;
			m_InterruptInputsCount = m_InterruptInputs.Count();
			
			for (int i = 0; i < m_InterruptInputsCount; i++)
			{
				m_InterruptInputDirect.Insert(GetUApi().GetInputByName(m_InterruptInputs[i]));
			}
		}
		
		//interrupts any callback if restrain action is in progress, takes priority
		if (!m_Callback.m_IsFullbody)
			return false;
		
		for (int idx = 0; idx < m_InterruptInputsCount; idx++)
		{
			if (m_InterruptInputDirect[idx].LocalPress())
			{
				return true;
			}
		}
		
		return false;
	}
	
	bool InterruptWaterCheck()
	{
		float waterLevel = m_Player.GetCurrentWaterLevel();
		
		if (m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_PRONE) && waterLevel >= m_HumanSwimSettings.m_fToCrouchLevel)
		{
			return true;
		}
		else if (m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEIDX_RAISEDCROUCH) && waterLevel >= m_HumanSwimSettings.m_fToErectLevel)
		{
			return true;
		}
		
		return m_Player.GetModifiersManager() && m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_DROWNING);
	}
	
	bool IsEmotePlaying()
	{
		return m_bEmoteIsPlaying || m_IsSurrendered || m_bEmoteIsRequestPending;
	}
	
	map<int,ref EmoteBase> GetNameEmoteMap()
	{
		return m_NameEmoteMap;
	}
	
	void CreateBleedingEffect(int Callback_ID)
	{
		if (GetGame().IsServer() && m_Player.IsAlive())
		{
			switch (Callback_ID)
			{
				case DayZPlayerConstants.CMD_SUICIDEFB_1HD :
					m_Player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("Neck");
				break;
				
				case DayZPlayerConstants.CMD_SUICIDEFB_FIREAXE :
					m_Player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftForeArmRoll");
				break;
				
				case DayZPlayerConstants.CMD_SUICIDEFB_PITCHFORK :
					m_Player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("Spine2");
				break;
				
				case DayZPlayerConstants.CMD_SUICIDEFB_SWORD :
					m_Player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("Spine2");
				break;
				
				case DayZPlayerConstants.CMD_SUICIDEFB_SPEAR :
					m_Player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("Head");
				break;
				
				case DayZPlayerConstants.CMD_SUICIDEFB_WOODAXE :
					m_Player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftForeArmRoll");
				break;
				
				default :
					ErrorEx("EmoteManager.c | CreateBleedingEffect | WRONG ID",ErrorExSeverity.INFO);
				break;
			}
		}
	}
	
/////////////////////////////////////////////////////////
	//! Deprecated
	protected void PickEmote(int gestureslot)
	{
	}
};

class SurrenderData
{
	//!called on surrender end request end
	void End();
}

/**@class		ChainedDropAndKillPlayerLambda
 * @brief		drops weapon in hands to ground and then calls kill
 **/
class ChainedDropAndKillPlayerLambda : ReplaceItemWithNewLambdaBase
{
	PlayerBase m_Player;

	void ChainedDropAndKillPlayerLambda (EntityAI old_item, string new_item_type, PlayerBase player)
	{
		m_Player = player;
		InventoryLocation gnd = new InventoryLocation;
		vector mtx[4];
		old_item.GetTransform(mtx);
		gnd.SetGround(old_item, mtx);
		
		OverrideNewLocation(gnd);
	}
		
	override void Execute (HumanInventoryWithFSM fsm_to_notify = null)
	{
		if (PrepareLocations())
		{
			vector transform[4];
		
			m_Player.GetInventory().TakeToDst(InventoryMode.SERVER, m_OldLocation, m_NewLocation);
			m_Player.OnItemInHandsChanged();			
		}
		
		m_Player.SetHealth(0);
		
		if (LogManager.IsInventoryHFSMLogEnable()) hndDebugPrint("[syncinv] player=" + Object.GetDebugName(m_Player) + " STS = " + m_Player.GetSimulationTimeStamp() + " ChainedDropAndKillPlayerLambda");
	}
}
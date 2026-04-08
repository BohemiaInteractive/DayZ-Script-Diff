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
		
		if (g_Game && m_player)
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
				
				if (g_Game.IsServer())
					m_Manager.KillPlayer();
				
				m_Manager.LogSuicide();
			break;
			
			case UA_ANIM_EVENT :
				if (m_callbackID == DayZPlayerConstants.CMD_GESTUREFB_SURRENDERIN)
					m_Manager.m_ItemToBeCreated = true;
			break;
			
			case EmoteConstants.EMOTE_SUICIDE_BLEED :
				if (g_Game.IsServer())
					m_Manager.CreateBleedingEffect(m_callbackID);
			break;
			
			case EmoteConstants.EMOTE_SUICIDE_SIMULATION_END:
				m_player.DeathDropHandEntity();
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
	protected bool m_IsStartGuaranteed; //remains in queue until played or canceled directly (logout sit pose etc.)
	protected bool m_SyncRequestSent;
	protected int m_ForcePlayEmote;
	protected int m_ID;
	
	void EmoteLauncher(int emoteID, bool interrupts_same)
	{
		m_ID = emoteID;
		m_InterruptsSameIDEmote = interrupts_same;
		m_ForcePlayEmote = FORCE_NONE;
		m_IsStartGuaranteed = false;
		m_SyncRequestSent = false;
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
	
	void SetStartGuaranteed(bool guaranted)
	{
		m_IsStartGuaranteed = guaranted;
	}
	
	bool IsStartGuaranteed()
	{
		return m_IsStartGuaranteed;
	}
	
	void VerifySyncRequest()
	{
		m_SyncRequestSent = true;
	}
	
	bool WasSynced()
	{
		return m_SyncRequestSent;
	}
}

class EmoteManager
{
	PlayerBase 				m_Player;
	ItemBase				m_item;
	EmoteCB					m_Callback;
	HumanInputController 	m_HIC;
	ref array<string> 		m_InterruptInputs;
	ref array<UAIDWrapper> 	m_InterruptInputDirect;
	ref InventoryLocation 	m_HandInventoryLocation;
	ref EmoteLauncher 		m_MenuEmote;
	bool					m_bEmoteIsRequestPending;
	bool					m_bEmoteIsPlaying;
	bool 					m_IsSurrendered;
	bool 					m_ItemToBeCreated;
	bool 					m_CancelEmote;
	bool 					m_InstantCancelEmote;
	bool 					m_GestureInterruptInput;
	protected bool 			m_DisconnectEmoteQueued; //reaction to client logging out
	protected bool 			m_DeferredEmoteLauncherCleanup; //safe cleanup from the outside
	protected bool			m_ItemToHands; //deprecated
	protected bool			m_ItemIsOn;
	protected bool			m_MouseButtonPressed;
	protected bool 			m_PlayerDies;
	protected bool 			m_controllsLocked;
	protected bool 			m_InventoryAccessLocked;
	protected bool 			m_EmoteLockState;
	protected int 			m_DeferredGuaranteedEmoteId; //secondary emote ID to ensure playback
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
		m_DeferredEmoteLauncherCleanup = false;
		m_DisconnectEmoteQueued = false;
		m_RPSOutcome = -1;
		m_DeferredEmoteExecution = CALLBACK_CMD_INVALID;
		m_DeferredGuaranteedEmoteId = -1;
		
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
		
		if (g_Game.IsServer())
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
	
	void SetPending(bool state)
	{
		m_bEmoteIsRequestPending = state;
	}
	
	//Called from players commandhandler each frame, checks input
	void Update(float deltaT)
	{
		// no updates on restrained characters
		if (m_Player.IsRestrained())
			return;
		
		if (m_ItemToBeCreated)
		{
			if (g_Game.IsServer() && m_Callback && !m_Player.GetItemInHands())
			{
				m_Player.GetHumanInventory().CreateInHands("SurrenderDummyItem");
			}
			m_ItemToBeCreated = false;
		}
		
		int gestureSlot = 0;
		#ifndef SERVER
			gestureSlot = DetermineGestureIndex();
		#endif
		
		//nuclear solution to inventory question
		if (g_Game.IsClient() && g_Game.GetUIManager().FindMenu(MENU_INVENTORY) && IsEmotePlaying())
			m_Player.CloseInventoryMenu();
		
		//deferred emote cancel
		if (m_InstantCancelEmote) //'hard' cancel
		{
			if (m_Callback)
				m_Callback.Cancel();
			
			ClearEmoteLauncher();
			ClearDeferredExecution();
			m_InstantCancelEmote = false;
			SetPending(false);
			
			if (m_IsSurrendered)
				ClearSurrenderState();
			else
				SetEmoteLockState(false);
		}
		else if (m_CancelEmote) //'soft' cancel
		{
			SetPending(false);
			m_CancelEmote = false;
			
			if (m_IsSurrendered)
				ClearSurrenderState();
			else if (m_Callback)
				m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
		}
		
		if (m_DeferredEmoteLauncherCleanup)
		{
			ClearEmoteLauncher(true);
			m_DeferredEmoteLauncherCleanup = false;
		}
		
		//if (m_DisconnectEmoteQueued && (!m_bEmoteIsPlaying && !m_bEmoteIsRequestPending))
		if (m_DisconnectEmoteQueued)
		{
			if (!m_MenuEmote || !m_MenuEmote.IsStartGuaranteed())
			{
				CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SITA);
				m_MenuEmote.SetForced(EmoteLauncher.FORCE_DIFFERENT);
				m_MenuEmote.SetStartGuaranteed(true);
				
				EmoteBase emote = EmoteBase.Cast(m_NameEmoteMap.Get(m_MenuEmote.GetID()));
				int stanceIdx = DayZPlayerUtils.ConvertStanceMaskToStanceIdx(emote.GetStanceMaskFullbody());
			
				//if the emote cannot be played, don't sent a request to server
				if (!DayZPlayerUtils.PlayerCanChangeStance(m_Player, stanceIdx))
				{
					m_MenuEmote = null; //setting this to null also ensures that the forcedEmotQueuedClient below is also false
					m_DisconnectEmoteQueued = false;
				}
			}
		}
		
		bool forcedEmoteQueuedClient = m_MenuEmote && !m_MenuEmote.WasSynced() && m_MenuEmote.GetForced() > EmoteLauncher.FORCE_NONE && !g_Game.IsDedicatedServer();
		if (forcedEmoteQueuedClient) //regular forced emote playing
		{
			SendEmoteRequestSyncEx(m_MenuEmote);
		}
		else if (m_Callback)
		{
			bool uiGesture = false;
			if (!g_Game.IsDedicatedServer())
			{
				uiGesture = g_Game.GetUIManager().IsMenuOpen(MENU_GESTURES);
				
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
			
			if (m_MenuEmote && !m_MenuEmote.IsStartGuaranteed() && m_bEmoteIsPlaying)
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
				SetPending(false);
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
			// getting out of surrender state
			else if (m_IsSurrendered && m_Player.GetItemInHands() && (m_HIC.IsSingleUse() || m_HIC.IsContinuousUseStart() || m_HIC.IsWeaponRaised()))
			{
				if (m_Player.GetItemInHands())
					m_Player.GetItemInHands().DeleteSafe();//Note, this keeps item 'alive' until it is released by all the systems (inventory swapping etc.)
				return;
			}
			// fallback in case lock does not end properly
			else if (m_IsSurrendered && (!m_Player.GetItemInHands() || (m_Player.GetItemInHands() && m_Player.GetItemInHands().GetType() != "SurrenderDummyItem" && m_EmoteLockState)))
			{
				ClearSurrenderState();
				return;
			}
			//actual emote launch
			else if (m_DeferredEmoteExecution > CALLBACK_CMD_INVALID)
			{
				PlayEmote(m_DeferredEmoteExecution);
				//m_DeferredGuaranteedEmoteId = CALLBACK_CMD_INVALID;
			}
			else if (m_DeferredGuaranteedEmoteId > CALLBACK_CMD_INVALID)
			{
				PlayEmote(m_DeferredGuaranteedEmoteId);
				m_DeferredGuaranteedEmoteId = CALLBACK_CMD_INVALID;
			}
			//client-side emote launcher
			else if (!m_bEmoteIsPlaying && m_MenuEmote && !m_MenuEmote.WasSynced() && !g_Game.IsDedicatedServer())
			{
				SendEmoteRequestSyncEx(m_MenuEmote);
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
		
		m_CurrentGestureID = 0;
		m_bEmoteIsPlaying = false;
		SetPending(false);
		
		if (m_IsSurrendered)
			return;
		
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
			int cancelID = 0;
			int random = -1;
			bool guaranteed = false;
			
			if (!ctx.Read(gestureID))
			{
				cancelID = CALLBACK_CMD_INSTACANCEL; //fallback solution in case of severe data corruption
			}
			else
			{
				ctx.Read(forced);
				ctx.Read(guaranteed);
				if (ctx.Read(random))
					m_RPSOutcome = random;
				//server-side check, sends CALLBACK_CMD_INSTACANCEL as a fail
				bool canPlay = CanPlayEmote(gestureID);
				if (!canPlay && (forced == EmoteLauncher.FORCE_NONE || !m_Player.GetCommand_Move())) //HOTFIX for swim and such
				{
					gestureID = CALLBACK_CMD_INVALID;
					cancelID = CALLBACK_CMD_INSTACANCEL;
				}
			}
			
			ScriptJunctureData pCtx = new ScriptJunctureData;
			pCtx.Write(gestureID);
			pCtx.Write(cancelID);
			pCtx.Write(forced);
			pCtx.Write(guaranteed);
			m_Player.SendSyncJuncture(DayZPlayerSyncJunctures.SJ_GESTURE_REQUEST, pCtx);
			SetPending(true);
			
			return true;
		}
		return false;
	}
	
	//server and client
	void OnSyncJuncture(int pJunctureID, ParamsReadContext pCtx)
	{
		int gestureID;
		int cancelID;
		int forced;
		bool guaranteed;
		
		if (!m_CancelEmote)
		{
			pCtx.Read(gestureID);
			pCtx.Read(cancelID);
			pCtx.Read(forced);
			pCtx.Read(guaranteed);
			
			if (gestureID != 0) 
			{
				// Check if player's state has changed since request was sent
				if (m_Player.IsSwimming())
				{
					// Player is now in water - reject the emote
					gestureID = CALLBACK_CMD_INVALID;
					cancelID = CALLBACK_CMD_INSTACANCEL;
					m_InstantCancelEmote = true;
					m_DeferredEmoteExecution = CALLBACK_CMD_INVALID;
					m_DeferredGuaranteedEmoteId = CALLBACK_CMD_INVALID;
					// Notify server about it
					if (g_Game.IsClient())
					{
						SendEmoteRequestSync(CALLBACK_CMD_INSTACANCEL); // Tell server we're cancelling
					}
					return;
				}
			}
			
			EmoteBase emoteData;
			if ((m_Callback || m_IsSurrendered) && (forced == EmoteLauncher.FORCE_ALL || (forced == EmoteLauncher.FORCE_DIFFERENT && m_CurrentGestureID != gestureID)))
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
			
			if (cancelID == CALLBACK_CMD_INSTACANCEL)
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
			
			if (guaranteed)
				m_DeferredGuaranteedEmoteId = gestureID;
			m_DeferredEmoteExecution = gestureID;
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
		ClearEmoteLauncher();
		if (id == m_DeferredGuaranteedEmoteId)
			m_DeferredGuaranteedEmoteId = CALLBACK_CMD_INVALID;
		ClearDeferredExecution();
		SetPending(false);
		
		//if (CanPlayEmote(id))
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
		
		SetEmoteLockState(IsEmotePlaying());
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
		if (!g_Game.IsClient())
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
					if (helm && g_Game.IsServer())
					{
						float damage = helm.GetMaxHealth("","");
						helm.AddHealth("","", -damage/2);
					}
					g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.KillPlayer);
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
		if (g_Game.IsServer())
		{
			m_Player.SetHealth(0);
		}
	}
	
	void LogSuicide()
	{
		if (g_Game.IsServer())
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
		if (ClearEmoteLauncher())
			m_MenuEmote = new EmoteLauncher(id,interrupts_same);
	}
	
	protected bool ClearEmoteLauncher(bool forced = false)
	{
		if (!forced && m_MenuEmote && m_MenuEmote.IsStartGuaranteed())
			return false;
		
		m_MenuEmote = null;
		return true;
	}
	
	protected bool ClearDeferredExecution()
	{
		m_DeferredEmoteExecution = CALLBACK_CMD_INVALID;
		return true;
	}
	
	EmoteLauncher GetEmoteLauncher()
	{
		return m_MenuEmote;
	}
	
	void InterruptCallbackCommand()
	{
		m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
		
		ClearEmoteLauncher();
		ClearDeferredExecution();
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
		
		ClearEmoteLauncher();
		ClearDeferredExecution();
	}
	
	//sends request (client)
	void SendEmoteRequestSync(int id)
	{
		int forced = EmoteLauncher.FORCE_NONE;
		bool guaranteedLaunch = false;
		
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
		if (g_Game.IsMultiplayer() && g_Game.IsClient())
		{
			bool canProceed = true; //running callbacks in certain state can block additional actions
			EmoteBase emoteData = m_NameEmoteMap.Get(m_CurrentGestureID);
			if (m_Callback && emoteData)
			{
				canProceed = emoteData.CanBeCanceledNormally(m_Callback);
			}
			
			if (ctx.CanStoreInputUserData() && ((CanPlayEmote(id) && CanPlayEmoteClientCheck(id)) || forced) && canProceed)
			{
				ctx.Write(INPUT_UDT_GESTURE);
				ctx.Write(id);
				ctx.Write(forced);
				ctx.Write(guaranteedLaunch);
				if (m_RPSOutcome != -1)
				{
					ctx.Write(m_RPSOutcome);
				}
				ctx.Send();
				SetPending(true);
			}
			else
			{
				SetPending(false);
			}
			
			SetEmoteLockState(IsEmotePlaying());
		}
		else if (!g_Game.IsMultiplayer())
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
		}
	}
	
	//! To avoid mixing m_MenuEmote with exceptional sync commands
	protected void SendEmoteRequestSyncEx(notnull EmoteLauncher launcher)
	{
		int id = launcher.GetID();
		int forced = launcher.GetForced();
		bool guaranteedLaunch = launcher.IsStartGuaranteed();
		
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
		if (g_Game.IsMultiplayer() && g_Game.IsClient())
		{
			bool canProceed = true; //running callbacks in certain state can block additional actions
			EmoteBase emoteData = m_NameEmoteMap.Get(m_CurrentGestureID);
			if (m_Callback && emoteData)
			{
				canProceed = emoteData.CanBeCanceledNormally(m_Callback);
			}
			
			if (ctx.CanStoreInputUserData() && ((CanPlayEmote(id) && CanPlayEmoteClientCheck(id)) || forced) && canProceed)
			{
				ctx.Write(INPUT_UDT_GESTURE);
				ctx.Write(id);
				ctx.Write(forced);
				ctx.Write(guaranteedLaunch);
				if (m_RPSOutcome != -1)
				{
					ctx.Write(m_RPSOutcome);
				}
				ctx.Send();
				launcher.VerifySyncRequest();
				SetPending(true);
			}
			else
			{
				SetPending(false);
			}
			
			if (launcher == m_MenuEmote)
				ClearEmoteLauncher();
			SetEmoteLockState(IsEmotePlaying());
		}
		else if (!g_Game.IsMultiplayer())
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
			
			if (launcher == m_MenuEmote)
				ClearEmoteLauncher();
			SetEmoteLockState(IsEmotePlaying());
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
			return false;
		}
		
		ItemBase item = m_Player.GetItemInHands();
		if (item)
		{
			if (item.IsHeavyBehaviour() && id != EmoteConstants.ID_EMOTE_SURRENDER)
			{
				return false;
			}
			
			SurrenderDummyItem sda;
			if (m_Player.IsItemsToDelete() && Class.CastTo(sda,item) && !sda.IsSetForDeletion())
			{
				return false;
			}
		}
		
		if ((m_Player.GetWeaponManager() && m_Player.GetWeaponManager().IsRunning()) || (m_Player.GetActionManager() && m_Player.GetActionManager().GetRunningAction()))
		{
			return false;
		}
		
		if (m_HIC.IsWeaponRaised() || m_Player.IsRolling() || m_Player.IsClimbing() || m_Player.IsRestrainStarted() || m_Player.IsFighting() || m_Player.IsSwimming() || m_Player.IsClimbingLadder() || m_Player.IsFalling() || m_Player.IsUnconscious() || m_Player.IsJumpInProgress() || m_Player.IsRestrained()) 	// rework conditions into something better?
		{
			return false;
		}
		
		if (m_Player.GetCommand_Vehicle())
		{
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
			return false;
		}
		
		if (m_Player.GetDayZPlayerInventory().IsProcessing())
		{
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
		}
		
		return false;
	}
	
	bool CanPlayEmoteClientCheck(int id)
	{
		if (!g_Game.IsClient())
			return true;
		
		if (g_Game.GetUIManager().FindMenu(MENU_INVENTORY))
		{
			return false;
		}
		
		return true;
	}
	
	// is player able to perform the animation at the moment
	/*bool CanPerformEmoteAnimation(int id)
	{
		
	}*/
	
	void PlaySurrenderInOut(bool state)
	{
		m_PreviousGestureID = m_CurrentGestureID;
		m_CurrentGestureID = EmoteConstants.ID_EMOTE_SURRENDER;
		
		if (state)
		{
			ItemBase item = m_Player.GetItemInHands();
			if (item)
			{
				if (!m_Player.CanDropEntity(item))
					return;
				
				if (m_Player.GetInventory().HasInventoryReservation(null, m_HandInventoryLocation))
					m_Player.GetInventory().ClearInventoryReservationEx(null, m_HandInventoryLocation);
				
				if (g_Game.IsMultiplayer())
				{
					if (g_Game.IsServer())
						m_Player.ServerDropEntity(item);
				}
				else
				{
					m_Player.PhysicalPredictiveDropItem(item); //SP only
				}
			}
			
			CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SURRENDERIN,DayZPlayerConstants.STANCEMASK_ALL,true);
			
			if (m_Callback)
			{
				m_Callback.RegisterAnimationEvent("ActionExec", UA_ANIM_EVENT);
			}
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
		
		if (g_Game.IsClient() && m_InventoryAccessLocked && g_Game.GetUIManager().FindMenu(MENU_INVENTORY))
			m_Player.CloseInventoryMenu();
		
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
	
	//! directly force-ends surrender state AND requests hard cancel
	void EndSurrenderRequest(SurrenderData data = null)
	{
		if (m_IsSurrendered && data)
		{
			PostSurrenderRequestServer();
			data.End();
		}
	}
	
	void SetClientLoggingOut(bool state)
	{
		m_DisconnectEmoteQueued = state;
		
		if (!state)
		{
			if (m_MenuEmote)
				m_DeferredEmoteLauncherCleanup = true;
		}
	}
	
	//! Queues item deletion only, surrender state cleared from item event directly
	protected void ClearSurrenderState()
	{
		if (m_IsSurrendered)
		{
			SurrenderDummyItem dummyItem = SurrenderDummyItem.Cast(m_Player.GetItemInHands());
			if (dummyItem)
				dummyItem.DeleteSafe();
			
			m_IsSurrendered = false;
			SetEmoteLockState(false);
			m_ItemToBeCreated = false;
		}
	}
	
	void ForceSurrenderState(bool state)
	{
		m_IsSurrendered = state;
		SetEmoteLockState(IsEmotePlaying());
	}
	
	//! server only
	protected void PostSurrenderRequestServer()
	{
		if ((g_Game.IsMultiplayer() && g_Game.IsServer()) || !g_Game.IsMultiplayer())
		{
			ScriptJunctureData pCtx = new ScriptJunctureData;
			pCtx.Write(CALLBACK_CMD_INSTACANCEL);
			pCtx.Write(EmoteLauncher.FORCE_ALL);
			m_Player.SendSyncJuncture(DayZPlayerSyncJunctures.SJ_GESTURE_REQUEST, pCtx);
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
			m_InterruptInputDirect = new array<UAIDWrapper>;
			m_InterruptInputsCount = m_InterruptInputs.Count();
			
			for (int i = 0; i < m_InterruptInputsCount; i++)
			{
				m_InterruptInputDirect.Insert(GetUApi().GetInputByName(m_InterruptInputs[i]).GetPersistentWrapper());
			}
		}
		
		//interrupts any callback if restrain action is in progress, takes priority
		if (!m_Callback.m_IsFullbody)
			return false;
		
		for (int idx = 0; idx < m_InterruptInputsCount; idx++)
		{
			if (m_InterruptInputDirect[idx].InputP().LocalPress())
			{
				return true;
			}
		}
		
		return false;
	}
	
	bool InterruptWaterCheck()
	{
		float waterLevel = m_Player.GetCurrentWaterLevel();
		
		if (m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_RAISEDPRONE) && waterLevel >= m_HumanSwimSettings.m_fToCrouchLevel)
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
		if (g_Game.IsServer() && m_Player.IsAlive())
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
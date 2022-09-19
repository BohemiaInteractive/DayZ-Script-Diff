class EmoteCB : HumanCommandActionCallback
{
	//const int 		CB_SURRENDER_STATE = 100;
	bool 			m_IsFullbody;
	int 			m_callbackID;
	PlayerBase 		m_player;
	EmoteManager 	m_Manager;
	
	void ~EmoteCB()
	{
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
			break;
			
			case UA_ANIM_EVENT :
				if (GetGame().IsServer())
				{
					if ( m_player.GetItemInHands() && SurrenderDummyItem.Cast(m_player.GetItemInHands()) )
						m_player.GetItemInHands().Delete();
					
					if ( m_player.GetItemInHands() )
					{
						m_player.PhysicalPredictiveDropItem(m_player.GetItemInHands());
					}
				}
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
	ref InventoryLocation 	m_HandInventoryLocation;
	ref EmoteLauncher 		m_MenuEmote;
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
	protected bool 			m_EmoteLockState;
	protected int 			m_DeferredEmoteExecution;
	protected int  			m_GestureID;
	protected int			m_PreviousGestureID;
	protected int			m_CurrentGestureID;
	protected int 			m_LastMask;
	protected int 			m_RPSOutcome;
	protected const int 	CALLBACK_CMD_INVALID = -1;
	protected const int 	CALLBACK_CMD_END = -2;
	protected const int 	CALLBACK_CMD_GESTURE_INTERRUPT = -3;
	protected const int 	CALLBACK_CMD_INSTACANCEL = -4;
	PluginAdminLog 			m_AdminLog;
	protected ref Timer 				m_ReservationTimer;
	protected ref map<int, ref EmoteBase>		m_NameEmoteMap; //<emote_ID,EmoteBase>
	protected ref array<ref EmoteBase>			m_EmoteClassArray; //registered 'EmoteBase' object refs
	protected ref array<int>					m_EmoteIDs; //IDs of registered emotes (found in 'EmoteConstants'). For quick access.
	protected ref array<int>					m_EmoteInputIDs; //input IDs for the registered 'EmoteBase' objects
	
	void EmoteManager( PlayerBase player ) 
	{
		m_Player = player;
		m_HIC = m_Player.GetInputController();
		m_ItemIsOn = false;
		m_RPSOutcome = -1;
		m_DeferredEmoteExecution = CALLBACK_CMD_INVALID;
		
		// new input-based interrupt actions
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
		// end of new input-based interrupt actions
		
		m_HandInventoryLocation = new InventoryLocation;
		m_HandInventoryLocation.SetHands(m_Player,null);
		
		if (GetGame().IsServer())
		{
			m_AdminLog = PluginAdminLog.Cast( GetPlugin(PluginAdminLog) );
		}
		
		m_ReservationTimer = new Timer();
		m_ReservationTimer.Run(8,this,"CheckEmoteLockedState",NULL,true);
		
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
			if ( !m_Player.GetItemInHands() && GetGame().IsServer() )
			{
				m_Player.GetHumanInventory().CreateInHands("SurrenderDummyItem");
			}
			m_ItemToBeCreated = false;
		}
		
		int gestureSlot = DetermineGestureIndex();
		
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
			
			m_CancelEmote = false;
		}
		
		if (m_MenuEmote && m_MenuEmote.GetForced() > EmoteLauncher.FORCE_NONE && (!GetGame().IsDedicatedServer())) //forced emote playing
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
				if (m_CurrentGestureID == EmoteConstants.ID_EMOTE_SUICIDE  && m_HIC.IsSingleUse())
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
				else if ( (m_CurrentGestureID == EmoteConstants.ID_EMOTE_THUMB || m_CurrentGestureID == EmoteConstants.ID_EMOTE_THUMBDOWN) && m_HIC.IsSingleUse() )
				{
					m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_ACTION);
				}
				else if ( m_HIC.IsSingleUse() && (m_CurrentGestureID == EmoteConstants.ID_EMOTE_RPS || m_CurrentGestureID == EmoteConstants.ID_EMOTE_RPS_R || m_CurrentGestureID == EmoteConstants.ID_EMOTE_RPS_P || m_CurrentGestureID == EmoteConstants.ID_EMOTE_RPS_S) )
				{
					if ( m_RPSOutcome != -1 )
					{
						if (m_RPSOutcome == 0) 			m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_RPS_ROCK);
						else if (m_RPSOutcome == 1) 	m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_RPS_PAPER);
						else if (m_RPSOutcome == 2) 	m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_RPS_SCISSORS);
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
			
			//HOTFIX for stance changing in additive emotes
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
			
			if ( m_DeferredEmoteExecution == CALLBACK_CMD_GESTURE_INTERRUPT )
			{
				InterruptCallbackCommand();
			}
		}
		//no m_Callback exists //TODO please future me, refactor into some sort of switch
		else
		{
			if (m_bEmoteIsPlaying)
			{
				m_bEmoteIsPlaying = false;
				OnEmoteEnd();
			}
			else if (!m_Player.GetItemInHands() && m_IsSurrendered && !m_ItemToBeCreated)
			{
				PlaySurrenderInOut(false);
				return;
			}
			// getting out of surrender state - hard cancel
			else if (m_IsSurrendered && (m_HIC.IsSingleUse() || m_HIC.IsContinuousUseStart() || m_HIC.IsWeaponRaised()))
			{
				if ( m_Player.GetItemInHands() )
					m_Player.GetItemInHands().Delete();
				return;
			}
			
			// fallback in case lock does not end properly
			else if (m_IsSurrendered && !m_Player.GetItemInHands() || (m_Player.GetItemInHands() && m_Player.GetItemInHands().GetType() != "SurrenderDummyItem" && m_EmoteLockState))
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
			else if (!m_bEmoteIsPlaying && m_MenuEmote && (!GetGame().IsDedicatedServer()))
			{
				SendEmoteRequestSync(m_MenuEmote.GetID());
			}
			else if (!m_MenuEmote && gestureSlot > 0)
			{
				CreateEmoteCBFromMenu(gestureSlot,true); //translation no longer needed
				//PickEmote(gestureSlot);
			}
		}
	}
	
	void OnEmoteEnd()
	{
		ShowItemInHands();
		
		if ( m_PlayerDies )
		{
			m_Player.SetHealth(0.0);
			return;
		}
		
		//surrender "state" switch
		if ( m_CurrentGestureID == EmoteConstants.ID_EMOTE_SURRENDER )
		{
			m_IsSurrendered = !m_IsSurrendered;
			SetEmoteLockState(m_IsSurrendered);
		}
		
		m_CurrentGestureID = 0;
		
		if ( m_IsSurrendered )
		{
			return;
		}
		m_GestureInterruptInput = false;
		SetEmoteLockState(false);

		//! back to the default - shoot from camera - if not set already
		if (!m_Player.IsShootingFromCamera()) m_Player.OverrideShootFromCamera(true);
	}
	
	//server-side
	bool OnInputUserDataProcess(int userDataType, ParamsReadContext ctx)
	{
		if ( userDataType == INPUT_UDT_GESTURE)
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
				
				//server-side check
				if (forced > EmoteLauncher.FORCE_NONE || CanPlayEmote(gestureID))
				{
					ScriptJunctureData pCtx = new ScriptJunctureData;
					pCtx.Write(gestureID);
					pCtx.Write(forced);
					
					m_Player.SendSyncJuncture(DayZPlayerSyncJunctures.SJ_GESTURE_REQUEST, pCtx);
				}
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
			
			if ( (m_Callback || m_IsSurrendered) && (forced == EmoteLauncher.FORCE_ALL || (forced == EmoteLauncher.FORCE_DIFFERENT && m_CurrentGestureID != gesture_id)) )
			{
				m_CancelEmote = true;
			}
			
			if (gesture_id == CALLBACK_CMD_INSTACANCEL)
			{
				m_InstantCancelEmote = true;
			}
			
			m_DeferredEmoteExecution = gesture_id;
		}
		else
			m_CancelEmote = false;
	}
	
	void AfterStoreLoad()
	{
		//failsafe, deletes helper item after fully loading character
		if ( m_Player.GetItemInHands() && SurrenderDummyItem.Cast(m_Player.GetItemInHands()) )
			m_Player.GetItemInHands().Delete();
	}
	
	//Configure Emote parameters for callback here
	bool PlayEmote( int id )
	{
		m_DeferredEmoteExecution = CALLBACK_CMD_INVALID;
		
		if (!CanPlayEmote(id))
		{
			m_Player.SetInventorySoftLock(false);
			return false;
		}
		
		actionDebugPrint("[emote] " + Object.GetDebugName(m_Player) + " play emote id=" + id + " IH=" + Object.GetDebugName(m_Player.GetItemInHands()));
		
		m_PreviousGestureID = m_CurrentGestureID;
		m_CurrentGestureID = id;
		if (id > 0)
		{
			//-----------------------------------------------
			EmoteBase emote;
			if (m_NameEmoteMap.Find(id,emote))
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
					Error("EmoteManager | DetermineEmoteData failed!");
				}
			}
		}
		
		if (m_bEmoteIsPlaying)
		{
			m_Player.SetInventorySoftLock(true);
			if (m_Callback.m_IsFullbody)
			{
				SetEmoteLockState(true);
			}
			
			return true;
		}
		
		return false;
	}
	
	//! Obsolete
	protected void PickEmote(int gestureslot)
	{
		/*if ( m_Player && (!GetGame().IsDedicatedServer()))
		{
			m_PreviousGestureID = gestureslot;
			if ( gestureslot == 12)
			{
				return;
			}
			
			int emote_id_from_slot = m_Player.m_ConfigEmotesProfile.GetEmoteFromSlot(gestureslot.ToString());
			CreateEmoteCBFromMenu(gestureslot,true);
		}*/
	}
	
	//creates Emote callback
	protected void CreateEmoteCallback(typename callbacktype, int id, int mask, bool fullbody )
	{
		if ( m_Player )
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
		if(m_Callback && m_item)
		{
			m_Player.TryHideItemInHands(true);
			//m_ItemToHands = true;
		}
	}
	
	protected void ShowItemInHands()
	{
		if (m_item)
		{
			m_Player.TryHideItemInHands(false);
		}
		//m_ItemToHands = false;
	}
	
	protected void CommitSuicide()
	{
		Weapon_Base weapon;
		WeaponEventBase weapon_event = new WeaponEventTrigger;
		
		if ( Weapon_Base.CastTo(weapon,m_Player.GetItemInHands()) )
		{
			//TODO : check multiple muzzles for shotguns, eventually
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
						helm.AddHealth( "","", -damage/2);
					}
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.KillPlayer);
					//LogSuicide(); 	// older logging
					if ( m_AdminLog )
					{
						m_AdminLog.Suicide( m_Player );
					}
				}
			}
			else
			{
				weapon.ProcessWeaponEvent(weapon_event);
				m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			}
		}
		else if ( m_Player.GetItemInHands() && m_Player.GetItemInHands().ConfigIsExisting("suicideAnim") )
		{
			m_Callback.RegisterAnimationEvent("Death",EmoteConstants.EMOTE_SUICIDE_DEATH);
			m_Callback.RegisterAnimationEvent("Bleed",EmoteConstants.EMOTE_SUICIDE_BLEED);
			m_Callback.RegisterAnimationEvent("Simulation_End",EmoteConstants.EMOTE_SUICIDE_SIMULATION_END);
			m_Player.SetSuicide(true);
			m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
			
			LogSuicide();
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
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(GetGame().AdminLog, "Player '" + identity.GetName() + "' (id=" + identity.GetId() + ") committed suicide.");
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
		//if (!m_CancelEmote)
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
	
	//sends request
	void SendEmoteRequestSync(int id)
	{
		int forced = EmoteLauncher.FORCE_NONE;
		if (m_MenuEmote)
		{
			forced = m_MenuEmote.GetForced();
		}
		
		//RPS handling. TODO: move
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
		if ( GetGame().IsMultiplayer() && GetGame().IsClient() )
		{
			if ( ctx.CanStoreInputUserData() && (CanPlayEmote(id) || forced) )
			{
				ctx.Write(INPUT_UDT_GESTURE);
				ctx.Write(id);
				ctx.Write(forced);
				if (m_RPSOutcome != -1)
				{
					ctx.Write(m_RPSOutcome);
				}
				ctx.Send();
				
				//TODO: investigate callstack to prevent multiple locking attempts (wasteful)
				if (id > 0)
				{
					m_Player.SetInventorySoftLock(true);
				}
			}
			m_MenuEmote = NULL;
		}
		else if (!GetGame().IsMultiplayer())
		{
			if (id == CALLBACK_CMD_END )
			{
				EndCallbackCommand();
			}
			else if (id == CALLBACK_CMD_GESTURE_INTERRUPT)
			{
				m_DeferredEmoteExecution = CALLBACK_CMD_GESTURE_INTERRUPT;
			}
			else
			{
				PlayEmote(id);
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
		if ( id == CALLBACK_CMD_END || id == CALLBACK_CMD_GESTURE_INTERRUPT || id == CALLBACK_CMD_INSTACANCEL )
		{
			return true;
		}
		
		if ( !m_Player || (!IsEmotePlaying() && m_Player.GetCommand_Action() || m_Player.GetCommandModifier_Action()) || m_Player.GetThrowing().IsThrowingModeEnabled() || m_Player.IsItemsToDelete())
		{	
			return false;
		}
		
		ItemBase item = m_Player.GetItemInHands();
		if ( item && item.IsHeavyBehaviour() && (id != EmoteConstants.ID_EMOTE_SURRENDER) )
		{
			return false;
		}
		
		if ( (m_Player.GetWeaponManager() && m_Player.GetWeaponManager().IsRunning()) || (m_Player.GetActionManager() && m_Player.GetActionManager().GetRunningAction()) )
		{
			return false;
		}
		
		if ( m_HIC.IsWeaponRaised() || m_Player.IsRolling() || m_Player.IsClimbing() || m_Player.IsRestrainStarted() || m_Player.IsFighting() || m_Player.IsSwimming() || m_Player.IsClimbingLadder() || m_Player.IsFalling() || m_Player.IsUnconscious() || m_Player.IsJumpInProgress() ) 	// rework conditions into something better?
		{
			return false;
		}
		if ( m_Player.GetCommand_Vehicle() )
		{
			if ( m_Player.GetCommand_Vehicle().GetTransport() )
			{
				return false;
			}
		}
		
		if ( m_Player.GetCommand_Move() && m_Player.GetCommand_Move().IsOnBack() && id != EmoteConstants.ID_EMOTE_SURRENDER)
		{
			return false;
		}
		
		//"locks" player in surrender state
		if ( m_IsSurrendered && (id != EmoteConstants.ID_EMOTE_SURRENDER) )
		{
			return false;
		}
		
		if ( m_Player.GetDayZPlayerInventory().IsProcessing() )
		{
			return false;
		}
		
		EmoteBase emote;
		if (m_NameEmoteMap.Find(id,emote))
		{
			int callback_ID;
			int stancemask;
			bool is_fullbody;
			if ( DetermineEmoteData(emote,callback_ID,stancemask,is_fullbody) && emote.EmoteCondition(stancemask) )
			{
				return true;
			}
		}
		
		return false;
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
				m_Player.PhysicalPredictiveDropItem(m_Player.GetItemInHands());
			}
			CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SURRENDERIN,DayZPlayerConstants.STANCEMASK_ALL,true);
			if (m_Callback)
				m_Callback.RegisterAnimationEvent("ActionExec", UA_ANIM_EVENT);
		}
		else
		{
			if ( m_Player.IsAlive() && !m_Player.IsUnconscious() )
			{
				CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SURRENDEROUT,DayZPlayerConstants.STANCEMASK_ALL,true);
			}
			else
			{
				m_bEmoteIsPlaying = false;
				OnEmoteEnd();
			}
		}
	}
	
	//!
	void SetEmoteLockState(bool state)
	{
		DayZPlayerCameraBase camera = DayZPlayerCameraBase.Cast(m_Player.GetCurrentCamera());
		if (!m_HandInventoryLocation)
		{
			m_HandInventoryLocation = new InventoryLocation;
			m_HandInventoryLocation.SetHands(m_Player,null);
		}
		
		if (!state)
		{
			if (m_Player.GetInventory().HasInventoryReservation(null, m_HandInventoryLocation))
			{
				m_Player.GetInventory().ClearInventoryReservationEx( null, m_HandInventoryLocation);
			}
			
			if ( m_Player.GetActionManager() )
				m_Player.GetActionManager().EnableActions(true);
			
			m_Player.SetInventorySoftLock(false);
						
			if ( m_controllsLocked )
			{
				m_controllsLocked = false;
				//camera.ForceFreelook(false);
				//m_Player.GetInputController().OverrideAimChangeX(false,0);
				//m_Player.GetInputController().OverrideMovementSpeed(false,0);
			}
		}
		else
		{
			//m_HandInventoryLocation.SetHands(m_Player,m_Player.GetItemInHands());
			if (!m_Player.GetInventory().HasInventoryReservation(null, m_HandInventoryLocation))
			{
				m_Player.GetInventory().AddInventoryReservationEx( null, m_HandInventoryLocation, GameInventory.c_InventoryReservationTimeoutMS);
			}
				
			if ( m_Player.GetActionManager() )
				m_Player.GetActionManager().EnableActions(false);
			
			//Movement lock in fullbody anims
			if (m_Callback && m_Callback.m_IsFullbody && !m_controllsLocked )
			{
				m_controllsLocked = true;
				//m_Player.GetInputController().OverrideAimChangeX(true,0);
				//m_Player.GetInputController().OverrideMovementSpeed(true,0);
				
				//camera.ForceFreelook(true);
			}
		}
		m_EmoteLockState = state;
	}
	
	void CheckEmoteLockedState()
	{
		if ( !m_Player.GetItemInHands() || (m_Player.GetItemInHands() && !SurrenderDummyItem.Cast(m_Player.GetItemInHands())) )
			return;
		
		//refreshes reservation in case of unwanted timeout
		if ( m_EmoteLockState && m_HandInventoryLocation )
		{
			m_Player.GetInventory().ExtendInventoryReservationEx( null, m_HandInventoryLocation, 10000);
		}
	}
	
	//! force-ends surrender state from outside of normal flow
	void EndSurrenderRequest(SurrenderData data = null)
	{
		if (m_IsSurrendered && data)
		{
			SetEmoteLockState(false);
			if (m_Player.GetItemInHands())
				m_Player.GetItemInHands().Delete();
			
			m_IsSurrendered = false;
			
			data.End();
		}
	}
	
	//sent from server to cancel general emote callback
	void ServerRequestEmoteCancel()
	{
		ScriptJunctureData pCtx = new ScriptJunctureData;
		pCtx.Write(CALLBACK_CMD_GESTURE_INTERRUPT);
		//pCtx.Write(CALLBACK_CMD_SERVER);
		
		m_Player.SendSyncJuncture(DayZPlayerSyncJunctures.SJ_GESTURE_REQUEST, pCtx);
	}
	
	bool InterruptGestureCheck()
	{
		//interrupts any callback if restrain action is in progress, takes priority
		if (!m_Callback.m_IsFullbody)
			return false;
		
		UAInput inp;
		for (int idx = 0; idx < m_InterruptInputs.Count(); idx++)
		{
			inp = GetUApi().GetInputByName(m_InterruptInputs[idx]);
			
			if (inp && inp.LocalPress())
			{
				return true;
			}
		}
		return false;
	}
	
	bool InterruptWaterCheck()
	{
		vector wl;
		int level = DayZPlayerUtils.CheckWaterLevel(m_Player,wl);
		
		if (m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_PRONE) && level > EWaterLevels.LEVEL_LOW)
		{
			return true;
		}
		else if (m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEIDX_RAISEDCROUCH) && level > EWaterLevels.LEVEL_CROUCH)
		{
			return true;
		}
		
		return m_Player.GetModifiersManager() && m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_DROWNING);
	}
	
	bool IsEmotePlaying()
	{
		return m_bEmoteIsPlaying || m_IsSurrendered;
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
		gnd.SetGround( old_item, mtx );
		
		OverrideNewLocation( gnd );
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
		
		hndDebugPrint("[syncinv] player=" + Object.GetDebugName(m_Player) + " STS = " + m_Player.GetSimulationTimeStamp() + " ChainedDropAndKillPlayerLambda");
	}
}
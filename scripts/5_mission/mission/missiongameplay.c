class MissionGameplay extends MissionBase
{
	int								m_LifeState;
	bool							m_Initialized;
	
	protected UIManager				m_UIManager;
	
	Widget							m_HudRootWidget;
	ImageWidget 					m_MicrophoneIcon;
	
	ref InventoryMenu				m_InventoryMenu;
	ref Chat 						m_Chat;
	ref ActionMenu					m_ActionMenu;
	ref IngameHud					m_Hud;
	ref HudDebug					m_HudDebug;
	ref LogoutMenu					m_Logout;
	ref DebugMonitor				m_DebugMonitor;
	
	protected ref GameplayEffectWidgets		m_EffectWidgets;
	
	ref Timer						m_ChatChannelHideTimer;
	ref WidgetFadeTimer				m_ChatChannelFadeTimer;
	ref WidgetFadeTimer				m_MicFadeTimer;
	
	Widget							m_ChatChannelArea;
	TextWidget						m_ChatChannelText;
	NoteMenu 						m_Note;
	
	protected ref Timer				m_ToggleHudTimer;
	protected const int 			HOLD_LIMIT_TIME	= 300; //ms
	protected int					m_ActionDownTime;
	protected int					m_ActionUpTime;
	protected bool 					m_InitOnce;
	protected bool 					m_ControlDisabled; //DEPRECATED; disabled mode stored below
	protected int 					m_ControlDisabledMode;
	protected ref array<string> 	m_ActiveInputExcludeGroups; //exclude groups defined in 'specific.xml' file
	protected ref array<int> 		m_ActiveInputRestrictions; //additional scripted restrictions
	protected bool 					m_QuickbarHold;
	protected bool 					m_PlayerRespawning;
	protected int 					m_RespawnModeClient; //for client-side usage
	protected bool 					m_PauseQueued;
	
	// von control info
	protected bool					m_VoNActive;
	protected Widget				m_VoiceLevels;
	protected ref map<int,ImageWidget> m_VoiceLevelsWidgets;
	protected ref map<int,ref WidgetFadeTimer> m_VoiceLevelTimers;

	void MissionGameplay()
	{
		DestroyAllMenus();
		m_Initialized				= false;
		m_EffectWidgets 			= new GameplayEffectWidgets;
		m_HudRootWidget				= null;
		m_Chat						= new Chat;
		m_ActionMenu				= new ActionMenu;
		m_LifeState					= -1;
		m_Hud						= new IngameHud;
		m_VoNActive					= false;
		m_PauseQueued				= false;
		m_ChatChannelFadeTimer		= new WidgetFadeTimer;
		m_MicFadeTimer				= new WidgetFadeTimer;
		m_ChatChannelHideTimer		= new Timer(CALL_CATEGORY_GUI);
		m_ToggleHudTimer			= new Timer(CALL_CATEGORY_GUI);
		
		m_ActiveRefresherLocations 	= new array<vector>;
		SyncEvents.RegisterEvents();
	}
	
	void ~MissionGameplay()
	{
		DestroyInventory();
		//GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.UpdateDebugMonitor);
	#ifndef NO_GUI
		if (g_Game.GetUIManager() && g_Game.GetUIManager().ScreenFadeVisible())
		{
			g_Game.GetUIManager().ScreenFadeOut(0);
		}
	#endif
	}
	
	InventoryMenu GetInventory()
	{
		return m_InventoryMenu;
	}
	
	override void OnInit()
	{
		super.OnInit();

		if ( m_Initialized )
		{
			return;
		}
		
		#ifdef DEVELOPER
		if (!GetGame().IsMultiplayer())//to make it work in single during development
		{
			UndergroundAreaLoader.SpawnAllTriggerCarriers();
		}
		#endif
		
		PPEffects.Init(); //DEPRECATED, left in for legacy purposes only
		MapMarkerTypes.Init();
		
		m_UIManager = GetGame().GetUIManager();
			
		m_Initialized				= true;

		// init hud ui
		if ( !m_HudRootWidget )
		{
			m_HudRootWidget			= GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_hud.layout");
			
			m_HudRootWidget.Show(false);
			
			m_Chat.Init(m_HudRootWidget.FindAnyWidget("ChatFrameWidget"));
			
			m_ActionMenu.Init( m_HudRootWidget.FindAnyWidget("ActionsPanel"), TextWidget.Cast( m_HudRootWidget.FindAnyWidget("DefaultActionWidget") ) );
			
			m_Hud.Init( m_HudRootWidget.FindAnyWidget("HudPanel") );
			
			// von enabled icon
			m_MicrophoneIcon = ImageWidget.Cast( m_HudRootWidget.FindAnyWidget("mic") );
			m_MicrophoneIcon.Show(false);
			
			// von voice level
			m_VoiceLevels = m_HudRootWidget.FindAnyWidget("VoiceLevelsPanel");
			m_VoiceLevelsWidgets = new map<int, ImageWidget>; // [key] voice level
			m_VoiceLevelTimers = new map<int,ref WidgetFadeTimer>; // [key] voice level
		
			if( m_VoiceLevels )
			{
				m_VoiceLevelsWidgets.Set(VoiceLevelWhisper, ImageWidget.Cast( m_VoiceLevels.FindAnyWidget("Whisper") ));
				m_VoiceLevelsWidgets.Set(VoiceLevelTalk, ImageWidget.Cast( m_VoiceLevels.FindAnyWidget("Talk") ));
				m_VoiceLevelsWidgets.Set(VoiceLevelShout, ImageWidget.Cast( m_VoiceLevels.FindAnyWidget("Shout") ));
				
				m_VoiceLevelTimers.Set(VoiceLevelWhisper, new WidgetFadeTimer);
				m_VoiceLevelTimers.Set(VoiceLevelTalk, new WidgetFadeTimer);
				m_VoiceLevelTimers.Set(VoiceLevelShout, new WidgetFadeTimer);
			}
			
			HideVoiceLevelWidgets();
			
			// chat channel
			m_ChatChannelArea		= m_HudRootWidget.FindAnyWidget("ChatChannelPanel");
			m_ChatChannelText		= TextWidget.Cast( m_HudRootWidget.FindAnyWidget("ChatChannelText") );
		}
		
		// init hud ui
		#ifdef DEVELOPER	
			m_HudDebug				= new HudDebug;
			
			if ( !m_HudDebug.IsInitialized() )
			{
				m_HudDebug.Init( GetGame().GetWorkspace().CreateWidgets("gui/layouts/debug/day_z_hud_debug.layout") );
				PluginConfigDebugProfile.GetInstance().SetLogsEnabled(LogManager.IsLogsEnable());
			}
		#endif

		//AIBehaviourHL.RegAIBehaviour("zombie2",AIBehaviourHLZombie2,AIBehaviourHLDataZombie2);
		//RegBehaviour("zombie2",AIBehaviourHLZombie2,AIBehaviourHLDataZombie2);
		
		if ( GetGame().IsMultiplayer() )
		{
			OnlineServices.m_MuteUpdateAsyncInvoker.Insert( SendMuteListToServer );
		}
	}
	
	UIManager GetUIManager()
	{
		return m_UIManager;
	}

	override void OnMissionStart()
	{
		g_Game.SetConnecting(false);
		//does not display HUD until player is fully loaded
		//m_HudRootWidget.Show(true);
		GetUIManager().ShowUICursor(false);
		g_Game.SetMissionState( DayZGame.MISSION_STATE_GAME );
	}
	
	void InitInventory()
	{
		if ( !m_InventoryMenu )
		{
			m_InventoryMenu = InventoryMenu.Cast( GetUIManager().CreateScriptedMenu(MENU_INVENTORY, null) );
		}
	}
	
	void TickScheduler(float timeslice)
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
				
		if( player )
			player.OnScheduledTick(timeslice);
	}
	
	void SendMuteListToServer( map<string, bool> mute_list )
	{
		if( mute_list && mute_list.Count() > 0 )
		{
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(mute_list);
			rpc.Send(null, ERPCs.RPC_USER_SYNC_PERMISSIONS, true, null);
		}
	}
	
	override void OnMissionFinish()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		//Print("OnMissionFinish");
		GetUIManager().HideDialog();
		DestroyAllMenus();
		
		m_Chat.Destroy();
		delete m_HudRootWidget;
		
		#ifdef PLATFORM_CONSOLE
			OnlineServices.m_MuteUpdateAsyncInvoker.Remove( SendMuteListToServer );
		#endif

		if (m_DebugMonitor)
			m_DebugMonitor.Hide();
		g_Game.GetUIManager().ShowUICursor(false);
		PPEManagerStatic.GetPPEManager().StopAllEffects(PPERequesterCategory.ALL);
		EnableAllInputs();
		g_Game.SetMissionState( DayZGame.MISSION_STATE_FINNISH );
	}
	
	override void OnUpdate(float timeslice)
	{
		Man player = GetGame().GetPlayer();
		PlayerBase playerPB = PlayerBase.Cast(player);
		TickScheduler(timeslice);
		UpdateDummyScheduler();//for external entities
		UIScriptedMenu menu = m_UIManager.GetMenu();
		InventoryMenu inventory = InventoryMenu.Cast( m_UIManager.FindMenu(MENU_INVENTORY) );
		NoteMenu note_menu = NoteMenu.Cast( m_UIManager.FindMenu(MENU_NOTE) );
		GesturesMenu gestures_menu = GesturesMenu.Cast(m_UIManager.FindMenu(MENU_GESTURES));
		RadialQuickbarMenu quickbar_menu = RadialQuickbarMenu.Cast(m_UIManager.FindMenu(MENU_RADIAL_QUICKBAR));
		InspectMenuNew inspect = InspectMenuNew.Cast( m_UIManager.FindMenu(MENU_INSPECT) );
		Input input = GetGame().GetInput();
		ActionBase runningAction;
		
		if ( playerPB )
		{
			#ifdef DEVELOPER
			if ( DiagMenu.GetBool(DiagMenuIDs.DM_HOLOGRAM) )
			{
				DbgUI.Begin("Hologram Debug", 5, 5);
			}
			#endif
			
			if ( playerPB.GetHologramLocal() )
			{
				playerPB.GetHologramLocal().UpdateHologram( timeslice );
			}
			#ifdef DEVELOPER
			else if ( DiagMenu.GetBool(DiagMenuIDs.DM_HOLOGRAM) )
			{	
				DbgUI.Text("No active Hologram");
			}
			
			if ( DiagMenu.GetBool(DiagMenuIDs.DM_HOLOGRAM) )
			{		
				DbgUI.End();
			}
			#endif
			
			runningAction = playerPB.GetActionManager().GetRunningAction();
		}
		
#ifdef PLATFORM_CONSOLE
		//'Special behaviour' (read 'hack') for colliding VON distance input actions [CONSOLE ONLY]
		if ( input.LocalValue("UAVoiceModifierHelper",false) && !menu)
		{
			GetUApi().GetInputByName("UAUIQuickbarRadialOpen").Lock();
			GetUApi().GetInputByName("UAZoomInToggle").Lock();
			GetUApi().GetInputByName("UAPersonView").Lock();
			GetUApi().GetInputByName("UALeanLeft").Lock();
			GetUApi().GetInputByName("UALeanRight").Lock();
		}
		else if ( input.LocalRelease("UAVoiceModifierHelper",false) ) //unlocks on release, if already excluded, all the other inputs should be locked/unlocked in the exclude (if defined properly!)
		{
			GetUApi().GetInputByName("UAUIQuickbarRadialOpen").Unlock();
			GetUApi().GetInputByName("UAZoomInToggle").Unlock();
			GetUApi().GetInputByName("UAPersonView").Unlock();
			GetUApi().GetInputByName("UALeanLeft").Unlock();
			GetUApi().GetInputByName("UALeanRight").Unlock();
			
			RefreshExcludes();
		}
		
		//Radial quickbar
		if ( input.LocalPress("UAUIQuickbarRadialOpen",false) )
		{
			//open quickbar menu
			if ( playerPB.IsAlive() && !playerPB.IsRaised() && !playerPB.IsUnconscious() && !playerPB.GetCommand_Vehicle() )	//player hands not raised, player is not in prone and player is not interacting with vehicle
			{
				if ( !GetUIManager().IsMenuOpen( MENU_RADIAL_QUICKBAR ) )
				{
					RadialQuickbarMenu.OpenMenu();
					m_Hud.ShowHudUI( false );
				}	
			}
		}
		
		bool b1 = RadialQuickbarMenu.GetItemToAssign() != null;
		//close quickbar menu from world
		if ( GetUIManager().IsMenuOpen( MENU_RADIAL_QUICKBAR ) && (!RadialQuickbarMenu.GetMenuInstance().GetParentMenu() || RadialQuickbarMenu.GetMenuInstance().GetParentMenu() != inventory) && (input.LocalRelease("UAUIQuickbarRadialOpen",false) || !input.LocalValue("UAUIQuickbarRadialOpen",false)) )
		{
			RadialQuickbarMenu.CloseMenu();
			quickbar_menu.SetMenuClosing(true);
			m_Hud.ShowHudUI( true );
		}
		
		//Radial Quickbar from inventory
		if ( (RadialQuickbarMenu.instance && RadialQuickbarMenu.GetMenuInstance().GetParentMenu() && RadialQuickbarMenu.GetMenuInstance().GetParentMenu() == inventory) && GetGame().GetInput().LocalRelease("UAUIQuickbarRadialInventoryOpen",false) )
		{
			//close radial quickbar menu
			if ( GetGame().GetUIManager().IsMenuOpen( MENU_RADIAL_QUICKBAR ) )
			{
				RadialQuickbarMenu.CloseMenu();
				quickbar_menu.SetMenuClosing(true);
				RadialQuickbarMenu.SetItemToAssign( NULL );
				AddActiveInputExcludes({"inventory"});
				AddActiveInputRestriction(EInputRestrictors.INVENTORY);
			}
		}
		
		//Special behaviour for leaning [CONSOLE ONLY]
		if ( playerPB && !GetGame().IsInventoryOpen() )
		{
			if ( playerPB.IsRaised() || playerPB.IsInRasedProne() )
			{
				GetUApi().GetInputByName( "UALeanLeft" 	).Unlock();
				GetUApi().GetInputByName( "UALeanRight" ).Unlock();
			}
			else
			{
				GetUApi().GetInputByName( "UALeanLeft" 	).Lock();
				GetUApi().GetInputByName( "UALeanRight" ).Lock();	
			}
		}
		else
		{
			if ( playerPB )
			{
				GetUApi().GetInputByName( "UALeanLeft" 	).Lock();
				GetUApi().GetInputByName( "UALeanRight" ).Lock();	
			}
		}
		
		//Special behaviour for freelook & zeroing [CONSOLE ONLY]
		if ( playerPB )
		{
			if ( playerPB.IsRaised() )
			{
				GetUApi().GetInputByName( "UALookAround" 	).Lock();		//disable freelook
				GetUApi().GetInputByName( "UALookAroundToggle" 	).Lock();		//disable freelook
				
				GetUApi().GetInputByName( "UAZeroingUp" 	).Unlock();		//enable zeroing
				GetUApi().GetInputByName( "UAZeroingDown" 	).Unlock();
			}
			else
			{
				GetUApi().GetInputByName( "UALookAround" 	).Unlock();	//enable freelook
				GetUApi().GetInputByName( "UALookAroundToggle" 	).Unlock();	//enable freelook
				
				GetUApi().GetInputByName( "UAZeroingUp" 	).Lock();		//disable zeroing
				GetUApi().GetInputByName( "UAZeroingDown" 	).Lock();
			}
		}
#endif
		//Gestures
		if ( input.LocalPress("UAUIGesturesOpen",false) )
		{
			//open gestures menu
			if ( !playerPB.IsRaised() && (playerPB.GetActionManager().ActionPossibilityCheck(playerPB.m_MovementState.m_CommandTypeId) || playerPB.IsEmotePlaying()) && !playerPB.GetCommand_Vehicle() )
			{
				if ( !GetUIManager().IsMenuOpen( MENU_GESTURES ) )
				{
					GesturesMenu.OpenMenu();
					m_Hud.ShowHudUI( false );
				}
			}
		}
		
		if ( input.LocalRelease("UAUIGesturesOpen",false) || input.LocalValue("UAUIGesturesOpen",false) == 0 )
		{
			//close gestures menu
			if ( GetUIManager().IsMenuOpen( MENU_GESTURES ) )
			{
				GesturesMenu.CloseMenu();
				gestures_menu.SetMenuClosing(true);
				m_Hud.ShowHudUI( true );
			}
		}
		
		if (player && m_LifeState == EPlayerStates.ALIVE && !player.IsUnconscious() )
		{
			// enables HUD on spawn
			if (m_HudRootWidget)
			{
				m_HudRootWidget.Show(true);
			}
			
		#ifndef NO_GUI
			// fade out black screen
			
			if ( GetUIManager().ScreenFadeVisible() )
			{
				 GetUIManager().ScreenFadeOut(0.5);
			}
			
		#endif
		
			if (input.LocalPress("UAGear",false))
			{
				if (!inventory && playerPB.CanManipulateInventory() && IsMapUnfoldActionRunning(runningAction))
				{
					ShowInventory();
					menu = m_InventoryMenu;
				}
				else if (menu == inventory)
				{
					HideInventory();
				}
			}
			
			if (input.LocalPress("UAUIMenu",false) && menu && inventory && menu == inventory)
			{
				HideInventory();
			}
			
			#ifndef PLATFORM_CONSOLE
			if ( input.LocalPress("UAChat",false) )
			{
				ChatInputMenu chat = ChatInputMenu.Cast( m_UIManager.FindMenu(MENU_CHAT) );		
				if ( menu == NULL )
				{
					ShowChat();
				}
			}
			#endif
			
			// voice level updated
			VONManager.GetInstance().HandleInput(input);
			
			if (input.LocalHold("UAUIQuickbarToggle", false))
			{
				if (!m_QuickbarHold)
				{
					m_QuickbarHold = true;
					m_Hud.ShowHudPlayer(m_Hud.IsHideHudPlayer());
				}
			}
			
			if (input.LocalRelease("UAUIQuickbarToggle", false))
			{
				if (!m_QuickbarHold)
				{
					m_Hud.ShowQuickbarPlayer(m_Hud.IsHideQuickbarPlayer());
				}
				m_QuickbarHold = false;
			}
			
			if ( g_Game.GetInput().LocalPress("UAZeroingUp",false) || g_Game.GetInput().LocalPress("UAZeroingDown",false) || g_Game.GetInput().LocalPress("UAToggleWeapons",false) )
			{

				m_Hud.ZeroingKeyPress();
			}
			
			if ( menu == NULL )
			{
				m_ActionMenu.Refresh();
				
				if (input.LocalPress("UANextActionCategory",false))
				{
					m_ActionMenu.NextActionCategory();
				}
				else if (input.LocalPress("UAPrevActionCategory",false))
				{
					m_ActionMenu.PrevActionCategory();
				}				
				else if (input.LocalPress("UANextAction",false))
				{
					m_ActionMenu.NextAction();
				}
				else if (input.LocalPress("UAPrevAction",false))
				{
					m_ActionMenu.PrevAction();
				}
			}
			else
			{
				m_ActionMenu.Hide();
			}
			
			//hologram rotation
			if (menu == NULL && playerPB.IsPlacingLocal() && playerPB.GetHologramLocal().GetParentEntity().PlacementCanBeRotated())
			{
				if ( input.LocalRelease("UANextAction",false) )
				{
					playerPB.GetHologramLocal().SubtractProjectionRotation(15);
				}
				
				if ( input.LocalRelease("UAPrevAction",false) )
				{
					playerPB.GetHologramLocal().AddProjectionRotation(15);
				}
			}
			
			if (CfgGameplayHandler.GetMapIgnoreMapOwnership())
			{
				if (input.LocalPress("UAMapToggle", false) && !m_UIManager.GetMenu())
				{
					if (IsMapUnfoldActionRunning(runningAction))
					{
						HandleMapToggleByKeyboardShortcut(player);
					}
				}
			}
		}
		
		// life state check
		if (player)
		{
			int life_state = player.GetPlayerState();
			
			// life state changed
			if (m_LifeState != life_state)
			{
				m_LifeState = life_state;
				
				if (m_LifeState != EPlayerStates.ALIVE)
				{
					CloseAllMenus();
				}
			}
		}
		
		if (menu && !menu.UseKeyboard() && menu.UseMouse())
		{
			int i;
			for (i = 0; i < 5; i++)
			{
				input.DisableKey(i | INPUT_DEVICE_MOUSE);
				input.DisableKey(i | INPUT_ACTION_TYPE_DOWN_EVENT | INPUT_DEVICE_MOUSE);
				input.DisableKey(i | INPUT_ACTION_TYPE_DOUBLETAP | INPUT_DEVICE_MOUSE);
			}
			
			for (i = 0; i < 6; i++)
			{
				input.DisableKey(i | INPUT_DEVICE_MOUSE_AXIS);
			}
		}
		
		if (!m_UIManager.IsDialogVisible())
		{
			if ( menu )
			{
				if ( menu == inspect )
				{
					if (input.LocalPress("UAGear",false))
					{
						if ( ItemManager.GetInstance().GetSelectedItem() == NULL )
						{
							HideInventory();
						}
					}
					else if (input.LocalPress("UAUIBack",false))
					{
						if ( ItemManager.GetInstance().GetSelectedItem() == NULL )
						{
							HideInventory();
						}
					}
				}
				else if (menu == note_menu && (!IsInputExcludeActive("inventory") || !IsInputRestrictionActive(EInputRestrictors.INVENTORY)))
				{
					AddActiveInputExcludes({"inventory"});
					AddActiveInputRestriction(EInputRestrictors.INVENTORY);
				}
				else if (menu == gestures_menu && !gestures_menu.IsMenuClosing() && !IsInputExcludeActive("radialmenu"))
				{
					AddActiveInputExcludes({"radialmenu"});
					GetUApi().GetInputByName("UAUIGesturesOpen").Unlock();
				}
				else if (menu == quickbar_menu && !quickbar_menu.IsMenuClosing() && !IsInputExcludeActive("radialmenu"))
				{
					AddActiveInputExcludes({"radialmenu"});
					GetUApi().GetInputByName("UAUIQuickbarRadialOpen").Unlock();
				}
				else if ( IsPaused() )
				{
					InGameMenuXbox menu_xb = InGameMenuXbox.Cast( GetGame().GetUIManager().GetMenu() );
					if ( !g_Game.GetUIManager().ScreenFadeVisible() && ( !menu_xb || !menu_xb.IsOnlineOpen() ) )
					{
						if ( input.LocalPress("UAUIMenu",false) )
						{
							Continue();
						}
						else if ( input.LocalPress( "UAUIBack", false ) )
						{
							Continue();
						}
					}
					else if ( input.LocalPress( "UAUIBack", false ) )
					{
						if ( menu_xb && menu_xb.IsOnlineOpen() )
						{
							menu_xb.CloseOnline();
						}
					}
				}
			}
			else if (input.LocalPress("UAUIMenu",false))
			{
				if (IsMapUnfoldActionRunning(runningAction))
				{
					Pause();
				}
			}
			
			//final controls check that suppresses inputs to avoid input collision. If anything needed to be handled without forced input suppression, it had been at this point.
			/*if ( playerPB )
			{
				//Print("playerPB.m_hac: " + playerPB.m_hac);
				if ( IsControlDisabled() && !menu && !m_PauseQueued && !playerPB.GetCommand_Melee2() && !playerPB.m_hac )
				{
					EnableAllInputs(true);
				}
			}*/
		}
		
		UpdateDebugMonitor();
		
		SEffectManager.Event_OnFrameUpdate(timeslice);
		
		if (!GetGame().IsMultiplayer())
			m_WorldData.UpdateBaseEnvTemperature( timeslice );

#ifdef DEVELOPER
		DisplayHairDebug();
#endif
		super.OnUpdate( timeslice );
	}
	
	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);
		m_Hud.KeyPress(key);
	}
	
	override void OnKeyRelease(int key)
	{
		super.OnKeyRelease(key);
	}
	
	override void OnEvent(EventType eventTypeId, Param params)
	{
		super.OnEvent(eventTypeId, params);
		InventoryMenu menu;
		Man player = GetGame().GetPlayer();
		
		switch (eventTypeId)
		{
			case ChatMessageEventTypeID:
				ChatMessageEventParams chat_params = ChatMessageEventParams.Cast( params );			
				if (m_LifeState == EPlayerStates.ALIVE)
				{
					m_Chat.Add(chat_params);
				}
				break;
				
			case ChatChannelEventTypeID:
				ChatChannelEventParams cc_params = ChatChannelEventParams.Cast( params );
				ChatInputMenu chatMenu = ChatInputMenu.Cast( GetUIManager().FindMenu(MENU_CHAT_INPUT) );
				if (chatMenu)
				{
					chatMenu.UpdateChannel();
				}
				else
				{
					m_ChatChannelText.SetText(ChatInputMenu.GetChannelName(cc_params.param1));
					m_ChatChannelFadeTimer.FadeIn(m_ChatChannelArea, 0.5, true);
					m_ChatChannelHideTimer.Run(2, m_ChatChannelFadeTimer, "FadeOut", new Param3<Widget, float, bool>(m_ChatChannelArea, 0.5, true));
				}
				break;
				
			case WindowsResizeEventTypeID:
				DestroyAllMenus();
				m_Hud.OnResizeScreen();
				
				break;
	
			case SetFreeCameraEventTypeID:
				SetFreeCameraEventParams set_free_camera_event_params = SetFreeCameraEventParams.Cast( params );
				PluginDeveloper plugin_developer = PluginDeveloper.Cast( GetPlugin(PluginDeveloper) );
				plugin_developer.OnSetFreeCameraEvent( PlayerBase.Cast( player ), set_free_camera_event_params.param1 );
				break;
		}
	}
	
	override void OnItemUsed(InventoryItem item, Man owner)
	{
		if (item && GetUIManager().GetMenu() == NULL)
		{
			if (item.IsInherited(ItemBook))
			{
				BookMenu bookMenu = BookMenu.Cast( GetUIManager().EnterScriptedMenu(MENU_BOOK, NULL) );
				if (bookMenu)
				{
					bookMenu.ReadBook(item);
				}
			}
		}
	}
	
#ifdef DEVELOPER
	override void SetInputSuppression(bool state)
	{
		m_SuppressNextFrame = state;
	}
	
	override bool GetInputSuppression()
	{
		return m_SuppressNextFrame;
	}
#endif
	
	//! Deprecated; removes last input exclude and associated controls restrictions
	override void PlayerControlEnable( bool bForceSupress )
	{
		super.PlayerControlEnable(bForceSupress);
		
		if (m_ControlDisabledMode != -1)
		{
			switch (m_ControlDisabledMode)
			{
				case INPUT_EXCLUDE_ALL:
				{
					RemoveActiveInputExcludes({"menu"},bForceSupress);
					break;
				}
				case INPUT_EXCLUDE_INVENTORY:
				{
					RemoveActiveInputExcludes({"inventory"},bForceSupress);
					RemoveActiveInputRestriction(EInputRestrictors.INVENTORY);
					break;
				}
				case INPUT_EXCLUDE_MAP:
				{
					RemoveActiveInputExcludes({"loopedactions"},bForceSupress);
					RemoveActiveInputRestriction(EInputRestrictors.MAP);
					break;
				}
				case INPUT_EXCLUDE_MOUSE_ALL:
				{
					RemoveActiveInputExcludes({"radialmenu"},bForceSupress);
					break;
				}
				case INPUT_EXCLUDE_MOUSE_RADIAL:
				{
					RemoveActiveInputExcludes({"radialmenu"},bForceSupress);
					break;
				}
			}
			
			m_ControlDisabledMode = -1;
			
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			if (player)
			{
				HumanInputController hic = player.GetInputController();
				hic.LimitsDisableSprint(false);
			}
		}
	}

	//! Deprecated; simple input restrictions
	override void PlayerControlDisable(int mode)
	{
		super.PlayerControlDisable(mode);
		
		switch (mode)
		{
			case INPUT_EXCLUDE_ALL:
			{
				AddActiveInputExcludes({"menu"});
				break;
			}
			case INPUT_EXCLUDE_INVENTORY:
			{
				AddActiveInputExcludes({"inventory"});
				AddActiveInputRestriction(EInputRestrictors.INVENTORY);
				break;
			}
			case INPUT_EXCLUDE_MAP:
			{
				AddActiveInputExcludes({"loopedactions"});
				AddActiveInputRestriction(EInputRestrictors.MAP);
				break;
			}
			case INPUT_EXCLUDE_MOUSE_ALL:
			{
				AddActiveInputExcludes({"radialmenu"});
				break;
			}
			case INPUT_EXCLUDE_MOUSE_RADIAL:
			{
				AddActiveInputExcludes({"radialmenu"});
				break;
			}
			default:
			{
				Debug.Log("Unknown controls disable mode");
				return;
			}
		}
		
		m_ControlDisabledMode = mode;
		
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( player )
		{
			ItemBase item = player.GetItemInHands();
			if (item && item.IsWeapon())
				player.RequestResetADSSync();
		}
	}
	
	//! Removes one or more exclude groups and refreshes excludes
	override void RemoveActiveInputExcludes(array<string> excludes, bool bForceSupress = false)
	{
		super.RemoveActiveInputExcludes(excludes,bForceSupress);
		
		if (excludes.Count() != 0)
		{
			bool changed = false;
			if (!m_ActiveInputExcludeGroups)
			{
				m_ActiveInputExcludeGroups = new array<string>;
			}
			else
			{
				for (int i = 0; i < excludes.Count(); i++)
				{
					if (m_ActiveInputExcludeGroups.Find(excludes[i]) != -1)
					{
						m_ActiveInputExcludeGroups.RemoveItem(excludes[i]);
						changed = true;
					}
				}
				//m_ActiveInputExcludeGroups.Sort(); //?
			}
			
			if (changed)
			{
				RefreshExcludes();
			}
			// supress control for next frame
			GetUApi().SupressNextFrame(bForceSupress);
		}
	}
	
	//! Removes one restriction (specific behaviour oudside regular excludes, defined below)
	override void RemoveActiveInputRestriction(int restrictor)
	{
		//unique behaviour outside regular excludes
		if (restrictor > -1)
		{
			switch (restrictor)
			{
				case EInputRestrictors.INVENTORY:
				{
					GetUApi().GetInputByName("UAWalkRunForced").ForceEnable(false); // force walk off!
					break;
				}
				case EInputRestrictors.MAP:
				{
					GetUApi().GetInputByName("UAWalkRunForced").ForceEnable(false); // force walk off!
					break;
				}
			}
			
			if (m_ActiveInputRestrictions && m_ActiveInputRestrictions.Find(restrictor) != -1)
			{
				m_ActiveInputRestrictions.RemoveItem(restrictor);
			}
		}
	}
	
	//! Adds one or more exclude groups to disable and refreshes excludes
	override void AddActiveInputExcludes(array<string> excludes)
	{
		super.AddActiveInputExcludes(excludes);
		
		if (excludes.Count() != 0)
		{
			bool changed = false;
			if (!m_ActiveInputExcludeGroups)
			{
				m_ActiveInputExcludeGroups = new array<string>;
			}
			
			for (int i = 0; i < excludes.Count(); i++)
			{
				if (m_ActiveInputExcludeGroups.Find(excludes[i]) == -1)
				{
					m_ActiveInputExcludeGroups.Insert(excludes[i]);
					changed = true;
				}
			}
			//m_ActiveInputExcludeGroups.Sort(); //?
			
			if (changed)
			{
				RefreshExcludes();
				#ifdef BULDOZER
					GetUApi().SupressNextFrame(true);
				#endif
			}
		}
	}
	
	//! Adds one input restriction (specific behaviour oudside regular excludes, defined below)
	override void AddActiveInputRestriction(int restrictor)
	{
		//unique behaviour outside regular excludes
		if (restrictor > -1)
		{
			switch (restrictor)
			{
				case EInputRestrictors.INVENTORY:
				{
					GetUApi().GetInputByName("UAWalkRunForced").ForceEnable(true); // force walk on!
					PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
					if ( player )
					{
						ItemBase item = player.GetItemInHands();
						if (item && item.IsWeapon())
							player.RequestResetADSSync();
					}
					break;
				}
				case EInputRestrictors.MAP:
				{
					GetUApi().GetInputByName("UAWalkRunForced").ForceEnable(true); // force walk on!
					break;
				}
			}
			
			if (!m_ActiveInputRestrictions)
			{
				m_ActiveInputRestrictions = new array<int>;
			}
			if (m_ActiveInputRestrictions.Find(restrictor) == -1)
			{
				m_ActiveInputRestrictions.Insert(restrictor);
			}
		}
	}
	
	//! refreshes any active excludes
	override void RefreshExcludes()
	{
		if (m_ActiveInputExcludeGroups)
		{
			for (int i = 0; i < m_ActiveInputExcludeGroups.Count(); i++)
			{
				GetUApi().ActivateExclude(m_ActiveInputExcludeGroups[i]);
			}
		}
		GetUApi().UpdateControls();
	}
	
	//! Removes all active input excludes and restrictions
	override void EnableAllInputs(bool bForceSupress = false)
	{
		m_ControlDisabledMode = -1;
		
		if (m_ActiveInputRestrictions)
		{
			int count = m_ActiveInputRestrictions.Count();
			for (int i = 0; i < count; i++)
			{
				RemoveActiveInputRestriction(m_ActiveInputRestrictions[0]);
			}
			m_ActiveInputRestrictions.Clear(); //redundant?
		}
		if (m_ActiveInputExcludeGroups)
		{
			m_ActiveInputExcludeGroups.Clear();
		}
		
		GetUApi().UpdateControls();		
		// supress control for next frame
		GetUApi().SupressNextFrame(bForceSupress);
	}
	
	//! returns if ANY exclude groups, restriction (or deprecated disable, if applicable) is active
	override bool IsControlDisabled()
	{
		bool active = false;
		if (m_ActiveInputExcludeGroups)
		{
			active |= m_ActiveInputExcludeGroups.Count() > 0;
		}
		if (m_ActiveInputRestrictions)
		{
			active |= m_ActiveInputRestrictions.Count() > 0;
		}
		active |= m_ControlDisabledMode >= INPUT_EXCLUDE_ALL; //legacy stuff, Justin case
		return active;
	}
	
	//! Returns true if the particular input exclude group had been activated via script and is active
	override bool IsInputExcludeActive(string exclude)
	{
		return m_ActiveInputExcludeGroups && m_ActiveInputExcludeGroups.Find(exclude) != -1;
	}
	
	//! Returns true if the particular 'restriction' (those govern special behaviour outside regular input excludes, *EInputRestrictors*) is active
	override bool IsInputRestrictionActive(int restriction)
	{
		return m_ActiveInputRestrictions && m_ActiveInputRestrictions.Find(restriction) != -1;
	}
	
	//! (mostly)DEPRECATED; only set on the old 'PlayerControlDisable' method
	override int GetControlDisabledMode()
	{
		return m_ControlDisabledMode;
	}
	
	void CloseAllMenus()
	{
		GetUIManager().CloseAll();
	}
	
	
	void DestroyAllMenus()
	{
		if( GetUIManager() )
		{
			GetUIManager().HideDialog();
			GetUIManager().CloseAll();
		}
		
		DestroyInventory();
		
		if( m_Chat )
			m_Chat.Clear();
	}
	
	void MoveHudForInventory( bool inv_open )
	{
		#ifdef PLATFORM_CONSOLE
		IngameHud hud = IngameHud.Cast( GetHud() );
		if( hud )
		{
			if( inv_open )
			{
				hud.GetHudPanelWidget().SetPos( 0, -0.055 );
			}
			else
			{
				hud.GetHudPanelWidget().SetPos( 0, 0 );
			}
		}
		#endif
	}
	
	override void ShowInventory()
	{
		UIScriptedMenu menu = GetUIManager().GetMenu();
		
		if ( !menu && GetGame().GetPlayer().GetHumanInventory().CanOpenInventory() && !GetGame().GetPlayer().IsInventorySoftLocked() && GetGame().GetPlayer().GetHumanInventory().IsInventoryUnlocked() )
		{
			if( !m_InventoryMenu )
			{
				InitInventory();
			}
			
			if( !GetUIManager().FindMenu( MENU_INVENTORY ) )
			{
				GetUIManager().ShowScriptedMenu(m_InventoryMenu, null);
				PlayerBase.Cast(GetGame().GetPlayer()).OnInventoryMenuOpen();
			}
			MoveHudForInventory( true );
			AddActiveInputExcludes({"inventory"});
			AddActiveInputRestriction(EInputRestrictors.INVENTORY);
		}
	}
	
	override void HideInventory()
	{
		if ( m_InventoryMenu )
		{
			GetUIManager().HideScriptedMenu(m_InventoryMenu);
			MoveHudForInventory( false );
			RemoveActiveInputExcludes({"inventory"},false);
			RemoveActiveInputRestriction(EInputRestrictors.INVENTORY);
			PlayerBase.Cast(GetGame().GetPlayer()).OnInventoryMenuClose();
			VicinityItemManager.GetInstance().ResetRefreshCounter();
		}
	}
	
	void DestroyInventory()
	{
		if ( m_InventoryMenu )
		{
			if (!m_InventoryMenu.GetParentMenu() && GetUIManager().GetMenu() != m_InventoryMenu)
			{
				m_InventoryMenu.SetParentMenu(GetUIManager().GetMenu()); //hack; guarantees the 'm_pCurrentMenu' will be set to whatever is on top currently
			}
			m_InventoryMenu.Close();
			m_InventoryMenu = NULL;
		}
	}
	
	override void ResetGUI()
	{
		DestroyInventory();
		InitInventory();
	}
	
	override void ShowChat()
	{
		m_ChatChannelHideTimer.Stop();
		m_ChatChannelFadeTimer.Stop();
		m_ChatChannelArea.Show(false);
		m_UIManager.EnterScriptedMenu(MENU_CHAT_INPUT, NULL);
		
		int level = GetGame().GetVoiceLevel();
		UpdateVoiceLevelWidgets(level);
		
		AddActiveInputExcludes({"menu"});
	}

	override void HideChat()
	{
		RemoveActiveInputExcludes({"menu"},true);
	}
	
	void ShowVehicleInfo()
	{
		if( GetHud() )
			GetHud().ShowVehicleInfo();
	}
	
	void HideVehicleInfo()
	{
		if( GetHud() )
			GetHud().HideVehicleInfo();
	}
	
	override Hud GetHud()
	{
		return m_Hud;
	}
	
	HudDebug GetHudDebug()
	{
		return m_HudDebug;
	}
	
	override void RefreshCrosshairVisibility()
	{
		if (GetHudDebug())
		GetHudDebug().RefreshCrosshairVisibility();
	}
	
	override void HideCrosshairVisibility()
	{
		if (GetHudDebug())
		GetHudDebug().HideCrosshairVisibility();
	}
	
	override bool IsPaused()
	{
		return GetGame().GetUIManager().IsMenuOpen(MENU_INGAME);
	}
	
	override void Pause()
	{
		if (IsPaused() || (GetGame().GetUIManager().GetMenu() && GetGame().GetUIManager().GetMenu().GetID() == MENU_INGAME))
		{
			return;
		}

		if ( g_Game.IsClient() && g_Game.GetGameState() != DayZGameState.IN_GAME )
		{
			return;
		}
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if ( player && !player.IsPlayerLoaded() || IsPlayerRespawning() )
		{
			return;
		}
		
		m_PauseQueued = true;
		
		CloseAllMenus();
		
		// open ingame menu
		GetUIManager().EnterScriptedMenu( MENU_INGAME, GetGame().GetUIManager().GetMenu() );
		AddActiveInputExcludes({"menu"});
	}
	
	override void Continue()
	{
		int menu_id = GetGame().GetUIManager().GetMenu().GetID();
		if ( !IsPaused() || ( menu_id != MENU_INGAME && menu_id != MENU_LOGOUT && menu_id != MENU_RESPAWN_DIALOGUE ) || ( m_Logout && m_Logout.layoutRoot.IsVisible() ) )
		{
			return;
		}
		
		m_PauseQueued = false;
		RemoveActiveInputExcludes({"menu"},true);
		GetUIManager().CloseMenu(MENU_INGAME);
		//GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(CloseInGameMenu,1,true);
	}
	
	/*void CloseInGameMenu()
	{
		if (GetUIManager().IsMenuOpen(MENU_INGAME))
		{
			GetUIManager().CloseMenu(MENU_INGAME);
		}
		else
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(CloseInGameMenu);
		}
	}*/
	
	override bool IsMissionGameplay()
	{
		return true;
	}
	
	override void AbortMission()
	{
		#ifdef BULDOZER
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(g_Game.RequestExit, IDC_MAIN_QUIT);
		#else
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().AbortMission);
		#endif
	}
	
	override void CreateLogoutMenu(UIMenuPanel parent)
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		// do not show logout screen if player's dead
		if (!player || player.IsDamageDestroyed())
		{
			// exit the game immediately
			AbortMission();
			return;
		}
		
		if (parent)
		{
			m_Logout = LogoutMenu.Cast(parent.EnterScriptedMenu(MENU_LOGOUT));
			
			if (m_Logout)
			{
				m_Logout.SetLogoutTime();
			}
		}		
	}
	
	override void StartLogoutMenu(int time)
	{
		if (m_Logout)
		{
			if (time > 0)
			{
				// character will be deleted from server int "time" seconds
				m_Logout.SetTime(time);
				m_Logout.Show();
				
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(m_Logout.UpdateTime, 1000, true);
			}
			else
			{
				// no waiting time -> player is most likely dead
				m_Logout.Exit();
			}
		}
	}
	
	override void CreateDebugMonitor()
	{
		if (!m_DebugMonitor)
		{
			m_DebugMonitor = new DebugMonitor();
			m_DebugMonitor.Init();
		}
	}
	
	protected void HandleMapToggleByKeyboardShortcut(Man player)
	{
		UIManager um = GetGame().GetUIManager();
		if (um && !um.IsMenuOpen(MENU_MAP))
		{
			um.CloseAll();
			if (!CfgGameplayHandler.GetUse3DMap())
			{
				um.EnterScriptedMenu(MENU_MAP, null);
				GetGame().GetMission().AddActiveInputExcludes({"map"});
			}
			else
			{
				GetGame().GetMission().AddActiveInputExcludes({"loopedactions"});
			}

			GetGame().GetMission().AddActiveInputRestriction(EInputRestrictors.MAP);
		}
	}
	
	protected bool IsMapUnfoldActionRunning(ActionBase pAction)
	{
		return !pAction || pAction.Type() != ActionUnfoldMap;
	}
	
	/*void ChangeBleedingIndicatorVisibility(bool visible)
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if (player)
		{
			BleedingSourcesManagerRemote manager = player.GetBleedingManagerRemote();
			if (manager && manager.GetBleedingSourcesCount() > 0)
			{
				manager.ChangeBleedingIndicatorVisibility(visible);
			}
		}
	}*/
	
	void UpdateDebugMonitor()
	{
		if (!m_DebugMonitor) return;
		
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if (player)
		{
			DebugMonitorValues values = player.GetDebugMonitorValues();
			if (values)
			{
				m_DebugMonitor.SetHealth(values.GetHealth());
				m_DebugMonitor.SetBlood(values.GetBlood());
				m_DebugMonitor.SetLastDamage(values.GetLastDamage());
				m_DebugMonitor.SetPosition(MiscGameplayFunctions.TruncateVec(player.GetPosition(),1));
			}
		}
	}
	
	void SetActionDownTime( int time_down )
	{
		m_ActionDownTime = time_down;
	}
	
	void SetActionUpTime( int time_up )
	{
		m_ActionUpTime = time_up;
	}
	
	int LocalPressTime()
	{
		return m_ActionDownTime;
	}
	
	int LocalReleaseTime()
	{
		return m_ActionUpTime;
	}
	
	float GetHoldActionTime()
	{
		float hold_action_time = LocalReleaseTime() - LocalPressTime();
		return hold_action_time;
	}
	
	void DisplayHairDebug()
	{
		if ( DiagMenu.GetBool(DiagMenuIDs.DM_HAIR_DISPLAY_DEBUG) )
		{
			if(!GetGame().IsDedicatedServer())
				ShowHairDebugValues(true);
		}
		else
		{
			if(!GetGame().IsDedicatedServer())
				ShowHairDebugValues(false);
		}
	}
	
	void ShowHairDebugValues(bool state)
	{
#ifdef DEVELOPER
		if( state )
		{
			PluginDiagMenu diagmenu = PluginDiagMenu.Cast(GetPlugin(PluginDiagMenu));
			
			int i 					= DiagMenu.GetValue(DiagMenuIDs.DM_HAIR_LEVEL);
			bool bState 			= diagmenu.m_HairHidingStateMap.Get(i);
			string selectionState;
			if (!bState)
				selectionState 		= "Hidden";
			else
				selectionState 		= "Shown";
			string selectionName 	= diagmenu.m_HairSelectionArray.Get(i);
			
			DbgUI.BeginCleanupScope();
	        DbgUI.Begin("Hair Debug", 50, 150);
	        DbgUI.Text("Current Hair Selection:" + selectionName);
			DbgUI.Text("State: " + selectionState);
			
	        DbgUI.End();
	        DbgUI.EndCleanupScope();
		}
		else
		{
			DbgUI.BeginCleanupScope();
			DbgUI.Begin("Hair Debug", 50, 50);
			DbgUI.End();
	        DbgUI.EndCleanupScope();
		}
#endif
	}
	
	override void UpdateVoiceLevelWidgets(int level)
	{
		for( int n = 0; n < m_VoiceLevelsWidgets.Count(); n++ )
		{
			int voiceKey = m_VoiceLevelsWidgets.GetKey(n);
			ImageWidget voiceWidget = m_VoiceLevelsWidgets.Get(n);
			
			// stop fade timer since it will be refreshed
			WidgetFadeTimer timer = m_VoiceLevelTimers.Get(n);		
			timer.Stop();
		
			// show widgets according to the level
			if( voiceKey <= level )
			{
				voiceWidget.SetAlpha(1.0); // reset from possible previous fade out 
				voiceWidget.Show(true);
				
				if( !m_VoNActive && !GetUIManager().FindMenu(MENU_CHAT_INPUT) ) 	
					timer.FadeOut(voiceWidget, 3.0);	
			}
			else
				voiceWidget.Show(false);
		}
		
		// fade out microphone icon when switching levels without von on
		if( !m_VoNActive )
		{
		  	if( !GetUIManager().FindMenu(MENU_CHAT_INPUT) )
			{
				m_MicrophoneIcon.SetAlpha(1.0); 
				m_MicrophoneIcon.Show(true);
				
				m_MicFadeTimer.FadeOut(m_MicrophoneIcon, 3.0);
			}
		}
		else
		{
			// stop mic icon fade timer when von is activated
			m_MicFadeTimer.Stop();
		}
	}
	
	override ImageWidget GetMicrophoneIcon() 
	{
		return m_MicrophoneIcon;
	}
	
	override WidgetFadeTimer GetMicWidgetFadeTimer()
	{
		return m_MicFadeTimer;
	}
	
	override map<int,ImageWidget> GetVoiceLevelWidgets()
	{
		return m_VoiceLevelsWidgets;
	}
	
	override map<int,ref WidgetFadeTimer> GetVoiceLevelTimers()
	{
		return m_VoiceLevelTimers;
	}
	
	override bool IsVoNActive()
	{
		return m_VoNActive;
	}
	
	override void SetVoNActive(bool active)
	{
		m_VoNActive = active;
	}
	
	override void HideVoiceLevelWidgets()
	{
		for ( int n = 0; n < m_VoiceLevelsWidgets.Count(); n++ )
		{
			ImageWidget voiceWidget = m_VoiceLevelsWidgets.Get( n );
			voiceWidget.Show(false);
		}
	}
	
	override UIScriptedMenu GetNoteMenu()
	{
		return m_Note;
	};
	
	override void SetNoteMenu(UIScriptedMenu menu)
	{
		m_Note = NoteMenu.Cast(menu);
	};
	
	override void OnPlayerRespawned(Man player)
	{
		#ifdef DEVELOPER
			if (m_HudDebug)
				m_HudDebug.RefreshByLocalProfile();
		#endif	
	}
	
	override void SetPlayerRespawning(bool state)
	{
		m_PlayerRespawning = state;
	}
	
	override bool IsPlayerRespawning()
	{
		return m_PlayerRespawning;
	}
	
	override array<vector> GetActiveRefresherLocations()
	{
		return m_ActiveRefresherLocations;
	}
	
	override void SetRespawnModeClient(int mode)
	{
		m_RespawnModeClient = mode;
	}
	
	override int GetRespawnModeClient()
	{
		return m_RespawnModeClient;
	}
	
	override GameplayEffectWidgets GetEffectWidgets()
	{
		return m_EffectWidgets;
	}
}

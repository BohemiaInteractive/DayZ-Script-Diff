// *************************************************************************************
// ! PluginDayzPlayerDebugUserData
// *************************************************************************************
class PluginDayzPlayerDebugUserData
{
	void PluginDayzPlayerDebugUserData(string name, int category, int pUserData, bool pFullBody, int pStanceMask = -1)
	{
		m_iUserData 	= pUserData;//animation CMD id
		m_bIsFullBody	= pFullBody;
		m_iStanceMask	= pStanceMask;
		m_Category		= category;//pType
		m_Name			= name;
	}

	int	GetInt()
	{
		return m_iUserData;
	}

	bool IsFullBody()
	{
		return m_bIsFullBody;
	}
	
	int GetStanceMask()
	{
		return m_iStanceMask;
	}
	
	int GetCategory()
	{
		return m_Category;
	}

	string 	GetName()
	{
		return m_Name;
	}
	string 	GetNameUnique()
	{
		return m_Name + "|" + GetCategory();
	}
	
	protected int 	m_iUserData;
	protected bool	m_bIsFullBody;
	protected int 	m_iStanceMask;
	protected int 		m_Category;
	protected string 	m_Name;
}

// *************************************************************************************
// ! PluginDayzPlayerDebugUI 
// *************************************************************************************
class PluginDayzPlayerDebugUIHandler extends ScriptedWidgetEventHandler
{
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		return m_pPluginPlayerDebug.OnClick(w, x, y, button);
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		return m_pPluginPlayerDebug.OnChange(w, x, y, finished);
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		super.OnMouseEnter(w,x,y);
		GetGame().GetMission().AddActiveInputExcludes({"menu"});
		return true;

	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		super.OnMouseLeave(w,enterW, x,y);
		GetGame().GetMission().RemoveActiveInputExcludes({"menu"},true);
		return true;
	}

	PluginDayzPlayerDebug		m_pPluginPlayerDebug;
}


// *************************************************************************************
// ! PluginDayzPlayerDebugUI 
// *************************************************************************************
class PluginDayzPlayerActionCallback extends HumanCommandActionCallback
{
	protected static ref set<string> m_AnimEventMapping;
	void 	PluginDayzPlayerActionCallback()
	{
		
		//Print("Action callback created");
		EnableCancelCondition(true);
		//RegisterAnimationEvent("ActionExec", 1);	// really weird id 		
		
		if (!m_AnimEventMapping)//making sure to only do once
		{
			m_AnimEventMapping = new set<string>();
			m_AnimEventMapping.Insert("ActionExec");
			m_AnimEventMapping.Insert("SoundWeapon");
			m_AnimEventMapping.Insert("SoundVoice");
			m_AnimEventMapping.Insert("SoundAttachment");
			m_AnimEventMapping.Insert("Sound");
		}
		
		
		foreach (int index, string eventName: m_AnimEventMapping)
			RegisterAnimationEvent(eventName, index);

		EnableStateChangeCallback();               	// enable this to get OnStateChange callbacks 

		m_fTimeStart = GetWorldTime();
	}
	
	void 	~PluginDayzPlayerActionCallback()
	{
		//Print("Action callback deleted");
	}

	override void 	OnFinish(bool pCanceled)
	{
		if (pCanceled)
		{
			m_pStateWidget.SetText("Result: canceled");
		}
		else
		{
			m_pStateWidget.SetText("Result: ended");
		}
	}


	bool				CancelCondition()	
	{ 
		// Print("Action cancel condition");
		return DefaultCancelCondition(); 
	}

	override void 				OnAnimationEvent(int pEventID)
	{
		
		//Print(pEventID);
		//Print("--------------------------------");
		// only one event id is registered - 1 so nothing else can be delivered
		string eventName= m_AnimEventMapping.Get(pEventID);
		string eventText = eventName + " event at: ";
		float  tFromSt = GetWorldTime() - m_fTimeStart;
		
		eventText += tFromSt.ToString();
		m_EventsHistory.AddItem(eventText,null,0);
		m_pAnimEventWidget.SetText(eventText);
	}

	override void 				OnStateChange(int pOldState, int pCurrentState)
	{
		string oldState = GetStateString(pOldState);
		string newState = GetStateString(pCurrentState);
		
		m_pStateWidget.SetText(oldState + "->" + newState);
	};



	//! 
	float 								m_fTimeStart;
	TextWidget							m_pStateWidget;
	TextWidget 							m_pAnimEventWidget;
	TextListboxWidget					m_EventsHistory;
}


// *************************************************************************************
// ! PluginDayzPlayerDebug  
// *************************************************************************************
class PluginDayzPlayerDebug extends PluginBase
{
	private const int TYPE_MOD_LOOPING	= 0;
	private const int TYPE_MOD_ONETIME	= 1;
	private const int TYPE_FB_LOOPING	= 2;
	private const int TYPE_FB_ONETIME	= 3;

	private const int TYPE_MOD_GESTURE_LOOPING	= 4;
	private const int TYPE_MOD_GESTURE_ONETIME	= 5;
	private const int TYPE_FB_GESTURE_LOOPING	= 6;
	private const int TYPE_FB_GESTURE_ONETIME	= 7;
	private const int TYPE_FB_SUICIDE_LOOPING	= TYPE_FB_GESTURE_LOOPING;
	
	private const int TYPE_ALL	= -1;

	

	ref Timer							m_TickTimer;
	
	protected ref array<ref PluginDayzPlayerDebugUserData> 			m_AnimationList 			= new array<ref PluginDayzPlayerDebugUserData>();
	protected ref map<string, ref PluginDayzPlayerDebugUserData> 	m_AnimationListMappings 	= new map<string, ref PluginDayzPlayerDebugUserData>();
	protected ref map<int, string> 									m_TypeNameMappings 			= new map<int, string>();
	
	bool 								m_IsActive			= false;
	bool								m_HasFocus			= false;
	bool								m_HasServerWalk = false;
	
	Widget 								m_MainWnd;
	Widget								m_Card0;
	Widget								m_Card1;
	Widget								m_Card2;
	Widget								m_Card3;
	Widget								m_Card4;
	ButtonWidget						m_Card0Button;		
	ButtonWidget						m_Card1Button;		
	ButtonWidget						m_Card2Button;		
	ButtonWidget						m_Card3Button;		
	ButtonWidget						m_Card4Button;		


	EditBoxWidget 						m_PlayerStatusV;
	TextListboxWidget					m_ActionsSelector;
	ButtonWidget						m_ActionsStart;
	ButtonWidget						m_ActionsEnd;
	ButtonWidget						m_ActionsFinish;
	ButtonWidget						m_ActionsCancel;
	ButtonWidget						m_ActionsOTA;
	ButtonWidget						m_ActionsOTL;
	TextWidget							m_ActionStateV;
	TextWidget 							m_ActionEventV;
	TextWidget 							m_ActionsGlobalStatus;
	
	EditBoxWidget 						m_ActionsFilter;

	ButtonWidget 						m_PlayerSyncCreateShadow;
	ButtonWidget 						m_PlayerSyncDelete;
	ButtonWidget 						m_PlayerServerWalk;

	TextWidget 							m_ItemAccClass;
	TextWidget 							m_ItemAccSClass;
	TextWidget 							m_ItemAccInst;

	ButtonWidget						m_ActionTypeAll;
	ButtonWidget						m_ActionTypeLA;
	ButtonWidget						m_ActionTypeOA;
	ButtonWidget						m_ActionTypeFLA;
	ButtonWidget						m_ActionTypeFOA;
	ButtonWidget						m_ActionTypeLG;
	ButtonWidget						m_ActionTypeOG;
	ButtonWidget						m_ActionTypeFLG;
	ButtonWidget						m_ActionTypeFOG;
	
	
	ButtonWidget						m_ClearEventsButton;
	
	TextListboxWidget					m_EventsHistory;

	ref PluginDayzPlayerDebugUIHandler m_pUIHandler;

	Entity 								m_pPlayerShadow;
	
	protected int 						m_CurrentType;

	// command handler properties
	bool								m_CH_ActionStart 	= false;
	int 								m_CH_ActionCommand 	= -1000;
	bool								m_CH_ActionCancel	= false;
		
	
	ref PluginDayzPlayerDebug_Weapons		m_Weapons;
	ref PluginDayzPlayerDebug_Ctrl			m_Controls;
	ref PluginDayzPlayerDebug_OtherCmds		m_OtherCmds;

	//---------------------------------------------------
    // gui stuff

	void PluginDayzPlayerDebug()
	{
		RegisterAnimations();
		CreateTypeNameMapping();
		
		
		#ifndef NO_GUI
		CreateModuleWidgets();
		m_Weapons 	= new PluginDayzPlayerDebug_Weapons(m_Card1);
		m_Controls	= new PluginDayzPlayerDebug_Ctrl(m_Card2);
		m_OtherCmds	= new PluginDayzPlayerDebug_OtherCmds(m_Card3);
		#endif
		
		
		//m_TypeNameMappings.Insert(TYPE_FB_SUICIDE_LOOPING, "TYPE_FB_SUICIDE_LOOPING");
		
	}
	
	void CreateTypeNameMapping()
	{
		m_TypeNameMappings.Insert(TYPE_MOD_LOOPING, "MOD_LOOPING");
		m_TypeNameMappings.Insert(TYPE_MOD_ONETIME, "MOD_ONETIME");
		m_TypeNameMappings.Insert(TYPE_FB_LOOPING, "FB_LOOPING");
		m_TypeNameMappings.Insert(TYPE_FB_ONETIME, "FB_ONETIME");
		m_TypeNameMappings.Insert(TYPE_MOD_GESTURE_LOOPING, "MOD_GESTURE_LOOPING");
		m_TypeNameMappings.Insert(TYPE_MOD_GESTURE_ONETIME, "MOD_GESTURE_ONETIME");
		m_TypeNameMappings.Insert(TYPE_FB_GESTURE_LOOPING, "FB_GESTURE_LOOPING");
		m_TypeNameMappings.Insert(TYPE_FB_GESTURE_ONETIME, "FB_GESTURE_ONETIME");
	}
	
	
	void ~PluginDayzPlayerDebug()
	{
		ReleaseFocus();	

		PlayerSyncDelete();

		#ifndef NO_GUI
		m_Weapons = NULL;
		m_Controls = NULL;
		m_OtherCmds = NULL;
		DestroyModuleWidgets();
		#endif		
	}
	
	

	void RegisterDebugItem(ItemBase item, PlayerBase player)
	{
	}
	
	
	bool IsActive()
	{
		return m_IsActive;
	}

	void ShowWidgets(bool show)
	{
		if(m_MainWnd) 
		{
			m_MainWnd.Show(show);
		}
	}

	//!
	void Show()
	{
		ShowWidgets(true);
		m_IsActive = true;

		if (!m_TickTimer)
		{
			m_TickTimer = new Timer();
			m_TickTimer.Run(0.1, this, "Tick", NULL, true);			
		}
	}

	//!
	void Hide()
	{
		ShowWidgets(false);
		m_IsActive = false;
			
		m_TickTimer = NULL;
	}

	//!
	void CaptureFocus()
	{
		if (!m_HasFocus)
		{
			GetGame().GetInput().ChangeGameFocus(1);
			GetGame().GetUIManager().ShowUICursor(true);
			m_HasFocus = true;
		}
	}

	//!
	void ReleaseFocus()
	{
		if (m_HasFocus)
		{
			GetGame().GetInput().ChangeGameFocus(-1);
			GetGame().GetUIManager().ShowUICursor(false);
			m_HasFocus = false;
		}	
	}

	int 	m_CurrentMode = 0;

	//!
	void ToggleDebugWindowEvent()
	{
		m_CurrentMode++;
		if (m_CurrentMode > 2)
		{
			m_CurrentMode = 0;
		}

		ToggleDebugWindowSetMode(m_CurrentMode);
	}


	//!
	void ToggleDebugWindowEventOld()
	{
		//! act:0 foc:0 -> act:1 foc:0
		//! act:1 foc:0 -> act:0 foc:0
		//! act:1 foc:1 -> act:1 foc:0

		if(m_IsActive && !m_HasFocus)
		{	
			Hide();
		}
		else
		{
			Show();
			ReleaseFocus();
		}
	}



	void ToggleDebugWindowEventP()
	{
		//! act:0 foc:0 -> act:1 foc:1
		//! act:1 foc:0 -> act:1 foc:1
		//! act:1 foc:1 -> act:0 foc:0

		if (m_IsActive && m_HasFocus)
		{
			Hide();
			ReleaseFocus();
		}
		else
		{
			Show();
			CaptureFocus();
		}
	}


	void ToggleDebugWindowSetMode(int pMode)
	{
		switch (pMode)
		{
		case 0:		Hide(); ReleaseFocus();	break;
		case 1:		Show(); ReleaseFocus();	break;
		case 2:		Show(); CaptureFocus();	break;
		}
	}

	void CreateModuleWidgets()
	{
		if (!m_pUIHandler)
		{
			m_pUIHandler = new PluginDayzPlayerDebugUIHandler();
			m_pUIHandler.m_pPluginPlayerDebug = this;
		}

		if (!m_MainWnd)
		{
			m_MainWnd = GetGame().GetWorkspace().CreateWidgets("gui/layouts/debug/day_z_playerdebug.layout");
			m_MainWnd.SetHandler(m_pUIHandler);
			m_MainWnd.Show(false);
		}
		
		m_Card0	= m_MainWnd.FindAnyWidget("Card0");
		m_Card1	= m_MainWnd.FindAnyWidget("Card1");
		m_Card2	= m_MainWnd.FindAnyWidget("Card2");
		m_Card3	= m_MainWnd.FindAnyWidget("Card3");
		m_Card4	= m_MainWnd.FindAnyWidget("Card4");
		m_Card0Button	= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("Card0Button") );
		m_Card1Button	= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("Card1Button") );
		m_Card2Button	= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("Card2Button") );
		m_Card3Button	= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("Card3Button") );
		m_Card4Button	= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("Card4Button") );


		m_PlayerStatusV 	= EditBoxWidget.Cast( m_MainWnd.FindAnyWidget("PlayerStatusV") );
		m_ActionsSelector	= TextListboxWidget.Cast( m_MainWnd.FindAnyWidget("ActionsSelector") );
		m_ActionsStart		= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsStart") );
		m_ActionsEnd		= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsEnd") );
		m_ActionsFinish		= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsFinish") );
		m_ActionsCancel		= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsCancel") );
		m_ActionsOTA			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsOTA") );
		m_ActionsOTL			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsOTL") );
		m_ActionStateV		= TextWidget.Cast( m_MainWnd.FindAnyWidget("ActionStateV") );
		m_ActionEventV		= TextWidget.Cast( m_MainWnd.FindAnyWidget("ActionEventV") );
		m_ActionsGlobalStatus	= TextWidget.Cast( m_MainWnd.FindAnyWidget("ActionGlobalStatus") );
		
		
		m_ActionsFilter	= EditBoxWidget.Cast( m_MainWnd.FindAnyWidget("FilterField") );

		m_PlayerSyncCreateShadow 	= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("PlayerSyncCreate") );
		m_PlayerSyncDelete 			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("PlayerSyncDelete") );
		m_PlayerServerWalk			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("PlayerServerWalk") );

		m_ItemAccClass			= TextWidget.Cast( m_MainWnd.FindAnyWidget("ItemAccessClass") );
		m_ItemAccSClass			= TextWidget.Cast( m_MainWnd.FindAnyWidget("ItemAccessSClass") );
		m_ItemAccInst				= TextWidget.Cast( m_MainWnd.FindAnyWidget("ItemAccessInst") );

		m_ActionTypeAll		= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsGroupALL") );
		m_ActionTypeLA			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsGroupLA") );
		m_ActionTypeOA			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsGroupOA") );
		m_ActionTypeFLA			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsGroupFLA") );
		m_ActionTypeFOA			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsGroupFOA") );
		m_ActionTypeLG			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsGroupLG") );
		m_ActionTypeOG			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsGroupOG") );
		m_ActionTypeFLG			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsGroupFLG") );
		m_ActionTypeFOG			= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ActionsGroupFOG") );
		
		
		m_EventsHistory			= TextListboxWidget.Cast( m_MainWnd.FindAnyWidget("EventHistory") );
		m_ClearEventsButton		= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("ClearButton") );

		DisplayActions(-1);

		ShowCard(0);
		
		// m_DebugAgentListWidget = m_DebugRootWidget.FindAnyWidget("w_Agents");
		// m_DebugFloatsListWidget = m_DebugRootWidget.FindAnyWidget("w_Floats");
		// m_DebugStringsListWidget = m_DebugRootWidget.FindAnyWidget("w_Strings");
		// m_DebugFloatsProperListWidget = m_DebugRootWidget.FindAnyWidget("w_FloatsProper");
		// m_ItemPreviewWidget = m_DebugRootWidget.FindAnyWidget("w_ItemPreview");
		// m_ClassNameWidget = m_DebugRootWidget.FindAnyWidget("w_ClassName");
	}
	
	void DestroyModuleWidgets()
	{
		delete m_MainWnd;
	}


	void ShowCard(int pCard)
	{
		m_Card0.Show(pCard == 0);
		m_Card1.Show(pCard == 1);
		m_Card2.Show(pCard == 2);
		m_Card3.Show(pCard == 3);
		m_Card4.Show(pCard == 4);
	}

	

	//---------------------------------------------------
    // main update

	
	void Tick()
	{
		DayZPlayer player = DayZPlayer.Cast( GetGame().GetPlayer() );
		if (!player)
		{
			return;
		}
				
		// Print("PluginDayzPlayerDebug tick");	
		UpdatePlayerStatus(player);
		
		// DbgShowBoneName(player);			
		UpdateCommandModifiers(player);

		//!
		UpdateItemAccessor(player);
		
		m_Weapons.Tick();
	}
	
	
	//---------------------------------------------------
    // Player main

	
	void UpdatePlayerStatus(DayZPlayer pPlayer)
	{
		
		ref HumanMovementState		state 	= new HumanMovementState();
		
		
		pPlayer.GetMovementState(state);
				
		string a = "Cmd:"; 
		
		if (state.m_CommandTypeId == DayZPlayerConstants.COMMANDID_MOVE)
		{
			a += "MOVE:";
			a += " st:" + state.m_iStanceIdx.ToString() + ", mv:" + state.m_iMovement.ToString();
		}
		else if (state.m_CommandTypeId == DayZPlayerConstants.COMMANDID_ACTION)
		{
			a += "FB ACTION:";
			a += " st:" + state.m_iStanceIdx.ToString();
		}
		else if (state.m_CommandTypeId == DayZPlayerConstants.COMMANDID_MELEE)
		{
			a += "MELEE:";
			a += " st:" + state.m_iStanceIdx.ToString();
		}
		else if (state.m_CommandTypeId == DayZPlayerConstants.COMMANDID_MELEE2)
		{
			a += "MELEE2:";
			a += " st:" + state.m_iStanceIdx.ToString();
		}
		else if (state.m_CommandTypeId == DayZPlayerConstants.COMMANDID_FALL)
		{
			a += "FALL:";
			a += " st:" + state.m_iStanceIdx.ToString();
		}
		else if (state.m_CommandTypeId == DayZPlayerConstants.COMMANDID_DEATH)
		{
			a += "now DEAD:";
			a += " st:" + state.m_iStanceIdx.ToString();
		}
		else if (state.m_CommandTypeId == DayZPlayerConstants.COMMANDID_LADDER)
		{
			a += "LADDER:";
			a += " st:" + state.m_iStanceIdx.ToString();
		}
		else if (state.m_CommandTypeId == DayZPlayerConstants.COMMANDID_UNCONSCIOUS)
		{
			a += "Unconscious:";
			a += " st:" + state.m_iStanceIdx.ToString();
		}
		else if (state.m_CommandTypeId == DayZPlayerConstants.COMMANDID_SWIM)
		{
			a += "Swimming:";
			a += " st:" + state.m_iStanceIdx.ToString();
		}
		else if (state.m_CommandTypeId == DayZPlayerConstants.COMMANDID_CLIMB)
		{
			a += "Climbing:";
			a += " st:" + state.m_iStanceIdx.ToString();
		}
		else
		{
			a += "UNKNOWN";
		}
		
		m_PlayerStatusV.SetText(a);
	}


	void DbgShowBoneName(DayZPlayer pPlayer)
	{
		int 	boneIndex = pPlayer.GetBoneIndexByName("Head");

		// vector 	pos			= pPlayer.GetBonePositionMS(boneIndex);
		vector 	tm[4];
		
		pPlayer.GetBoneTransformMS(boneIndex, tm);

	

		string a = "Bone index:";
		a += boneIndex.ToString();
		a += ","; 
		a += tm.ToString();
		Print(a);
	}


	void UpdateCommandModifiers(DayZPlayer pPlayer)
	{
		Actions_UpdateGlobals(pPlayer);
	
		// int nMods = pPlayer.GetCommandModifierCount();
		// Print("Number of modifiers");
		// Print(nMods);

		/*
		for (int i = 0; i < nMods; i ++)
		{
			// int a = pPlayer.GetCommandModifierId(i);

			if (pPlayer.GetCommandModifierId(i) == DayZPlayerConstants.COMMANDID_MOD_ACTION)
			{
			}
			
			// Print(a);
			
		}
		*/

		HumanCommandActionCallback clbk = pPlayer.GetCommandModifier_Action();
		if (clbk)
		{
			Print(clbk);
		};
	}


	//---------------------------------------------------
    // Actions
	
	void DisplayActions(int pType)
	{
		m_CurrentType = pType;
		TStringArray sortedNames = new TStringArray();
		m_ActionsSelector.ClearItems();
		string filterTextLower = m_ActionsFilter.GetText();
		filterTextLower.ToLower();
		
		TStringArray filterTokens = new TStringArray;
		filterTextLower.Split(" ", filterTokens);
		
		foreach (PluginDayzPlayerDebugUserData dta: m_AnimationList)
		{
			string nameLower = dta.GetName();
			nameLower.ToLower();
			bool add = false;
			if (pType == -1 || pType == dta.GetCategory())
			{
				if (filterTextLower)
				{
					foreach (string token:filterTokens)
					{
						if (nameLower.Contains(token))
							add = true;
					}
				}
				else
					add = true;
			}
			if (add)
				sortedNames.Insert(dta.GetNameUnique());
		}
		sortedNames.Sort();
		foreach (int i, string sortedNameUnique:sortedNames)
		{
			PluginDayzPlayerDebugUserData data = m_AnimationListMappings.Get(sortedNameUnique);
			string sortedName = data.GetName();
			string typeName;
			
			if (m_TypeNameMappings.Contains(data.GetCategory()))
			{
				typeName = m_TypeNameMappings.Get(data.GetCategory());
			}

			m_ActionsSelector.AddItem(sortedName, data, 0,i);
			if (typeName)
				m_ActionsSelector.SetItem(i, typeName, null, 1);
		}
	}


	//!
	void	Actions_Start()
	{
		int		row = m_ActionsSelector.GetSelectedRow();
		if (row == -1)
		{
			return;
		}

		//!
		DayZPlayer player = DayZPlayer.Cast( GetGame().GetPlayer() );
		if (!player)
		{
			return;
		}

		PluginDayzPlayerDebugUserData	userData;
		m_ActionsSelector.GetItemData(row, 0, userData);

		//! start action 
		PluginDayzPlayerActionCallback a;
	
		if (userData.IsFullBody())
		{
			a = PluginDayzPlayerActionCallback.Cast( player.StartCommand_Action(userData.GetInt(), PluginDayzPlayerActionCallback, userData.GetStanceMask()) );
		}
		else
		{
			a = PluginDayzPlayerActionCallback.Cast( player.AddCommandModifier_Action(userData.GetInt(), PluginDayzPlayerActionCallback) );
		}
		m_ActionEventV.SetText("");
		a.m_pStateWidget 		= m_ActionStateV;
		a.m_pAnimEventWidget	= m_ActionEventV;
		a.m_EventsHistory		= m_EventsHistory;

		Print(m_ActionStateV);
		m_ActionStateV.SetText("Crash Test");
	}

	//!
	void	Actions_DoInternalCommand(int pCommandId)
	{
		//!
		DayZPlayer player = DayZPlayer.Cast( GetGame().GetPlayer() );
		if (!player)
		{
			return;
		}

		HumanCommandActionCallback clbk = player.GetCommand_Action();
		if (!clbk)
		{
			clbk = player.GetCommandModifier_Action();
			if (!clbk)
			{
				return;
			}
		}
		
		clbk.InternalCommand(pCommandId);		
	}

	//!
	void	Actions_DoCancel()
	{
		//!
		DayZPlayer player = DayZPlayer.Cast( GetGame().GetPlayer() );
		if (!player)
		{
			return;
		}

		HumanCommandActionCallback clbk = player.GetCommand_Action();
		if (!clbk)
		{
			clbk = player.GetCommandModifier_Action();
			if (!clbk)
			{
				return;
			}
		}
		clbk.Cancel();			
	}


	//! 
	void 	Actions_UpdateGlobals(DayZPlayer pPlayer)
	{
		HumanCommandActionCallback clbk = pPlayer.GetCommandModifier_Action();
		if (!clbk)
		{
			m_ActionsGlobalStatus.SetText("No Action");
		}

		m_ActionsGlobalStatus.SetText(clbk.ToString());
	}

        
    //! ---------------------------------------------------------


	void 	PlayerSyncCreate()
	{
		PlayerSyncDelete();

		vector item_position = 	GetGame().GetPlayer().GetPosition() + ( GetGame().GetPlayer().GetDirection() * 2.0);

		m_pPlayerShadow = Entity.Cast( GetGame().CreateObject("SurvivorM_Mirek", item_position, false, false) );

		DayZPlayer 	pl = DayZPlayer.Cast( GetGame().GetPlayer() );		
		pl.DebugSyncShadowSetup( DayZPlayer.Cast( m_pPlayerShadow ) );
	}

	void 	PlayerSyncDelete()
	{
		if (m_pPlayerShadow)
		{
			GetGame().ObjectDelete(m_pPlayerShadow);
			m_pPlayerShadow = NULL;
		}

		DayZPlayer 	pl = DayZPlayer.Cast( GetGame().GetPlayer() );		
		if (pl)
		{
			pl.DebugSyncShadowSetup(NULL);
		}

	}


	//---------------------------------------------------
	// item accessor

	void UpdateItemAccessor(DayZPlayer pPlayer)
	{
		m_ItemAccClass.SetText(pPlayer.DebugGetItemClass());
		m_ItemAccSClass.SetText(pPlayer.DebugGetItemSuperClass());
		m_ItemAccInst.SetText(pPlayer.DebugGetItemAnimInstance());
	}


	//---------------------------------------------------
    // window ui clicks

	//! buttons clicks
	bool	OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_Card0Button)
		{
			ShowCard(0);
		}
		else if (w == m_Card1Button)
		{
			ShowCard(1);
		}
		else if (w == m_Card2Button)
		{
			ShowCard(2);
		}
		else if (w == m_Card3Button)
		{
			ShowCard(3);
		}	
		else if (w == m_Card4Button)
		{
			ShowCard(4);
		}
		
		else if (w == m_ClearEventsButton)
		{
			m_EventsHistory.ClearItems();
		}
		else if (w == m_ActionsStart)
		{
			Print("PluginPlayerDebug: Action Start");
			m_CH_ActionStart = true;
			return true;
		}
		else if (w == m_ActionsEnd)
		{
			Print("PluginPlayerDebug: Action End");
			m_CH_ActionCommand = DayZPlayerConstants.CMD_ACTIONINT_END;
			return true;
		}
		else if (w == m_ActionsFinish)
		{
			Print("PluginPlayerDebug: Action Finish");
			m_CH_ActionCommand = DayZPlayerConstants.CMD_ACTIONINT_FINISH;
			return true;
		}
		else if (w == m_ActionsOTA)
		{
			Print("PluginPlayerDebug: Action One Time Action");
			m_CH_ActionCommand = DayZPlayerConstants.CMD_ACTIONINT_ACTION;
			return true;
		}
		else if (w == m_ActionsOTL)
		{
			Print("PluginPlayerDebug: Action Loop");
			m_CH_ActionCommand = DayZPlayerConstants.CMD_ACTIONINT_ACTIONLOOP;
			return true;
		}
		else if (w == m_ActionsCancel)
		{
			Print("PluginPlayerDebug: Action Cancel");
			m_CH_ActionCancel = true;
			return true;
		}
		else if (w == m_PlayerSyncCreateShadow)
		{
			PlayerSyncCreate();
			return true;
		}
		else if (w == m_PlayerSyncDelete)
		{
			PlayerSyncDelete();
			return true;
		}
		else if (w == m_PlayerServerWalk)
		{
			//! send rcp 
			m_HasServerWalk = !m_HasServerWalk;
#ifdef DEVELOPER
			Param1<bool> rp = new Param1<bool>(m_HasServerWalk);
			GetGame().RPCSingleParam(GetGame().GetPlayer(), ERPCs.RPC_DAYZPLAYER_DEBUGSERVERWALK, rp, true);
#endif
		}
		else if (w == m_ActionTypeLA)
		{
			DisplayActions(TYPE_MOD_LOOPING);
		}
		else if (w == m_ActionTypeOA)
		{
			DisplayActions(TYPE_MOD_ONETIME);
		}
		else if (w == m_ActionTypeFLA)
		{
			DisplayActions(TYPE_FB_LOOPING);
		}
		else if (w == m_ActionTypeFOA)
		{
			DisplayActions(TYPE_FB_ONETIME);
		}
		else if (w == m_ActionTypeLG)
		{
			DisplayActions(TYPE_MOD_GESTURE_LOOPING);
		}
		else if (w == m_ActionTypeOG)
		{
			DisplayActions(TYPE_MOD_GESTURE_ONETIME);
		}
		else if (w == m_ActionTypeFLG)
		{
			DisplayActions(TYPE_FB_GESTURE_LOOPING);
		}
		else if (w == m_ActionTypeFOG)
		{
			DisplayActions(TYPE_FB_GESTURE_ONETIME);
		}
		else if (w == m_ActionTypeAll)
		{
			DisplayActions(TYPE_ALL);
		}

		if (m_Weapons.OnClick(w,x,y,button))
		{
			return true;
		}
		else if (m_OtherCmds.OnClick(w,x,y,button))
		{
			return true;
		}
		else 
		{
			return m_Controls.OnClick(w,x,y,button);
		}
	}
	
	bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w == m_ActionsFilter)
		{
			DisplayActions(m_CurrentType);
		}
		
		if ( m_Controls.OnChange(w, x, y, finished) )
			return true;
		
		
		return false;
	}

	//---------------------------------------------------
    // Global handler to handle commands from player

	bool		IsWeaponChange(out string pNewWeapon, out int pHideSlot, out int pShowSlot)
	{
		if (m_Weapons)
		{
			m_Weapons.IsWeaponChange(pNewWeapon, pHideSlot, pShowSlot);
		}
		
		return false;
	}


	//---------------------------------------------------
    // Global handler to handle commands from player

	void 	CommandHandler()
	{
		if (m_CH_ActionStart)
		{
			Actions_Start();
			m_CH_ActionStart	= false;
		}

		if (m_CH_ActionCommand != -1000)
		{
			Actions_DoInternalCommand(m_CH_ActionCommand);
			m_CH_ActionCommand = -1000;
		}

		if (m_CH_ActionCancel)
		{
			Actions_DoCancel();			
			m_CH_ActionCancel = false;		
		}

		if (m_Weapons)
		{
			m_Weapons.CommandHandler();
		}

		if (m_Controls)
		{
			m_Controls.CommandHandler();	
		}
		
		if (m_OtherCmds)
		{
			m_OtherCmds.CommandHandler();	
		}
	}

	void RegisterAnimation(string name, int category, int pUserData, bool pFullBody, int pStanceMask = -1)
	{
		PluginDayzPlayerDebugUserData data = new PluginDayzPlayerDebugUserData(name, category, pUserData, pFullBody, pStanceMask);
		m_AnimationList.Insert(data);
		m_AnimationListMappings.Insert(data.GetNameUnique(), data);
	}
	
	void RegisterAnimations()
	{
		RegisterAnimation("CLEAN HANDS BOTTLE", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_CLEANHANDSBOTTLE, false);
		RegisterAnimation("DRINK", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_DRINK, false);
		RegisterAnimation("EAT", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_EAT, false);
		RegisterAnimation("EMPTY VESSEL", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_EMPTY_VESSEL, false);
		RegisterAnimation("CATCH RAIN", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_CATCHRAIN, false);
		RegisterAnimation("VIEW COMPASS", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_VIEWCOMPASS, false);
		RegisterAnimation("ITEM TUNE", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_ITEM_TUNE, false);
		RegisterAnimation("GIVE L ITEM", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_GIVEL, false);
		RegisterAnimation("GIVE R ITEM", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_GIVER, false);
		RegisterAnimation("SHAVE", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_SHAVE, false);
		RegisterAnimation("FILL MAG", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_FILLMAG, false);
		RegisterAnimation("EMPTY MAG", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_EMPTYMAG, false);
		RegisterAnimation("OPEN ITEM", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM, false);
		RegisterAnimation("TAKE TEMP SELF", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_TAKETEMPSELF, false);
		RegisterAnimation("VIEW MAP", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_VIEWMAP, false);
		RegisterAnimation("RAISE ITEM", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_RAISEITEM, false);
		RegisterAnimation("SEARCH INVENTORY", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_SEARCHINVENTORY, false);
		RegisterAnimation("CRAFTING UPPDERBODY", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_CRAFTING, false);
		RegisterAnimation("RESTRAINED STRUGGLE", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_RESTRAINEDSTRUGGLE, false);
		RegisterAnimation("COVER HEAD SELF", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_COVERHEAD_SELF, false);
		RegisterAnimation("COVER HEAD TARGET", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_COVERHEAD_TARGET, false);
		RegisterAnimation("SET ALARM CLOCK", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_SET_ALARM, false);
		RegisterAnimation("ENGINE START", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_STARTENGINE, false);
		RegisterAnimation("TOOT HORN", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_TOOTHORN, false);
		RegisterAnimation("ENGINE STOP", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_STOPENGINE, false);
		RegisterAnimation("SHIFT GEAR", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_SHIFTGEAR, false);
		RegisterAnimation("TURN ON/OFF LIGHTS", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_HEADLIGHT, false);
		RegisterAnimation("DRIVER OPEN DOOR", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_OPEN, false);
		RegisterAnimation("DRIVER CLOSE DOOR", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_CLOSE, false);
		RegisterAnimation("CODRIVER OPEN DOOR", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOOROPEN, false);
		RegisterAnimation("CODRIVER CLOSE DOOR", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE, false);
		RegisterAnimation("BLOODTEST", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_BLOODTEST, false);
		RegisterAnimation("BLOODTESTOTHER", TYPE_MOD_LOOPING, DayZPlayerConstants.CMD_ACTIONMOD_BLOODTESTOTHER, false);
		//! one time
		RegisterAnimation("PICK UP HANDS", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS, false);
		RegisterAnimation("PICK UP INVENTORY", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_INVENTORY, false);
		RegisterAnimation("LICK BATTERY", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_LICKBATTERY, false);
		RegisterAnimation("LIGHT FLARE", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_LIGHTFLARE, false);
		RegisterAnimation("LITCHEM LIGHT", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_LITCHEMLIGHT, false);
		RegisterAnimation("UNPIN GRENAGE", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_UNPINGRENAGE, false);
		RegisterAnimation("OPEN DOOR FW", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW, false);
		RegisterAnimation("OPEN LID", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_OPENLID, false);
		RegisterAnimation("CLOSE LID", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_CLOSELID, false);
		RegisterAnimation("RADIO ON", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON, false);
		RegisterAnimation("RADIO OFF", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_ITEM_OFF, false);
		RegisterAnimation("BATON EXTEND", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_BATONEXTEND, false);
		RegisterAnimation("BATON RETRACT", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_BATONRETRACT, false);
		RegisterAnimation("UNLOCK HANDCUFF TARGET", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_UNLOCKHANDCUFFTARGET, false);
		RegisterAnimation("FISHINGROD EXTEND", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_FISHINGRODEXTEND, false);
		RegisterAnimation("FISHINGROD RETRACT", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_FISHINGRODRETRACT, false);
		RegisterAnimation("OPEN ITEM ONCE", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM_ONCE, false);
		RegisterAnimation("CLOSE ITEM ONCE", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_CLOSEITEM_ONCE, false);
		RegisterAnimation("ATTACH SCOPE", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_ATTACHSCOPE, false);
		RegisterAnimation("ATTACH BARREL", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_ATTACHBARREL, false);
		RegisterAnimation("EMPTY SEEDS PACK", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_EMPTYSEEDSPACK, false);
		RegisterAnimation("INTERACT", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE, false);
		RegisterAnimation("ATTACH ITEM", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM, false);
		RegisterAnimation("STOP ALARM CLOCK", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_STOP_ALARM, false);
		RegisterAnimation("PRESS TRIGGER", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_PRESS_TRIGGER, false);
		RegisterAnimation("DROP ITEM HANDS", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_DROPITEM_HANDS, false);
		RegisterAnimation("DROP ITEM INVENTORY", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_DROPITEM_INVENTORY, false);
		RegisterAnimation("EAT PILL", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_EAT_PILL, false);
		RegisterAnimation("EAT TABLET", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_EAT_TABLET, false);
		RegisterAnimation("HEATPACK", TYPE_MOD_ONETIME, DayZPlayerConstants.CMD_ACTIONMOD_HEATPACK, false);
		//! fullbody looping
		RegisterAnimation("DRINK", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_DRINK, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("EAT", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_EAT, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("CATCH RAIN", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_CATCHRAIN, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("WALKIETALKIE TUNE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_ITEM_TUNE, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("GIVE L ITEM", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_GIVEL, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("GIVE R ITEM", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_GIVER, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("FILL MAG", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_FILLMAG, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("EMPTY MAG", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_EMPTYMAG, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("DRINK POND", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_DRINKPOND, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("DRINK WELL", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_DRINKWELL, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("FILL BOTTLE WELL", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_FILLBOTTLEWELL, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("FIRE ESTINGUISHER", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_FIREESTINGUISHER, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("FLAME REPAIR", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_FLAME_REPAIR, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("WRING CLOTH", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_WRING, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("FISHING", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_FISHING, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("CPR", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_CPR, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("BANDAGE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_BANDAGE, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("CRAFTING", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_CRAFTING, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("INTERACT", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_INTERACT, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("FORCE FEED TARGET", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_FORCEFEED, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("BANDAGE TARGET", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_BANDAGETARGET, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("SPRAY PLANT", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_SPRAYPLANT, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("START FIRE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_STARTFIRE, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("ANIMAL SKINNING", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("WASH HANDS WELL", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSWELL, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("WASH HANDS POND", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSPOND, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("SALINE BLOODBAG TARGET", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_SALINEBLOODBAGTARGET, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("SALINE BLOOD BAG", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_SALINEBLOODBAG, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("STITCH UP SELF", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_STITCHUPSELF, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("VOMIT", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_VOMIT, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("UNRESTRAINT TARGET", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_UNRESTRAINTARGET, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("RESTRAIN TARGET", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_RESTRAINTARGET, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("CHECK PULSE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_CHECKPULSE, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("CLEAN WOUNDS TARGET", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_CLEANWOUNDSTARGET, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("COLLECT BLOOD SELF", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_COLLECTBLOODSELF, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("EMPTY VESSEL", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("OPEN ITEM", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_OPENITEM, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("HACK BUSH", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_HACKBUSH, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("HACK TREE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_HACKTREE, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("TAKE TEMP SELF", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_TAKETEMPSELF, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("DIG HOLE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_DIG, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("DIG UP CACHE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_DIGUPCACHE, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("DIG MANIPULATE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_DIGMANIPULATE, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("DEPLOY HEAVY", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_HEAVY, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("DEPLOY 1HD", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_1HD, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("DEPLOY 2HD", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("BLOW FIREPLACE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_BLOWFIREPLACE, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("VIEW MAP", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_VIEWMAP, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("VIEW COMPASS", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_VIEWCOMPASS, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("FILL BOTTLE POND", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_FILLBOTTLEPOND, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("PLACING HEAVY", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_PLACING_HEAVY, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("PLACING 1HD", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_PLACING_1HD, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("PLACING 2HD", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_PLACING_2HD, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("CUT BARK", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_CUTBARK, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("VIEW NOTE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_VIEWNOTE, true, DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("SEARCH INVENTORY", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_SEARCHINVENTORY, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("LOOK OPTICS", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_LOOKOPTICS, true, DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("MINE ROCK", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_MINEROCK, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("RAISE ITEM", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_RAISEITEM, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("RESTRAINED STRUGGLE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_RESTRAINEDSTRUGGLE, true, DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("RESTRAIN SELF", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_RESTRAINSELF, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("ASSEMBLE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_ASSEMBLE, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("DISASSEMBLE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_DISASSEMBLE, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("RAISE FLAG", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_RAISE_FLAG, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("LOWER FLAG", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_LOWER_FLAG, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("SET ALARM CLOCK", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_SET_ALARM, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("SHOVEL DIG", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_SHOVEL_DIG, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("VEHICLE_PUSH", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_VEHICLE_PUSH, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("PATCHING_TIRE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_PATCHING_TIRE, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("PATCHING_DUCTTAPE", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_PATCHING_DUCTTAPE, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("PATCHING_SEWING", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_PATCHING_SEWING, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("PATCHING_LEATHER_SEWING_KIT", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_PATCHING_LEATHER_SEWING_KIT, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("SPLITTING FIREWOOD", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("BREAKING STICK", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_BREAKING_STICK, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("CLEANING_WEAPON", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_CLEANING_WEAPON, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("EATING_SNOW", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_EATING_SNOW, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("FILLING_CONTAINER_SNOW", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_FILLING_CONTAINER_SNOW, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("DIGGING_WORMS", TYPE_FB_LOOPING, DayZPlayerConstants.CMD_ACTIONFB_DIGGIN_WORMS, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);


		//! one time
		RegisterAnimation("PICK UP HANDS", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HANDS, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("PICK UP INVENTORY", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_PICKUP_INVENTORY, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("LICK BATTERY", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_LICKBATTERY, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("LIGHT FLARE", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_LIGHTFLARE, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("LITCHEM LIGHT", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_LITCHEMLIGHT, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("UNPIN GRENAGE", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_UNPINGRENAGE, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("RADIO ON", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("RADIO OFF", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_ITEM_OFF, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("HANDCUFF TARGET", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_HANDCUFFTARGET, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("MORPHINE", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_MORPHINE, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("INJECTION", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_INJECTION, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("INJECTION TARGET", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_INJECTIONTARGET, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("DRINK SIP", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_DRINKSIP, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("CLEAN HANDS BOTTLE", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_CLEANHANDSBOTTLE, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("OPEN ITEM ONCE", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_OPENITEM_ONCE, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("POKE", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_POKE, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("ATTACH SCOPE", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_ATTACHSCOPE, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("ATTACH BARREL", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_ATTACHBARREL, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("RESTRAIN", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_RESTRAIN, true, DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("PICK UP HEAVY", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HEAVY, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("STOP ALARM CLOCK", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_STOP_ALARM, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("PRESS TRIGGER", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_PRESS_TRIGGER, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("EAT PILL", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_EAT_PILL, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("EAT TABLET", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_EAT_TABLET, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("HEATPACK", TYPE_FB_ONETIME, DayZPlayerConstants.CMD_ACTIONFB_HEATPACK, true, DayZPlayerConstants.STANCEMASK_PRONE);
		//! ---------------------------------------------------------
		//! ---------------------- GESTURES -------------------------
		//! ---------------------------------------------------------
		//! looping
		RegisterAnimation("GREETING", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_GREETING, false);
		RegisterAnimation("POINT", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_POINT, false);
		RegisterAnimation("THUMB UP", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_THUMB, false);
		RegisterAnimation("THUMB DOWN", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_THUMB, false);
		RegisterAnimation("SILENCE", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_SILENCE, false);
		RegisterAnimation("TAUNT", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_TAUNT, false);
		RegisterAnimation("TIME OUT", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_TIMEOUT, false);
		RegisterAnimation("HEART", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_HEART, false);
		RegisterAnimation("FACEPALM", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_FACEPALM, false);
		RegisterAnimation("WATCHING", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_WATCHING, false);
		RegisterAnimation("HOLD", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_HOLD, false);
		RegisterAnimation("LISTENING", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_LISTENING, false);
		RegisterAnimation("POINT AT SELF", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_POINTSELF, false);
		RegisterAnimation("LOOK AT ME", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_LOOKATME, false);
		RegisterAnimation("OKAY", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_OKAY, false);
		RegisterAnimation("RPS", TYPE_MOD_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREMOD_RPS, false);
		//! one time
		RegisterAnimation("THROAT", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_THROAT, false);
		RegisterAnimation("CLAP", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_CLAP, false);
		RegisterAnimation("DABBING", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_DABBING, false);
		RegisterAnimation("MOVE", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_MOVE, false);
		RegisterAnimation("DOWN", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_DOWN, false);
		RegisterAnimation("COME", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_COME, false);
		RegisterAnimation("TAUNT KISS", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_TAUNTKISS, false);
		RegisterAnimation("TAUNT ELBOW", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_TAUNTELBOW, false);
		RegisterAnimation("TAUNT THINK", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_TAUNTTHINK, false);
		RegisterAnimation("NOD HEAD", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_NODHEAD, false);
		RegisterAnimation("SHAKE HEAD", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_SHAKEHEAD, false);
		RegisterAnimation("SHRUG", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_SHRUG, false);
		RegisterAnimation("SURRENDER", TYPE_MOD_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREMOD_SURRENDER, false);
		//! fullbody looping
		RegisterAnimation("GREETING", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_GREETING, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("POINT", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_POINT, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("THUMBUP", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_THUMB, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("THUMBDOWN", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_THUMB, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("SILENCE", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_SILENCE, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("TAUNT", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_TAUNT, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("TIMEOUT", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_TIMEOUT, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("HEART", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_HEART, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("WATCHING", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_WATCHING, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("HOLD", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_HOLD, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("LISTENING", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_LISTENING, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("POINTSELF", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_POINTSELF, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("LOOKATME", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_LOOKATME, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("OKAY", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_OKAY, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("SALUTE", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_SALUTE, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("CAMPFIRE", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_CAMPFIRE, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("LYINGDOWN", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_LYINGDOWN, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("SOS", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_SOS, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("SITA", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_SITA, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("SITB", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_SITB, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("DABBING", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_DABBING, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("SURRENDER", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_SURRENDER, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("KNEEL", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_GESTUREFB_KNEEL, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("TRAILER WAKE UP", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_TRAILER_WAKE_UP, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("TRAILER DEAD BODY", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_TRAILER_DEAD_BODY, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("TRAILER WALKIE TALKIE", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_TRAILER_WALKIE_TALKIE, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("TRAILER WOUNDED", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_TRAILER_WOUNDED, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("TRAILER WALK AWAY", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_TRAILER_WALK_AWAY, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("TRAILER DEAD DEAD", TYPE_FB_GESTURE_LOOPING, DayZPlayerConstants.CMD_TRAILER_DEAD, true, DayZPlayerConstants.STANCEMASK_ERECT);
		//! fullbody one time
		RegisterAnimation("THROAT", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREFB_THROAT, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("MOVE", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREFB_MOVE, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("DOWN", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREFB_DOWN, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("COME", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREFB_COME, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("TAUNT KISS", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREFB_TAUNTKISS, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("TAUNT THINK", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREFB_TAUNTTHINK, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("DANCE", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREFB_DANCE, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("NOD HEAD", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREFB_NODHEAD, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("SHAKE HEAD", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREFB_SHAKEHEAD, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("SHRUG", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREFB_SHRUG, true, DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("SURRENDER", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_GESTUREFB_SURRENDER, true, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_PRONE);
		RegisterAnimation("TRAILER MASK", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_TRAILER_MASK, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("TRAILER MASK_02", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_TRAILER_MASK_02, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("TRAILER MASK_CHAINS", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_TRAILER_MASK_CHAINS, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("TRAILER MASK_DALLAS", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_TRAILER_MASK_DALLAS, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("TRAILER MASK_HOXTON", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_TRAILER_MASK_HOXTON, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("TRAILER MASK_WOLF", TYPE_FB_GESTURE_ONETIME, DayZPlayerConstants.CMD_TRAILER_MASK_WOLF, true, DayZPlayerConstants.STANCEMASK_ERECT);

		//! ---------------------------------------------------------
		//! ---------------------- SUICIDE --------------------------
		//! ---------------------------------------------------------
		//! fullbody looping
		RegisterAnimation("1HD", TYPE_FB_SUICIDE_LOOPING, DayZPlayerConstants.CMD_SUICIDEFB_1HD, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("FIREAXE", TYPE_FB_SUICIDE_LOOPING, DayZPlayerConstants.CMD_SUICIDEFB_FIREAXE, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("PITCHFORK", TYPE_FB_SUICIDE_LOOPING, DayZPlayerConstants.CMD_SUICIDEFB_PITCHFORK, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("PISTOL", TYPE_FB_SUICIDE_LOOPING, DayZPlayerConstants.CMD_SUICIDEFB_PISTOL, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("RIFLE", TYPE_FB_SUICIDE_LOOPING, DayZPlayerConstants.CMD_SUICIDEFB_RIFLE, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("SWORD", TYPE_FB_SUICIDE_LOOPING, DayZPlayerConstants.CMD_SUICIDEFB_SWORD, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("UNARMED", TYPE_FB_SUICIDE_LOOPING, DayZPlayerConstants.CMD_SUICIDEFB_UNARMED, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("WOODAXE", TYPE_FB_SUICIDE_LOOPING, DayZPlayerConstants.CMD_SUICIDEFB_WOODAXE, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("SPEAR", TYPE_FB_SUICIDE_LOOPING, DayZPlayerConstants.CMD_SUICIDEFB_SPEAR, true, DayZPlayerConstants.STANCEMASK_ERECT);
		RegisterAnimation("SICKLE", TYPE_FB_SUICIDE_LOOPING, DayZPlayerConstants.CMD_SUICIDEFB_SICKLE, true, DayZPlayerConstants.STANCEMASK_CROUCH);
		RegisterAnimation("HOE", TYPE_FB_SUICIDE_LOOPING, DayZPlayerConstants.CMD_SUICIDEFB_HOE, true, DayZPlayerConstants.STANCEMASK_ERECT);
	}
			
}

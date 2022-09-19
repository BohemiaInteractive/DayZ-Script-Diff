// *************************************************************************************
// ! PluginDayZInfectedDebugUIHandler 
// *************************************************************************************
class PluginDayZInfectedDebugUIHandler extends ScriptedWidgetEventHandler
{
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		return m_pPluginInfectedDebug.OnClick(w, x, y, button);
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		return m_pPluginInfectedDebug.OnChange(w, x, y, finished);
	}

	PluginDayZInfectedDebug		m_pPluginInfectedDebug;
}

// *************************************************************************************
// ! PluginDayZInfectedDebugAttackDesc
// *************************************************************************************
class PluginDayZInfectedDebugAttackDesc
{
	void PluginDayZInfectedDebugAttackDesc(string pName, int pValue)
	{
		name = pName;
		animValue = pValue;
	}
	
	string name;
	int animValue;
}

// *************************************************************************************
// ! PluginDayZInfectedDebug  
// *************************************************************************************
class PluginDayZInfectedDebug extends PluginBase
{
	int m_CurrentMode = 0;
	bool m_HasFocus = false;
	bool m_IsActive = false;

	//! main controls	
	Widget 								m_MainWnd;
	ButtonWidget						m_SpawnEntityButton;
	ButtonWidget						m_CardMovementButton;
	ButtonWidget						m_CardFightButton;
	Widget								m_CardMovementFrame;
	Widget								m_CardFightFrame;
	TextWidget							m_StatusText;
	
	//! movement card
	XComboBoxWidget 					m_StanceCB;
	XComboBoxWidget 					m_MindStateCB;
	EditBoxWidget						m_MovementSpeedEB;
	
	ButtonWidget						m_StartTurnButton;
	XComboBoxWidget 					m_TurnTypeCB;
	EditBoxWidget 						m_TurnDirectionEB;
	
	XComboBoxWidget 					m_VaultingCB;
	ButtonWidget						m_StartVaultButton;
	ButtonWidget						m_KnucleOutButton;
	
	XComboBoxWidget 					m_CrawlingTypeCB;
	ButtonWidget						m_StartCrawlingButton;
	
	//! fight card
	EditBoxWidget 						m_HitDirEB;
	XComboBoxWidget 					m_HitTypeCB;
	XComboBoxWidget 					m_HitBodyPartCB;
	ButtonWidget						m_StartHitButton;

	XComboBoxWidget 					m_DeathTypeCB;
	ButtonWidget						m_StartDeathButton;

	XComboBoxWidget 					m_AttackTypeCB;
	XComboBoxWidget 					m_AttackDirCB;
	ButtonWidget						m_StartAttackButton;
	
	//! controlled instance	
	DayZInfected						m_ControlledInfected;
	
	//! command handler values
	bool								m_CH_DoTurn = false;
	bool								m_CH_DoVault = false;
	bool								m_CH_DoKnucleOut = false;
	bool								m_CH_GoToCrawl = false;
	bool								m_CH_DoHit = false;
	bool								m_CH_StartDeath = false;
	bool								m_CH_DoAttack = false;
	
	ref PluginDayZInfectedDebugUIHandler m_pUIHandler;
	ref array<ref PluginDayZInfectedDebugAttackDesc> m_AttackDescriptors = new array<ref PluginDayZInfectedDebugAttackDesc>;
	
	//! status refresh timer
	ref Timer							m_TickTimer;

	void PluginDayZInfectedDebug()
	{
		#ifndef NO_GUI
		CreateModuleWidgets();
		#endif
	}
	
	void ~PluginDayZInfectedDebug()
	{
		#ifndef NO_GUI
		DestroyModuleWidgets();
		#endif		
	}
	
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
	void ToggleDebugWindowSetMode(int pMode)
	{
		switch (pMode)
		{
		case 0:		Hide(); ReleaseFocus();	break;
		case 1:		Show(); ReleaseFocus();	break;
		case 2:		Show(); CaptureFocus();	break;
		}
	}

	//!
	void 	CaptureFocus()
	{
		if (!m_HasFocus)
		{
			GetGame().GetInput().ChangeGameFocus(1);
			GetGame().GetUIManager().ShowUICursor(true);
			m_HasFocus = true;
		}
	}

	//!
	void	ReleaseFocus()
	{
		if (m_HasFocus)
		{
			GetGame().GetInput().ChangeGameFocus(-1);
			GetGame().GetUIManager().ShowUICursor(false);
			m_HasFocus = false;
		}	
	}

	//!
	void Show()
	{
		ShowWidgets(true);
		m_IsActive = true;

		GetUApi().ActivateExclude("menu");
		GetUApi().UpdateControls();
		
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
		
		GetUApi().UpdateControls();
		
		m_TickTimer = NULL;
	}

	//!
	void ShowWidgets(bool show)
	{
		if(m_MainWnd) 
		{
			m_MainWnd.Show(show);
		}
	}

	//!
	void CreateModuleWidgets()
	{
		if (!m_pUIHandler)
		{
			m_pUIHandler = new PluginDayZInfectedDebugUIHandler();
			m_pUIHandler.m_pPluginInfectedDebug = this;
		}

		if (!m_MainWnd)
		{
			m_MainWnd = GetGame().GetWorkspace().CreateWidgets("gui/layouts/debug/day_z_infecteddebug.layout");
			m_MainWnd.SetHandler(m_pUIHandler);
			m_MainWnd.Show(false);
		}

		//! main controls		
		m_SpawnEntityButton = ButtonWidget.Cast( m_MainWnd.FindAnyWidget("SpawnEntityButton") );
		m_CardMovementButton = ButtonWidget.Cast( m_MainWnd.FindAnyWidget("CardMovementButton") );
		m_CardFightButton = ButtonWidget.Cast( m_MainWnd.FindAnyWidget("CardFightButton") );
		m_CardMovementFrame = m_MainWnd.FindAnyWidget("CardMovementFrame");
		m_CardFightFrame = m_MainWnd.FindAnyWidget("CardFightFrame");
		m_StatusText = TextWidget.Cast( m_MainWnd.FindAnyWidget("StatusText") );

		//! movement card
		m_StanceCB = XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("StanceCB") );
		m_MindStateCB = XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("MindStateCB") );
		m_MovementSpeedEB = EditBoxWidget.Cast( m_MainWnd.FindAnyWidget("MovementSpeedEB") );
		
		m_StartTurnButton = ButtonWidget.Cast( m_MainWnd.FindAnyWidget("StartTurnButton") );
		m_TurnTypeCB = XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("TurnTypeCB") );
		m_TurnDirectionEB = EditBoxWidget.Cast( m_MainWnd.FindAnyWidget("TurnDirectionEB") );
		
		m_VaultingCB = XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("TurnTypeCB") );
		m_StartVaultButton = ButtonWidget.Cast( m_MainWnd.FindAnyWidget("StartVaultButton") );
		m_KnucleOutButton = ButtonWidget.Cast( m_MainWnd.FindAnyWidget("KnucleOutButton") );
	
		m_CrawlingTypeCB = XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("TurnTypeCB") );
		m_StartCrawlingButton = ButtonWidget.Cast( m_MainWnd.FindAnyWidget("StartCrawlingButton") );
		
		//! fight card
		m_HitDirEB = EditBoxWidget.Cast( m_MainWnd.FindAnyWidget("HitDirEB") );
		m_HitTypeCB = XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("HitTypeCB") );
		m_HitBodyPartCB = XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("HitBodyPartCB") );
		m_StartHitButton = ButtonWidget.Cast( m_MainWnd.FindAnyWidget("StartHitButton") );
		
		m_DeathTypeCB = XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("DeathTypeCB") );
		m_StartDeathButton = ButtonWidget.Cast( m_MainWnd.FindAnyWidget("StartDeathButton") );

		m_AttackTypeCB = XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("AttackTypeCB") );
		m_AttackDirCB = XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("AttackDirCB") );
		m_StartAttackButton = ButtonWidget.Cast( m_MainWnd.FindAnyWidget("StartAttackButton") );
		
		//! fill some controls
		InitAttacks();
		
		ShowCard(0);
	}
	
	//!
	void DestroyModuleWidgets()
	{
		delete m_MainWnd;
	}
	
	//!
	void ShowCard(int pCard)
	{
		m_CardMovementFrame.Show(pCard == 0);
		m_CardFightFrame.Show(pCard == 1);
	}

	//! buttons clicks
	bool OnClick(Widget w, int x, int y, int button)
	{
		if( !m_HasFocus )
			return false;
		
		if( w == m_SpawnEntityButton )
		{
			ResetValues();
			SpawnNewInfectedEntity();
			return true;
		}
		if( w == m_CardMovementButton )
		{
			ShowCard(0);
			return true;
		}
		else if( w == m_CardFightButton )
		{
			ShowCard(1);
			return true;
		}
		else if( w == m_StartTurnButton )
		{
			m_CH_DoTurn = true;
			return true;
		}
		else if( w == m_StartVaultButton )
		{
			m_CH_DoVault = true;
			return true;
		}
		else if( w == m_KnucleOutButton )
		{
			m_CH_DoKnucleOut = true;
			return true;
		}
		else if( w == m_StartCrawlingButton )
		{
			m_CH_GoToCrawl = true;
			return true;
		}
		else if( w == m_StartHitButton )
		{
			m_CH_DoHit = true;
			return true;
		}
		else if( w == m_StartDeathButton )
		{
			m_CH_StartDeath = true;
			return true;
		}
		else if( w == m_StartAttackButton )
		{
			m_CH_DoAttack = true;
			return true;
		}
		
		return false;
	}
	
	//!
	bool OnChange(Widget w, int x, int y, bool finished)
	{
		return false;
	}

	//!
	void ResetValues()
	{
	}
	
	//!
	void SpawnNewInfectedEntity()
	{
		m_ControlledInfected = DayZInfected.Cast(GetGame().CreateObject("ZmbF_JournalistNormal_Blue", GetGame().GetPlayer().GetPosition(), 0, true));
		m_ControlledInfected.GetAIAgent().SetKeepInIdle(true);
	}
	
	//!
	void CommandHandler(DayZInfected infected)
	{
		if( infected != m_ControlledInfected )
			return;
		
		infected.GetInputController().OverrideMovementSpeed(true, m_MovementSpeedEB.GetText().ToFloat());
		
		DayZInfectedCommandMove moveCommand = infected.GetCommand_Move();
		if( moveCommand )
		{
			moveCommand.SetStanceVariation(m_StanceCB.GetCurrentItem());
			moveCommand.SetIdleState(m_MindStateCB.GetCurrentItem());
		}		

		if( m_CH_DoTurn )
		{
			moveCommand.StartTurn(m_TurnDirectionEB.GetText().ToFloat(), m_TurnTypeCB.GetCurrentItem());
			m_CH_DoTurn = false;
		}
		if( m_CH_DoVault )
		{
			infected.StartCommand_Vault(m_VaultingCB.GetCurrentItem());
			m_CH_DoVault = false;
		}
		if( m_CH_DoKnucleOut )
		{
			infected.StartCommand_Vault(-1);
			m_CH_DoKnucleOut = false;
		}
		if( m_CH_GoToCrawl )
		{
			infected.StartCommand_Crawl(m_CrawlingTypeCB.GetCurrentItem());
			m_CH_GoToCrawl = false;
		}
		if( m_CH_DoHit )
		{
			float hitDir = m_HitDirEB.GetText().ToFloat();
			int bodyPart = m_HitBodyPartCB.GetCurrentItem();
			bool heavyHit = m_HitTypeCB.GetCurrentItem() == 1;
			
			infected.StartCommand_Hit(heavyHit, bodyPart, hitDir);
			
			m_CH_DoHit = false;
		}
		if( m_CH_StartDeath )
		{
			int deathType = m_DeathTypeCB.GetCurrentItem();
			float deathDir = m_HitDirEB.GetText().ToFloat();
			
			infected.StartCommand_Death(deathType, deathDir);
			
			m_CH_StartDeath = false;
		}
		if( m_CH_DoAttack )
		{
			float attackDir = m_AttackDirCB.GetCurrentItem();
			int attackType = m_AttackDescriptors.Get(m_AttackTypeCB.GetCurrentItem()).animValue;
			
			infected.StartCommand_Attack(null, attackType, attackDir);
			
			m_CH_DoAttack = false;
		}		
	}
	
	//!
	void Tick()
	{
		if( !m_ControlledInfected )
			return;
				
	}

	//!
	void InitAttacks()
	{
		m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Attack Left Light", 0));
		m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Attack Right Light", 1));
		m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Attack Left Heavy", 2));
		m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Attack Right Heavy", 3));
		
		m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Crawl Attack", 7));
		//m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Crawl Attack Combo", 8));
		
		//m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Door Push Attack", 9));
		//m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Door Head Attack", 10));
		//m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Door Shoulder Attack", 11));
		//m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Door LR Attack", 12));
		//m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Door Kick Attack", 13));
		//m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Door Charge R", 14));
		//m_AttackDescriptors.Insert(new PluginDayZInfectedDebugAttackDesc("Door Charge L", 15));

		for( int i = 0; i < m_AttackDescriptors.Count(); ++i )
		{
			m_AttackTypeCB.AddItem(m_AttackDescriptors.Get(i).name);
		}
	}
}


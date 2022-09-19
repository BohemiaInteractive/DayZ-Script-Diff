

// *************************************************************************************
// ! PluginDayzPlayerDebug_Ctrl
// *************************************************************************************
class PluginDayzPlayerDebug_Ctrl
{	
	Widget 									m_MainWnd;

	XComboBoxWidget 						m_ForceStance;
	EditBoxWidget							m_MovSpeedEB;
	EditBoxWidget							m_MovDirEB;
	CheckBoxWidget							m_MovOverrideCheckbox;
	// 

	int 									m_ApplyStanceChange;
	bool									m_OverrideMovementChange = false;


	//---------------------------------------------------
  // gui stuff

	void PluginDayzPlayerDebug_Ctrl(Widget pMainWnd)
	{
		m_MainWnd 			= pMainWnd;
		m_ApplyStanceChange = -2;

		CreateModuleWidgets();
	}
	
	
	void ~PluginDayzPlayerDebug_Ctrl()
	{
		DestroyModuleWidgets();
	}
	
	
	void CreateModuleWidgets()
	{
		m_ForceStance = XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("StanceV") );
		m_MovSpeedEB = EditBoxWidget.Cast( m_MainWnd.FindAnyWidget("MovSpeedEB") );
		m_MovDirEB = EditBoxWidget.Cast( m_MainWnd.FindAnyWidget("MovDirEB") );
		m_MovOverrideCheckbox = CheckBoxWidget.Cast( m_MainWnd.FindAnyWidget("OverrideMovCheckBox") );
	}
	
	void DestroyModuleWidgets()
	{
	}

	

	//---------------------------------------------------
    // main update

	
	/* void Tick()
	{
		DayZPlayer player = DayZPlayer.Cast( GetGame().GetPlayer() );
		if (!player)
		{
			return;
		}
		
		if (player)
		{
		}
	} */
	

	//---------------------------------------------------
    // window ui clicks

	//! buttons clicks
	bool	OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_ForceStance)
		{
			Print("Stance changed");
			m_ApplyStanceChange = m_ForceStance.GetCurrentItem() - 1;
			return true;
		}
		else if (w == m_MovOverrideCheckbox)
		{
			Print("Movement override");
			m_OverrideMovementChange = true;
		}
		
	
		return false;
	}

	bool OnChange(Widget w, int x, int y, bool finished)
	{
		if( w == m_MovSpeedEB || w == m_MovDirEB )
		{
			Print("Movement override");
			m_OverrideMovementChange = true;
			return true;
		}
		
		return false;
	}

	//---------------------------------------------------
    // Global handler to handle commands from player

	void 	CommandHandler()
	{
		if (m_ApplyStanceChange != -2)
		{
			DayZPlayer player = DayZPlayer.Cast( GetGame().GetPlayer() );
			if (!player)
			{	
				return;
			}
			
			HumanCommandMove cm = player.GetCommand_Move();
			if (cm)
			{
				cm.ForceStance(m_ApplyStanceChange);
				m_ApplyStanceChange = -2;
			}
		}
		if (m_OverrideMovementChange)
		{
			m_OverrideMovementChange = false;
			
			player = DayZPlayer.Cast( GetGame().GetPlayer() );
			if (!player)
				return;
						
			float speed = m_MovSpeedEB.GetText().ToFloat();
			float dir = m_MovDirEB.GetText().ToFloat();
			player.GetInputController().OverrideMovementSpeed(m_MovOverrideCheckbox.IsChecked(), speed);
			player.GetInputController().OverrideMovementAngle(m_MovOverrideCheckbox.IsChecked(), dir);
		}
	}
}

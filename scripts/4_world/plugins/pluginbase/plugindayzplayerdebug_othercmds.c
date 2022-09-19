// *************************************************************************************
// ! PluginDayzPlayerDebug_OtherCmds
// *************************************************************************************

class PluginDayzPlayerDebug_OtherCmds
{	
	// widgets
	Widget								m_MainWnd;

	XComboBoxWidget 					m_DeathTypeCB;
	EditBoxWidget 						m_DeathDirectionEdit;
	ButtonWidget						m_DeathStartButton;
	
	XComboBoxWidget 					m_HitTypeCB;
	ButtonWidget						m_HitStartButton;

	XComboBoxWidget 					m_UnconTypeCB;
	ButtonWidget						m_UnconStartButton;
	ButtonWidget						m_UnconEndButton;
	
	// command handler properties
	bool								m_CH_DeathStart = false;
	bool								m_CH_HitStart = false;
	bool								m_CH_UnconStart = false;
	bool								m_CH_UnconEnd	= false;

	
	//---------------------------------------------------
	// gui stuff

	void PluginDayzPlayerDebug_OtherCmds(Widget pMainWnd)
	{
		m_MainWnd = pMainWnd;
		CreateModuleWidgets();
	}
	
	
	void ~PluginDayzPlayerDebug_OtherCmds()
	{
		DestroyModuleWidgets();
	}
	
	
	void CreateModuleWidgets()
	{
		m_DeathTypeCB			= XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("DeathTypeCB") );
		m_DeathDirectionEdit	= EditBoxWidget.Cast( m_MainWnd.FindAnyWidget("DeathDirectionEdit") );
		m_DeathStartButton		= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("DeathStartButton") );
		
		m_HitTypeCB				= XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("HitTypeCB") );
		m_HitStartButton		= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("HitStartButton") );

		m_UnconTypeCB			= XComboBoxWidget.Cast( m_MainWnd.FindAnyWidget("UnconTypeCB") );
		m_UnconStartButton		= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("UnconStartButton") );
		m_UnconEndButton		= ButtonWidget.Cast( m_MainWnd.FindAnyWidget("UnconEndButton") );
	}
	
	void DestroyModuleWidgets()
	{
	}

	//---------------------------------------------------
    // window ui clicks

	//! buttons clicks
	bool OnClick(Widget w, int x, int y, int button)
	{
		if( w == m_DeathStartButton )
		{
			Print("PluginPlayerDebug: Death Start");
			m_CH_DeathStart = true;
			return true;
		}
		else if( w == m_HitStartButton )
		{
			Print("PluginPlayerDebug: Uncon Start");
			m_CH_HitStart = true;
			return true;
		}
		else if( w == m_UnconStartButton )
		{
			Print("PluginPlayerDebug: Uncon Start");
			m_CH_UnconStart = true;
			return true;
		}
		else if( w == m_UnconEndButton )
		{
			Print("PluginPlayerDebug: Uncon End");
			m_CH_UnconEnd = true;
			return true;
		}
		
		return false;
	}


	//---------------------------------------------------
    // Global handler to handle commands from player

	void CommandHandler()
	{
		if( m_CH_DeathStart )
		{
			Death_Start();
			m_CH_DeathStart = false;
		}
		if( m_CH_HitStart )
		{
			Hit_Start();
			m_CH_HitStart = false;
		}
		if( m_CH_UnconStart )
		{
			Uncon_Start();
			m_CH_UnconStart = false;
		}
		if( m_CH_UnconEnd )
		{
			Uncon_End();
			m_CH_UnconEnd = false;
		}
	}

	//---------------------------------------------------
    // Commands start functions

	void Death_Start()
	{
		DayZPlayer player = DayZPlayer.Cast( GetGame().GetPlayer() );
		if( !player )
			return;

		int deathType = m_DeathTypeCB.GetCurrentItem();
		if( deathType > 0 )
			deathType += 9;
		
		float deathDirection = m_DeathDirectionEdit.GetText().ToInt();
		
		
		player.StartCommand_Death(deathType, deathDirection, HumanCommandDeathCallback);
	}

	void Hit_Start()
	{
		DayZPlayer player = DayZPlayer.Cast( GetGame().GetPlayer() );
		if( !player )
			return;

		float hitDirection = m_DeathDirectionEdit.GetText().ToInt();
		int hitType = m_HitTypeCB.GetCurrentItem();
		if( hitType == 0 )
		{
			player.AddCommandModifier_Damage(0, hitDirection);
		}
		else
		{
			player.StartCommand_Damage(0, hitDirection);
		}
	}

	void Uncon_Start()
	{
		DayZPlayerImplement player = DayZPlayerImplement.Cast( GetGame().GetPlayer() );
		if( !player )
			return;
		
		int type = m_UnconTypeCB.GetCurrentItem();
		
		player.m_UnconsciousDebug = true;
		player.StartCommand_Unconscious(type);
	}
	
	void Uncon_End()
	{
		DayZPlayerImplement player = DayZPlayerImplement.Cast( GetGame().GetPlayer() );
		if( !player )
			return;
		
		player.m_UnconsciousDebug = false;
		HumanCommandUnconscious hcu = player.GetCommand_Unconscious();
		if( hcu )
			hcu.WakeUp();
	}
}

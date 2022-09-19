class MainMenuNewsfeed extends ScriptedWidgetEventHandler
{
	protected Widget		m_Root;
	
	protected Widget		m_DLDLC;
	protected Widget		m_Discord;
	protected Widget		m_Feedback;
	protected Widget		m_DayZForum;
	protected Widget		m_Twitter;
	protected Widget		m_Youtube;
	
	protected TextWidget	m_MainText1;
	protected TextWidget	m_MainText2;
	protected TextWidget	m_MainText3;
	protected TextWidget	m_SecText1;
	protected TextWidget	m_SecText2;
	protected TextWidget	m_SecText3;
	
	void MainMenuNewsfeed( Widget root )
	{
		m_Root			= root;
		
		m_DLDLC			= m_Root.FindAnyWidget( "downloaddlc" );
		m_Discord		= m_Root.FindAnyWidget( "discord" );
		m_Feedback		= m_Root.FindAnyWidget( "feedback_tracker" );
		m_DayZForum		= m_Root.FindAnyWidget( "dayz_forums" );
		m_Twitter		= m_Root.FindAnyWidget( "twitter" );
		m_Youtube		= m_Root.FindAnyWidget( "youtube" );
		
		m_MainText1		= TextWidget.Cast( m_Root.FindAnyWidget( "SGInfoT1" ) );
		m_MainText2		= TextWidget.Cast( m_Root.FindAnyWidget( "SGInfoT2" ) );
		m_MainText3		= TextWidget.Cast( m_Root.FindAnyWidget( "SGInfoT3" ) );
		m_SecText1		= TextWidget.Cast( m_Root.FindAnyWidget( "SGInfoC1" ) );
		m_SecText2		= TextWidget.Cast( m_Root.FindAnyWidget( "SGInfoC2" ) );
		m_SecText3		= TextWidget.Cast( m_Root.FindAnyWidget( "SGInfoC3" ) );
		
		ShowNewsfeed();
		
		m_Root.SetHandler( this );
	}
	
	void ShowNewsfeed()
	{
		//m_Root.Show( true );
		m_MainText1.SetText( "#layout_mainmenu_newsfeed_sgz_title_1" );
		m_MainText1.Update();
		m_MainText2.SetText( "#layout_mainmenu_newsfeed_sgz_title_2" );
		m_MainText2.Update();
		m_MainText3.SetText( "#layout_mainmenu_newsfeed_sgz_title_3" );
		m_MainText3.Update();
		m_SecText1.SetText( "#layout_mainmenu_newsfeed_sgz_text_1" );
		m_SecText1.Update();
		m_SecText2.SetText( "#layout_mainmenu_newsfeed_sgz_text_2" );
		m_SecText2.Update();
		m_SecText3.SetText( "#layout_mainmenu_newsfeed_sgz_text_3" );
		m_SecText3.Update();
	}
	
	void HideNewsfeed()
	{
		m_Root.Show( false );
	}
	
	void OpenDLC()
	{
		GetGame().OpenURL( "https://store.steampowered.com/app/830660/Survivor_GameZ/" );
	}
	
	void OpenDiscord()
	{
		GetGame().OpenURL( "https://discord.gg/bXkyMNm" );
	}
	
	void OpenFeedback()
	{
		GetGame().OpenURL( "https://feedback.bistudio.com/tag/dayz" );
	}
	
	void OpenForums()
	{
		GetGame().OpenURL( "https://forums.dayz.com" );
	}
	
	void OpenTwitter()
	{
		GetGame().OpenURL( "https://twitter.com/DayZ" );
	}
	
	void OpenYoutube()
	{
		GetGame().OpenURL( "https://www.youtube.com/user/DayZDevTeam" );
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		if( button == MouseState.LEFT )
		{
			if ( w == m_DLDLC )
			{
				OpenDLC();
				return true;
			}
			else if ( w == m_Discord )
			{
				OpenDiscord();
				return true;
			}
			else if ( w == m_Feedback )
			{
				OpenFeedback();
				return true;
			}
			else if ( w == m_DayZForum )
			{
				OpenForums();
				return true;
			}
			else if ( w == m_Twitter )
			{
				OpenTwitter();
				return true;
			}
			else if ( w == m_Youtube )
			{
				OpenYoutube();
				return true;
			}
		}
		return false;
	}
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorRed( w, x, y );
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorWhite( w, enterW, x, y );
			return true;
		}
		return false;
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorRed( w, x, y );
			return true;
		}
		return false;
	}
	
	override bool OnFocusLost( Widget w, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorWhite( w, null, x, y );
			return true;
		}
		return false;
	}
	
	bool IsFocusable( Widget w )
	{
		if( w )
		{
			return ( w == m_DLDLC || w == m_Discord || w == m_Feedback || w == m_DayZForum || w == m_Twitter || w == m_Youtube );
		}
		return false;
	}
	
	//Coloring functions (Until WidgetStyles are useful)
	void ColorRed( Widget w, int x, int y )
	{
		SetFocus( w );
		if( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			button.SetTextColor( ARGB( 255, 255, 0, 0 ) );
			button.SetAlpha( 0.9 );
		}
		
		TextWidget text		= TextWidget.Cast(w.FindWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindWidget( w.GetName() + "_text_1" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindWidget( w.GetName() + "_image" ) );
		
		if( text )
		{
			text.SetColor( ARGB( 255, 255, 0, 0 ) );
		}
		
		if( text2 )
		{
			text2.SetColor( ARGB( 255, 255, 0, 0 ) );
		}
		
		if( image )
		{
			image.SetColor( ARGB( 255, 255, 0, 0 ) );
		}
	}
	
	void ColorWhite( Widget w, Widget enterW, int x, int y )
	{
		if( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			button.SetTextColor( ARGB( 255, 255, 255, 255 ) );
			button.SetAlpha( 0.75 );
		}
		
		TextWidget text		= TextWidget.Cast(w.FindWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindWidget( w.GetName() + "_text_1" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindWidget( w.GetName() + "_image" ) );
		
		if( text )
		{
			text.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if( text2 )
		{
			text2.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if( image )
		{
			image.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
	}
}

class CreditsMenu extends UIScriptedMenu
{
	protected static float						MENU_FADEIN_TIME	= 2.0;		//fade starts as soon as menu opens
	protected static float						LOGO_FADEIN_TIME	= 1.0;		//fade starts halfway through menu fade in
	protected static float						CREDIT_SCROLL_SPEED	= 200.0;	//pixels per second (relative to 1080p res)
	
	protected float								m_MenuFadeInIncrement;
	protected float								m_MenuFadeInLevel;
	protected float								m_MenuFadeInLevel2 = 1;
	protected float								m_LogoFadeInIncrement;
	protected float								m_LogoFadeInLevel;
	protected float								m_ScrollIncrement;
	protected float								m_ScrollLevel;
	protected float								m_ScrollMax;
	protected float								m_ScrollSize;
	
	protected ref JsonDataCredits				m_CreditsData;
	
	protected float								m_CurrentTime = 0.0;
	protected ref array<ref CreditsElement>		m_CreditsEntries = new array<ref CreditsElement>;
	
	protected ImageWidget						m_Logo;
	protected ScrollWidget						m_Scroller;
	protected WrapSpacerWidget					m_Content;
	protected RichTextWidget 					m_InfoPanelText;
	protected Widget 							m_InfoPanel;
	
	override Widget Init()
	{
		float x_f;
		int x, y;
		
		layoutRoot	= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/credits/credits_menu.layout", null );
		m_Logo		= ImageWidget.Cast( layoutRoot.FindAnyWidget( "Logo" ) );
		m_Scroller	= ScrollWidget.Cast( layoutRoot.FindAnyWidget( "CreditsPanel" ) );
		m_Content	= WrapSpacerWidget.Cast( layoutRoot.FindAnyWidget( "CreditsContent" ) );
		m_InfoPanelText = RichTextWidget.Cast( layoutRoot.FindAnyWidget( "InfoPanelText" ) );
		m_InfoPanel = layoutRoot.FindAnyWidget( "InfoPanel" );
		
		GetScreenSize( x, y );
		
		m_MenuFadeInIncrement	= 1 / MENU_FADEIN_TIME;
		m_LogoFadeInIncrement	= 1 / LOGO_FADEIN_TIME;
		m_ScrollIncrement		= CREDIT_SCROLL_SPEED * ( y / 1080 );
		
		m_Scroller.VScrollToPos01( 0 );
		m_Scroller.GetScreenSize( x_f, m_ScrollSize );
		
		GetGame().GameScript.Call( this, "LoadDataAsync", null );
		
		UpdateInfoPanelText(GetGame().GetInput().GetCurrentInputDevice());
		
		return layoutRoot;
	}
	
	void LoadDataAsync()
	{
		m_CreditsData = CreditsLoader.GetData();
		for( int i = 1; i <= m_CreditsData.Departments.Count(); i++ )
		{
			ref CreditsDepartmentElement e = new CreditsDepartmentElement( i, m_Content, m_CreditsData.Departments.Get( i - 1 ) );
			m_CreditsEntries.Insert( e );
		}
		m_Content.Update();
	}
	
	override void Update( float timeslice )
	{
		float new_menu_val;
		if( m_LogoFadeInLevel != 1 )
		{
			new_menu_val = m_MenuFadeInLevel + m_MenuFadeInIncrement * timeslice;
			if( new_menu_val < 1 )
				m_MenuFadeInLevel = new_menu_val;
			else
				m_MenuFadeInLevel = 1;
			
			if( m_MenuFadeInLevel > 0.5 )
			{
				float new_logo_val = m_LogoFadeInLevel + m_LogoFadeInIncrement * timeslice;
				if( new_menu_val < 1 )
					m_LogoFadeInLevel = new_logo_val;
				else
					m_LogoFadeInLevel = 1;
			}
			
			layoutRoot.SetAlpha( m_MenuFadeInLevel );
			m_Logo.SetAlpha( m_LogoFadeInLevel );
			m_InfoPanelText.SetAlpha( m_MenuFadeInLevel );
		}
		else if( m_ScrollLevel + m_ScrollSize <= m_Scroller.GetContentHeight() )
		{
			float new_scroll_val = m_ScrollLevel + m_ScrollIncrement * timeslice;
			m_ScrollLevel = new_scroll_val;
			m_Scroller.VScrollToPos( m_ScrollLevel );
		}
		else
		{
			new_menu_val = m_MenuFadeInLevel2 - m_MenuFadeInIncrement * timeslice;
			if( new_menu_val > 0 )
				m_MenuFadeInLevel2 = new_menu_val;
			else
				Close();
			
			layoutRoot.SetAlpha( m_MenuFadeInLevel2 );
			m_InfoPanelText.SetAlpha( m_MenuFadeInLevel2 );
		}
		
		m_CurrentTime += timeslice;
		
		if( GetGame().GetInput().LocalRelease("UAUIBack") )
		{
			Close();
		}
	}
	
	void UpdateInfoPanelText(int input_device_type)
	{
		if (GetGame().GetInput().IsEnabledMouseAndKeyboard() && input_device_type == EInputDeviceType.MOUSE_AND_KEYBOARD)
		{
			m_InfoPanelText.SetText("ESC " + "#menu_back");
		}
		else
		{
			m_InfoPanelText.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#menu_back", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		}
	}
}
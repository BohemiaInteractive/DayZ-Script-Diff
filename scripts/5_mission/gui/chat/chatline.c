class ChatLine
{
	// Consts
	const float FADE_TIMEOUT = 30;
	const float FADE_OUT_DURATION = 3;
	const float FADE_IN_DURATION = 0.5;
	
	private const string RADIO_PREFIX = "(" + "#str_radio" + ") ";
	private const string GAME_PREFIX = "(" + "#layout_chat_game" + ") ";
	private const string ADMIN_PREFIX = "(" + "#STR_MP_MASTER" + ") ";
	private const int 	 DEFAULT_COLOUR = ARGB(255, 255, 255, 255);
	private const int 	 GAME_TEXT_COLOUR = ARGB(255, 255, 0, 0);
	private const int 	 ADMIN_TEXT_COLOUR = ARGB(255, 255, 255, 0);
	
	// Widgets
	Widget							m_RootWidget;
	TextWidget						m_NameWidget;
	TextWidget						m_TextWidget;
	
	private ref WidgetFadeTimer 	m_FadeTimer;
	private ref Timer 				m_TimeoutTimer;

	void ChatLine(Widget root_widget)
	{
		m_RootWidget	= GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_chat_item.layout", root_widget);
	
		m_NameWidget	= TextWidget.Cast( m_RootWidget.FindAnyWidget( "ChatItemSenderWidget" ) );
		m_TextWidget	= TextWidget.Cast( m_RootWidget.FindAnyWidget( "ChatItemTextWidget" ) );
		
		m_FadeTimer		= new WidgetFadeTimer;
		m_TimeoutTimer	= new Timer(CALL_CATEGORY_GUI);
	}

	void ~ChatLine()
	{
		delete m_TextWidget;
	}

	void Set(ChatMessageEventParams params)	// Param 1 --> Channel, Param 2 --> sender name, Param 3 --> message, Param 4 ?? 
	{
		int channel; 
		
		m_NameWidget.SetText("");
		m_TextWidget.SetText("");
		
		SetColour(DEFAULT_COLOUR);
		m_RootWidget.Show(true);
		
		channel = params.param1; // 0 = Survior   1 = Game/System   2 = Admin 

		/*
		if (params.param2 != "")
		{
			m_NameWidget.SetText( params.param2 + " : "); 
		}
		
		if( channel & CCSystem )
 		{
			if(params.param2 != "")
			{
				m_NameWidget.SetText(GAME_PREFIX + ": " );
			} 
			SetColour(GAME_TEXT_COLOUR);
 		}
		else if( channel & CCAdmin )
		{
			m_NameWidget.SetText(ADMIN_PREFIX + ": ");
			SetColour(ADMIN_TEXT_COLOUR);			
		}
		else if( channel & CCTransmitter )
		{
			m_NameWidget.SetText(RADIO_PREFIX + params.param2 + " : ");
		}		
		*/
		
		if ( channel & CCSystem )
		{
			// Game
			if (params.param2 != "")
			{
				m_NameWidget.SetText(GAME_PREFIX + ": " );
			}
			
			SetColorByParam(params.param4, GAME_TEXT_COLOUR);
		}
		else if ( channel & CCAdmin )
		{
			// Admin
			m_NameWidget.SetText(ADMIN_PREFIX + ": ");
			SetColorByParam(params.param4, ADMIN_TEXT_COLOUR);
		}
		else if ( channel & CCTransmitter )
		{
			// Radio - Trasnmitter
			m_NameWidget.SetText(RADIO_PREFIX + params.param2 + " : ");
		}
		else if ( channel == 0 || channel & CCDirect )
		{
			// Player
			if (params.param2 != "")
			{
				m_NameWidget.SetText(params.param2 + " : ");
			}
		}
		
		
		m_TextWidget.SetText(params.param3);		
		
		m_FadeTimer.FadeIn(m_RootWidget, FADE_IN_DURATION);
		m_TimeoutTimer.Run(FADE_TIMEOUT, m_FadeTimer, "FadeOut", new Param2<Widget, float>(m_RootWidget, FADE_OUT_DURATION));
	}
	
	private void SetColour(int colour)
	{
		m_NameWidget.SetColor(colour);
		m_TextWidget.SetColor(colour);
	}
	
	protected void SetColorByParam(string pParamValue, int pFallback)
	{
		if (pParamValue != "")
		{
			SetColour(ColorNameToColor(pParamValue));
			return;
		}
		
		SetColour(pFallback);
	}

	void Clear()
	{
		m_RootWidget.Show( false );
		m_TimeoutTimer.Stop();
		m_FadeTimer.Stop();
	}
	
	protected int ColorNameToColor(string pColorName)
	{
		int color = 0xFFFFFFFF;
		switch (pColorName)
		{
		case "colorStatusChannel":
			color = COLOR_BLUE;
		break;
		case "colorAction":
			color = COLOR_YELLOW;
		break;
		case "colorFriendly":
			color = COLOR_GREEN;
		break;
		case "colorImportant":
			color = COLOR_RED;
		break;
		}
		
		return color;
	}
}
class RespawnDialogue extends UIScriptedMenu
{
	const int 					ID_RESPAWN_CUSTOM = 101;
	const int 					ID_RESPAWN_RANDOM = 102;
	
	//tooltips
	protected Widget			m_DetailsRoot;
	protected TextWidget		m_DetailsLabel;
	protected RichTextWidget	m_DetailsText;
	
	protected Widget 			m_CustomRespawn;
	
	//helper
	protected Widget 			m_CurrentlyHighlighted;
	
	void RespawnDialogue();
	void ~RespawnDialogue();
	
	override Widget Init()
	{
		layoutRoot 					= GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_respawn_dialogue.layout");
		m_DetailsRoot 				= layoutRoot.FindAnyWidget("menu_details_tooltip");
		m_DetailsLabel				= TextWidget.Cast(m_DetailsRoot.FindAnyWidget("menu_details_label"));
		m_DetailsText				= RichTextWidget.Cast(m_DetailsRoot.FindAnyWidget("menu_details_tooltip_content"));
		
		m_CustomRespawn 			= layoutRoot.FindAnyWidget("respawn_button_custom");
		SetFocus(m_CustomRespawn);

		return layoutRoot;
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		if (GetUApi().GetInputByID(UAUIBack).LocalPress() || GetUApi().GetInputByID(UAUIMenu).LocalPress())
			Close();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		switch (w.GetUserID())
		{
			case IDC_CANCEL:
				Close();
				return true;

			case ID_RESPAWN_CUSTOM:
				return RequestRespawn(false);

			case ID_RESPAWN_RANDOM:
				return RequestRespawn(true);
		}

		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		string tooltip_header = "";
		string tooltip_text = "";
		ColorHighlight(w);
		switch (w.GetUserID())
		{
			case ID_RESPAWN_RANDOM:
				tooltip_header = "#main_menu_respawn_random";
				tooltip_text = "#main_menu_respawn_random_tooltip";
				break;
			
			case ID_RESPAWN_CUSTOM:
				tooltip_header = "#main_menu_respawn_custom";
				tooltip_text = "#main_menu_respawn_custom_tooltip";
				break;
		}

		SetTooltipTexts(w, tooltip_header, tooltip_text);
		return true;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		ColorNormal(w);
		return true;
	}
	
	override void OnShow()
	{
		super.OnShow();
		
		SetFocus(m_CustomRespawn);
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		string tooltip_header = "";
		string tooltip_text = "";
		if (IsFocusable(w))
		{
			ColorHighlight(w);
			switch (w.GetUserID())
			{
				case ID_RESPAWN_RANDOM:
					tooltip_header = "#main_menu_respawn_random";
					tooltip_text = "#main_menu_respawn_random_tooltip";
					break;
				
				case ID_RESPAWN_CUSTOM:
					tooltip_header = "#main_menu_respawn_custom";
					tooltip_text = "#main_menu_respawn_custom_tooltip";
					break;
			}

			SetTooltipTexts(w, tooltip_header, tooltip_text);
			return true;
		}

		SetTooltipTexts(w, tooltip_header, tooltip_text);
		return false;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (IsFocusable(w))
		{
			ColorNormal(w);
			return true;
		}

		return false;
	}
	
	bool IsFocusable(Widget w)
	{
		if (w)
		{
			if (w.GetUserID() == IDC_CANCEL || w.GetUserID() == ID_RESPAWN_CUSTOM || w.GetUserID() == ID_RESPAWN_RANDOM);
				return true;
		}

		return false;
	}
	
	protected void ColorHighlight(Widget w)
	{
		if (!w)
			return;

		if (m_CurrentlyHighlighted != w)
		{
			if (m_CurrentlyHighlighted)
				ColorNormal(m_CurrentlyHighlighted);

			m_CurrentlyHighlighted = w;
		}

		ButtonSetColor(w, ARGB(255, 0, 0, 0));
		ButtonSetTextColor(w, ARGB(255, 255, 0, 0));
	}
	
	protected void ColorNormal(Widget w)
	{
		if (!w)
			return;
		
		ButtonSetColor(w, ARGB(0, 0, 0, 0));
		ButtonSetTextColor(w, ARGB(255, 255, 255, 255));
	}
	
	protected void ButtonSetColor(Widget w, int color)
	{
		Widget panel = w.FindWidget(w.GetName() + "_panel");
		if (panel)
			panel.SetColor(color);
	}
	
	protected void ButtonSetTextColor(Widget w, int color)
	{
		TextWidget label = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		if (label)
			label.SetColor(color);
	}
	
	void SetTooltipTexts(Widget w, string header = "", string desc = "")
	{
		bool show = header != "" && desc != "";
		m_DetailsRoot.Show(show);
		m_DetailsLabel.SetText(header);
		m_DetailsText.SetText(desc);
		
		m_DetailsText.Update();
		m_DetailsLabel.Update();
		m_DetailsRoot.Update();
	}
	
	bool RequestRespawn(bool random)
	{
		IngameHud.Cast(GetGame().GetMission().GetHud()).InitBadgesAndNotifiers();
		Man player = GetGame().GetPlayer();
		if (player && (player.GetPlayerState() == EPlayerStates.ALIVE && !player.IsUnconscious()))
			return false;
		
		#ifdef PLATFORM_CONSOLE
		InGameMenuXbox menu_ingame = InGameMenuXbox.Cast(GetGame().GetUIManager().FindMenu(MENU_INGAME));
		#else
		InGameMenu menu_ingame = InGameMenu.Cast(GetGame().GetUIManager().FindMenu(MENU_INGAME));
		#endif
		
		if (!menu_ingame)
			return false;
		
		menu_ingame.MenuRequestRespawn(this, random);
		return true;
	}
}

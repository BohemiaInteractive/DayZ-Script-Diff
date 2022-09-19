class ActionMenu
{
	private const float FADE_IN_TIME = 0.3;
	private const float FADE_OUT_TIME = 0.3;
	private const float HIDE_MENU_TIME = 5;

	private ref WidgetFadeTimer m_FadeTimer;
	private ref array<TextWidget> m_actionItems;
	private TextWidget m_defaultActionWidget;
	private Widget m_actionsPanelWidget;
	private ref Timer m_hide_timer;
	
	private bool m_visible;

	void ActionMenu()
	{
		m_actionItems = new array<TextWidget>;
		m_FadeTimer = new WidgetFadeTimer;
		m_hide_timer = new Timer(CALL_CATEGORY_GUI);
		m_visible = false;
	}

	void Init(Widget actions_panel_widget, TextWidget default_action_widget)
	{
		m_defaultActionWidget = default_action_widget;
		m_actionsPanelWidget = actions_panel_widget;
		m_actionItems.Clear();
		int i = 0;
		TextWidget w = TextWidget.Cast( m_actionsPanelWidget.FindAnyWidget("ActionListItem" + i.ToString()) );

		while (w)
		{
			m_actionItems.Insert(w);
			w.Show(false);
			i++;
			w = TextWidget.Cast( m_actionsPanelWidget.FindAnyWidget("ActionListItem" + i.ToString()) );
		}

		m_defaultActionWidget.Show(false);
		m_actionsPanelWidget.Show(true);
		m_actionsPanelWidget.SetAlpha(0);
	}
	
	void UpdateWidgets()
	{
#ifdef DEVELOPER
		// tmp
		bool newATSelection = true;

		newATSelection = DiagMenu.GetBool( DiagMenuIDs.DM_ACTION_TARGETS_NEW, true );

		// read the action manager
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		TSelectableActionInfoArray actions = player.GetActionManager().GetSelectableActions();
		int selectedActionIndex = player.GetActionManager().GetSelectedActionIndex();

		// clear state
		m_defaultActionWidget.Show(false);
		if( actions.Count() <= 1 )
			m_FadeTimer.FadeOut(m_actionsPanelWidget, FADE_OUT_TIME, true);		
		else
			m_FadeTimer.FadeIn(m_actionsPanelWidget, FADE_IN_TIME, true);
		
		// update state
		for (int i = 0; i < m_actionItems.Count(); i++)
		{
			TextWidget w = m_actionItems.Get(i);

			if (i < actions.Count())
			{
				TSelectableActionInfo actionInfo = actions.Get(i);
				w.SetText(actionInfo.param3);

				if (i == selectedActionIndex)
				{
					w.SetColor(0xFFFFEC44);
					
					m_defaultActionWidget.SetText(actionInfo.param3);
					if ( !newATSelection )
					{
						m_defaultActionWidget.Show(true);
					}
				}
				else
				{
					w.SetColor(0xFFFFFFFF);
				}

				w.Show(true);
			}
			else
			{
				w.Show(false);
			}
		}

		float sx;
		float sy;
		m_actionsPanelWidget.GetSize(sx, sy);
		m_actionsPanelWidget.SetSize(sx, 8 + (actions.Count() * 32));
#endif
	}

	void NextAction()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		player.GetActionManager().SelectNextAction();
		
		UpdateWidgets();
		
		m_hide_timer.Run(HIDE_MENU_TIME, this, "HideWithFadeout");
	}

	void PrevAction()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		player.GetActionManager().SelectPrevAction();
		
		UpdateWidgets();
		
		m_hide_timer.Run(HIDE_MENU_TIME, this, "HideWithFadeout");
	}
	
	void NextActionCategory()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		player.GetActionManager().SelectNextActionCategory();
		
		UpdateWidgets();
		
		m_hide_timer.Run(HIDE_MENU_TIME, this, "HideWithFadeout");
	}

	void PrevActionCategory()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		player.GetActionManager().SelectPrevActionCategory();
		
		UpdateWidgets();
		
		m_hide_timer.Run(HIDE_MENU_TIME, this, "HideWithFadeout");
	}

	void Show()
	{
		if( m_visible )
			return;
				
		m_visible = true;
		m_hide_timer.Run(HIDE_MENU_TIME, this, "HideWithFadeout");
	}
	
	void Refresh()
	{
		if( !m_visible )
			Show();
		
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if( player.GetActionManager().IsSelectableActionsChanged() )
			UpdateWidgets();
	}
	
	void Hide()
	{
		if( !m_visible )
			return;
		
		m_visible = false;
		m_hide_timer.Stop();
		m_FadeTimer.Stop();
		//m_defaultActionWidget.Show(false);
		m_actionsPanelWidget.Show(false);
	}
	
	void HideWithFadeout()
	{
		//m_defaultActionWidget.Show(false);
		if (m_actionsPanelWidget.IsVisible())
			m_FadeTimer.FadeOut(m_actionsPanelWidget, FADE_OUT_TIME, true);
	}	
}
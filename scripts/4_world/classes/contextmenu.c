//--------------------------------------------------------------------------
class ContextMenu extends ScriptedWidgetEventHandler
{
	protected static ref ContextMenu m_ContextMenuInstance;
	
	Widget m_context_menu_root_widget;
	private Widget m_context_menu_panel_widget;
	private ref array<ref CallQueueContext> m_commands;
	private int m_max_item_width;
	private int m_count;
	private bool m_builtIn = false;
	const int ITEMS_COUNT = 27;
	
	//--------------------------------------------------------------------------
	void ContextMenu()
	{
		m_commands = new array<ref CallQueueContext>;
		m_count = 0;
	}
	//--------------------------------------------------------------------------
	void ~ContextMenu()
	{
		Clear();
	
		delete m_context_menu_root_widget;
	}
	//--------------------------------------------------------------------------
	void Init(Widget layoutRoot, bool builtIn = false)
	{
		m_builtIn = builtIn;
		if (!m_context_menu_root_widget)
		{
			m_context_menu_root_widget = g_Game.GetWorkspace().CreateWidgets("gui/layouts/day_z_inventory_context_menu.layout", layoutRoot);
			m_context_menu_panel_widget = m_context_menu_root_widget.FindAnyWidget("PanelWidget");
			m_context_menu_root_widget.Show(false);
			m_context_menu_root_widget.SetHandler(this);
		}
	}
	
	//--------------------------------------------------------------------------
	void Show(int x, int y)
	{
		if ( m_count == 0) return;
		int screen_w, screen_h;
		float w, h;
		float sx, sy;
		int offset_x;// = -20;
		int offset_y;// = -10;
	
		GetScreenSize(screen_w, screen_h);
	
		// align buttons
		float button_height_percent = 0.02;  // button height is 4% of screen height
		float button_height = screen_h * button_height_percent;
		
		for ( int i = 0; i < m_count; i++)
		{
			ButtonWidget menu_button = ButtonWidget.Cast( m_context_menu_root_widget.FindAnyWidget( String( "Button" + (i+1).ToString() ) ) );
			if (menu_button)
			{
				menu_button.SetSize(0.90, button_height);
				menu_button.Show(true);
			}
		}
	
	
		AutoHeightSpacer spacer;
		m_context_menu_panel_widget.GetScript(spacer);
		if ( spacer )
		{
			spacer.Update();	
		}
	
		m_context_menu_root_widget.GetSize(w, h);
		m_context_menu_panel_widget.GetSize(sx, sy);
		m_context_menu_root_widget.SetSize(w, sy);
	
		// set position
		m_context_menu_root_widget.GetScreenSize(w,h);
		screen_w -= 10;
		screen_h -= 10;
	
		int right_edge = x + w - offset_x;
		if (right_edge > screen_w) 
		{
			x = screen_w - w - offset_x;
		}
		else
		{
			x = x + offset_x;
		}
	
		int bottom_edge = y + h - offset_y;
		if (bottom_edge > screen_h)
		{
			y = y - h - offset_y;
		}
		else
		{
			y = y + offset_y;
		}
	
		m_context_menu_root_widget.SetPos(x, y);
		m_context_menu_root_widget.Show(true);
	}	
	//--------------------------------------------------------------------------
	void SetSize(float x, float y)
	{
		m_context_menu_root_widget.SetSize(x, y);
	}

	//--------------------------------------------------------------------------
	void ShowBackdrop(bool show)
	{
		if (show == true)
		{
			m_context_menu_root_widget.FindAnyWidget("BackdropImageWidget").Show(true);
		}
		else
		{
			m_context_menu_root_widget.FindAnyWidget("BackdropImageWidget").Show(false);
		}
	}
	
	//--------------------------------------------------------------------------
	void Hide()
	{
		m_context_menu_root_widget.Show(false);
	
		Clear();
	}
	
	//--------------------------------------------------------------------------
	bool IsVisible()
	{
		return m_context_menu_root_widget.IsVisible();
	}
	
	//--------------------------------------------------------------------------
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		super.OnMouseLeave(w, enterW, x, y);
		
		if ( !m_builtIn && enterW && m_context_menu_panel_widget && enterW != m_context_menu_panel_widget && enterW.GetParent() != m_context_menu_panel_widget )
		{
			Hide();
			return true;
		}
		return false;
	}
	
	//--------------------------------------------------------------------------
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		super.OnMouseButtonDown(w, x, y, button);
		
		if (button == MouseState.LEFT && w.GetUserID() > -1 && w.GetUserID() < m_commands.Count())
		{
			CallQueueContext ctx = m_commands.Get(w.GetUserID());

			int actionId = Param3<EntityAI, int, int>.Cast(ctx.m_params).param2;
			if (actionId == EActions.DELETE)
				Hide();
			
			UIScriptedMenu menu = g_Game.GetUIManager().GetMenu();
			if (menu)
				g_Game.GetCallQueue(CALL_CATEGORY_GUI).Call(menu.Refresh);
			
			ctx.Call();

			return true;
		}

		return false;
	}
	
	//--------------------------------------------------------------------------
	void Add(string label, Class obj, string fn_name, Param params)
	{
		 AddEx(label, FadeColors.LIGHT_GREY, obj, fn_name, params);
	}
	
	void AddEx(string label, int labelColor, Class obj, string funcName, Param params)
	{
		int count = Count();
		ButtonWidget menuButton = ButtonWidget.Cast(m_context_menu_root_widget.FindAnyWidget(string.Format("Button%1", count + 1)));
		if (menuButton)
		{
			label.ToUpper();
			menuButton.SetText(label);
			menuButton.SetTextColor(labelColor);
			menuButton.Show(true);
			
			if (funcName == "")
			{
				menuButton.SetFlags(menuButton.GetFlags() | WidgetFlags.IGNOREPOINTER);
			}
			else
			{
				menuButton.ClearFlags(WidgetFlags.IGNOREPOINTER);
			}

			int itemWidth = label.Length();
			if (m_max_item_width < itemWidth)
				m_max_item_width = itemWidth;
		}
		
		m_count++;
		m_commands.Insert(new CallQueueContext(obj, funcName, params));
	}
	
	//--------------------------------------------------------------------------
	void Remove(int index)
	{
		if (index < m_commands.Count())
		{
			m_commands.RemoveOrdered(index);
			ButtonWidget menu_button = ButtonWidget.Cast( m_context_menu_root_widget.FindAnyWidget( String( "Button" + ( index + 1 ).ToString() ) ) );
			menu_button.Show( false );
			menu_button.SetText( "" );
			m_count--;
		}
	}
	
	//--------------------------------------------------------------------------
	int Count()
	{
		return m_commands.Count();
	}
	
	//--------------------------------------------------------------------------
	void Clear()
	{	
		int i;
		
		m_commands.Clear();
		
		if (!m_context_menu_panel_widget)
			return;
		Widget child = m_context_menu_panel_widget.GetChildren();
		while(child)
		{
			ButtonWidget button = ButtonWidget.Cast(child);
			if(button)
			{
				button.Show(false);
			}
			child = child.GetSibling();
			
			
		}
		m_count = 0;
		m_max_item_width = 0;
	}

	void BuildContextMenu(notnull EntityAI entity, notnull Widget rootWidget, Class target)
	{
		Clear();

		TSelectableActionInfoArrayEx customActions = new TSelectableActionInfoArrayEx();
		entity.GetDebugActions(customActions);
	
		int actionsCount = customActions.Count();
		for (int i = 0; i < customActions.Count(); i++)
		{
			TSelectableActionInfoWithColor actionInfo = TSelectableActionInfoWithColor.Cast(customActions.Get(i));
			if (actionInfo)
			{
				int actionId 		= actionInfo.param2;
				int textColor		= actionInfo.param4;
				string actionText 	= actionInfo.param3;
				
				if (actionId == EActions.SEPARATOR)
					AddEx(actionText, textColor, null, "", null);
				else
					AddEx(actionText, textColor, target, "OnSelectAction", new Param3<EntityAI, int, int>(entity, actionId, textColor));
			}
		}
	}
	
	//--------------------------------------------------------------------------
	static void DisplayContextMenu(int x, int y, notnull EntityAI entity, notnull Widget rootWidget, Class target)
	{
		m_ContextMenuInstance = new ContextMenu();
		if (m_ContextMenuInstance)
		{
			m_ContextMenuInstance.Init(rootWidget);
			m_ContextMenuInstance.BuildContextMenu(entity, rootWidget, target);

			m_ContextMenuInstance.SetSize(1,1);
			m_ContextMenuInstance.Show(x, y);
		}
	}
	
};

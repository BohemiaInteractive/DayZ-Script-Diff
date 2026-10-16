class ScriptConsoleMenuElement : ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ButtonWidget m_ContextButton;
	protected TextWidget m_ButtonText;
	protected GridSpacerWidget m_ContextSubGrid;

	protected int m_Id;	
	protected bool m_IsSelected;
	protected ref array<ref ScriptConsoleMenuElement> m_ContextSubElements = new array<ref ScriptConsoleMenuElement>();
	protected ScriptConsole m_ScriptConsole;
	
	void ScriptConsoleMenuElement(int id, Widget parent, string name, ScriptConsole scriptConsole)
	{
		m_Root = g_Game.GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console_menu_element.layout", parent);
		m_ContextButton = ButtonWidget.Cast(m_Root.FindAnyWidget("ContextButton"));
		m_ContextSubGrid = GridSpacerWidget.Cast(m_Root.FindAnyWidget("ContextSubGrid"));
		m_ButtonText = TextWidget.Cast(m_Root.FindAnyWidget("ButtonText"));
		
		m_ButtonText.SetText(name);
				
		m_Id = id;
		m_ScriptConsole = scriptConsole;
		m_Root.SetHandler(this);
	}
	
	Widget GetRoot()
	{
		return m_Root;
	}
	
	ButtonWidget GetButton()
	{
		return m_ContextButton;
	}
	
	GridSpacerWidget GetGrid()
	{
		return m_ContextSubGrid;
	}
	
	void HideGrid()
	{
		m_ContextSubGrid.Show(false);
	}
	
	void AddSubElement(ScriptConsoleMenuElement subElement)
	{
		m_ContextSubElements.Insert(subElement);
	}
	
	bool HasSubElements()
	{
		return m_ContextSubElements.Count();
	}
	
	bool IsSubElementButton(Widget w)
	{
		foreach(ScriptConsoleMenuElement subElement: m_ContextSubElements)
		{
			if (w == subElement.GetButton())
				return true;
		}
		
		return false;
	}
	
	void Select()
	{
		m_ContextButton.SetState(true);
		m_ContextButton.SetColor(ARGB(200, 255, 255, 255));
		m_ButtonText.SetColor(ARGB(255, 52, 152, 219));
		m_IsSelected = true;
	}
	
	void Deselect()
	{
		m_ContextButton.SetState(false);
		m_ContextButton.SetColor(ARGB(255, 100, 100, 100));
		m_ButtonText.SetColor(ARGB(255, 255, 255, 255));
		m_IsSelected = false;
	}
	
	void SetTextAlignment(EInputDeviceType pInputDeviceType)
	{
		if (pInputDeviceType == EInputDeviceType.CONTROLLER)
		{
			m_ButtonText.SetFlags(WidgetFlags.CENTER);
			m_ButtonText.SetFlags(WidgetFlags.VCENTER);
		}
		else
		{
			m_ButtonText.ClearFlags(WidgetFlags.CENTER);
			m_ButtonText.ClearFlags(WidgetFlags.VCENTER);
		}
	}
	
	protected bool IsChildOfWidget(Widget child, Widget parent)
	{
		if (!child || !parent)
			return false;
	
		Widget currentParent = child.GetParent();
		while (currentParent)
		{
			if (currentParent == parent)
				return true;
			currentParent = currentParent.GetParent();
		}
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		super.OnMouseLeave(w, enterW, x, y);
		
		if (w == m_ContextButton)
		{
			if (!m_IsSelected)
			{
				m_ButtonText.SetColor(ARGB(255, 255, 255, 255));
			}
			else
			{
				m_ButtonText.SetColor(ARGB(255, 52, 152, 219));
			}
		
			if (!IsSubElementButton(enterW) && enterW != m_ContextSubGrid && !IsChildOfWidget(enterW, m_ContextSubGrid))
			{
				m_ContextSubGrid.Show(false);
			}
			
			return true;
		}
		
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		super.OnMouseEnter(w, x, y);
		
		if (w == m_ContextButton)
		{
			if (!m_IsSelected)
				m_ButtonText.SetColor(ARGB(255, 0, 0, 0));
			
			if (m_ContextSubElements.Count())
				m_ContextSubGrid.Show(true);
		}
		
		return false;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w == m_ContextButton)
		{
			m_ScriptConsole.SelectTabByID(m_Id);
			return true;
		}
		
		return false;
	}
}
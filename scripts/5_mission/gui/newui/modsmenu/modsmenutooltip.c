class ModsMenuTooltip extends ScriptedWidgetEventHandler
{
	protected Widget			m_Root;
	protected RichTextWidget	m_Text;

	void ModsMenuTooltip(Widget parent)
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/mods_menu/mods_tooltip.layout", parent);
		m_Text = RichTextWidget.Cast(m_Root.FindAnyWidget("TooltipText"));
	}
	
	void ~ModsMenuTooltip()
	{
		delete m_Root;
	}
	
	void ShowTooltip(ModInfo mod_ref)
	{
		m_Root.Show(true);
		m_Text.SetText(mod_ref.GetTooltip());
		
		int x, y;
		GetMousePos(x,y);
		m_Root.SetPos(x, y);
		
		m_Text.Update();
		m_Root.Update();
	}

	void HideTooltip()
	{
		m_Root.Show(false);
		m_Root.SetPos(0, 0);
		m_Text.SetText("");
	}
}
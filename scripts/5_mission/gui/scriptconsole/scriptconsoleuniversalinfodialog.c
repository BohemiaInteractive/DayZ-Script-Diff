class ScriptConsoleUniversalInfoDialog extends UIScriptedMenu
{
	private const int EDITBOX_TEXT_SIZE = 16;
	
	private TextWidget m_Label;
	private ScrollWidget m_ContentScroll;
	private MultilineEditBoxWidget m_Content;
	private ButtonWidget m_OKButton;
	
	void ScriptConsoleUniversalInfoDialog();
	void ~ScriptConsoleUniversalInfoDialog();

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console_universal_info_dialog.layout");
		m_Label = TextWidget.Cast(layoutRoot.FindAnyWidget("Label"));
		m_ContentScroll = ScrollWidget.Cast(layoutRoot.FindAnyWidget("ContentScroll"));
		m_ContentScroll.VScrollToPos(0);
		m_Content = MultilineEditBoxWidget.Cast(layoutRoot.FindAnyWidget("Content"));
		m_OKButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("MessageText"));

		return layoutRoot;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w.GetUserID() == IDC_OK)
		{
			UIScriptedMenu consoleMenu = GetGame().GetUIManager().FindMenu(MENU_SCRIPTCONSOLE);
			if (consoleMenu != null)
			{
				ScriptConsole scriptedConsole = ScriptConsole.Cast(consoleMenu);
				ScriptConsoleItemsTab tab = ScriptConsoleItemsTab.Cast(scriptedConsole.GetTabHandler(ScriptConsoleItemsTab));
				if (tab)
				{
					Close();
					return true;
				}
			}
		}

		return false;
	}
	
	void SetLabel(string label)
	{
		m_Label.SetText(label);
	}
	
	void SetContent(string content)
	{
		m_Content.SetText(content);
		m_Content.Update();
		float contentSizeX, contentSizeY = 0;
		m_Content.GetSize(contentSizeX, contentSizeY);
		m_Content.SetSize(contentSizeX, EDITBOX_TEXT_SIZE * m_Content.GetLinesCount());
		float y_c = m_ContentScroll.GetContentHeight();
		float x, y;
		m_Content.GetScreenSize(x, y);
		if (y > y_c)
		{
			m_ContentScroll.SetAlpha(1);
		}
	}
}

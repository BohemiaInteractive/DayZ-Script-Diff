class ScriptConsoleRenamePresetDialog extends UIScriptedMenu
{
	void ScriptConsoleRenamePresetDialog()
	{

	}

	void ~ScriptConsoleRenamePresetDialog()
	{
	}

	override Widget Init()
	{
		layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console_dialog_preset.layout");
		m_Editbox = EditBoxWidget.Cast( layoutRoot.FindAnyWidget("PrimaryEditBox") );
		m_Label = TextWidget.Cast( layoutRoot.FindAnyWidget("WindowLabel") );
		m_Message = TextWidget.Cast( layoutRoot.FindAnyWidget("MessageText") );

		MissionBase mission = MissionBase.Cast( g_Game.GetMission() );

		UIScriptedMenu ui_menu = g_Game.GetUIManager().FindMenu(MENU_SCRIPTCONSOLE);
		if ( ui_menu != NULL )
		{
			ScriptConsole scripted_console = ScriptConsole.Cast( ui_menu );
			ScriptConsoleItemsTab tab = ScriptConsoleItemsTab.Cast(scripted_console.GetTabHandler(ScriptConsoleItemsTab));
			if (tab)
			{
				m_Editbox.SetText( tab.GetCurrentPresetName() );
				m_Label.SetText("RENAME PRESET");
				m_Message.SetText( tab.GetCurrentPresetName() );
			}
		}

		return layoutRoot;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);

		if (w.GetUserID() == IDC_OK)
		{
			UIScriptedMenu ui_menu = g_Game.GetUIManager().FindMenu(MENU_SCRIPTCONSOLE);
			if ( ui_menu != NULL )
			{
				ScriptConsole scripted_console = ScriptConsole.Cast( ui_menu );
				ScriptConsoleItemsTab tab = ScriptConsoleItemsTab.Cast(scripted_console.GetTabHandler(ScriptConsoleItemsTab));
				if (tab)
				{
					tab.RenamePreset( m_Editbox.GetText() );
					Close();
					return true;
				}
			}
		}
		else if (w.GetUserID() == IDC_CANCEL)
		{
			Close();
			return true;
		}

		return false;
	}
	
	EditBoxWidget m_Editbox;
	TextWidget m_Label;
	TextWidget m_Message;
}

class ScriptConsoleAddLocation extends UIScriptedMenu
{
	void ScriptConsoleAddLocation()
	{

	}

	void ~ScriptConsoleAddLocation()
	{
	}
	
	void SetPosition(vector pos)
	{
		m_EditboxPos.SetText(pos.ToString());
	}

	override Widget Init()
	{
		m_ConfigDebugProfile = PluginConfigDebugProfile.Cast( GetPlugin(PluginConfigDebugProfile) );
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console_add_location_dialogue.layout");
		m_EditboxName = EditBoxWidget.Cast( layoutRoot.FindAnyWidget("LocationName") );
		m_EditboxPos = EditBoxWidget.Cast( layoutRoot.FindAnyWidget("Position") );
		m_Label = TextWidget.Cast( layoutRoot.FindAnyWidget("WindowLabel") );
		m_ClearButton = ButtonWidget.Cast( layoutRoot.FindAnyWidget("ButtonClear") );
		m_Label.SetText("ADD NEW LOCATION");

		return layoutRoot;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);

		if (w.GetUserID() == IDC_OK)
		{
			string name = m_EditboxName.GetText();
			ScriptConsole console = ScriptConsole.Cast(GetGame().GetUIManager().FindMenu(MENU_SCRIPTCONSOLE));
			if (!console.IsLocationNameAvailable(name) || name == "" || m_EditboxPos.GetText() == "")
				return false;
			m_ConfigDebugProfile.CustomLocationsAdd(name, m_EditboxPos.GetText().BeautifiedToVector());
			Close();
			
			console.RefreshLocations();
			return true;
		}
		else if (w.GetUserID() == IDC_CANCEL)
		{
			Close();
			return true;
		}
		else if (w == m_ClearButton)
		{
			m_EditboxPos.SetText("");
			return true;
		}

		return false;
	}
	PluginConfigDebugProfile m_ConfigDebugProfile;
	EditBoxWidget m_EditboxName;
	EditBoxWidget m_EditboxPos;
	TextWidget m_Label;
	TextWidget m_Message;
	ButtonWidget m_ClearButton;
}

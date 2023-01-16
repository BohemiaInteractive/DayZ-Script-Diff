class ChatInputMenu extends UIScriptedMenu
{
	private EditBoxWidget m_edit_box;
	private TextWidget m_channel_text;
	private UAIDWrapper m_BackInputWrapper;
	private ref Timer m_close_timer;

	void ChatInputMenu()
	{
		m_close_timer = new Timer();
	}

	override Widget Init()
	{
		m_BackInputWrapper = GetUApi().GetInputByID(UAUIBack).GetPersistentWrapper();
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_chat_input.layout");
		m_edit_box = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("InputEditBoxWidget"));
		m_channel_text = TextWidget.Cast(layoutRoot.FindAnyWidget("ChannelText"));
		
		UpdateChannel();
		return layoutRoot;
	}
	
	override bool UseKeyboard() 
	{ 
		return true; 
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		
		if (!finished) return false;
		
		string text = m_edit_box.GetText();

		if (text != "")
		{
			GetGame().ChatPlayer(text);
			if (!GetGame().IsMultiplayer())
			{
				string name;
				GetGame().GetPlayerName(name);
				ChatMessageEventParams chat_params = new ChatMessageEventParams(CCDirect, name, text, "");
				MissionGameplay.Cast(GetGame().GetMission()).m_Chat.Add(chat_params);
			}
		}

		m_close_timer.Run(0.1, this, "Close");
		
		GetUApi().GetInputByID(UAPersonView).Supress();	
		
		return true;
	}
	
	override void OnShow()
	{
		SetFocus(m_edit_box);
	}
	
	override void OnHide()
	{
		super.OnHide();
		GetGame().GetMission().HideChat();
		
		if (!GetGame().GetMission().IsVoNActive())
			GetGame().GetMission().HideVoiceLevelWidgets();
	}
	
	override void Refresh()
	{	
	}
	
	override void Update(float timeslice)
	{
		if (m_BackInputWrapper.InputP().LocalPress())
		{
			Close();
		}
	}
	
	void UpdateChannel()
	{
		m_channel_text.SetText(GetChannelName(/*GetGame().ChatGetChannel()*/0));	
	}
	
	static string GetChannelName(ChatChannel channel)
	{
		switch (channel)
		{
			case CCSystem:
				return "System";
			case CCAdmin:
				return "Admin";
			case CCDirect:
				return "Direct";
			case CCMegaphone:
				return "Megaphone";   
			case CCTransmitter:
				return "Radio"; 
			case CCPublicAddressSystem:
				return "PAS";
		}	
		
		return "";
	}
}

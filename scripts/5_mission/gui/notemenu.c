class NoteMenu extends UIScriptedMenu
{
	protected MultilineEditBoxWidget m_edit;
	protected HtmlWidget m_html;
	protected ButtonWidget m_confirm_button;
	protected ItemBase m_Paper;
	protected EntityAI m_Pen;
	protected bool m_IsWriting;
	//protected int m_Handwriting;
	protected int m_SymbolCount;
	protected string m_PenColor; //color in hex-code format, transferred as string. Could be transferred as int or array<int, bool>?

	void NoteMenu()
	{
		MissionGameplay mission = MissionGameplay.Cast( GetGame().GetMission() );
		if( mission )
		{
			IngameHud hud = IngameHud.Cast( mission.GetHud() );
			if ( hud )
			{
				hud.ShowHudUI( false );
			}
		}
	}

	void ~NoteMenu()
	{
		MissionGameplay mission = MissionGameplay.Cast( GetGame().GetMission() );
		if( mission )
		{
			IngameHud hud = IngameHud.Cast( mission.GetHud() );
			if ( hud )
			{
				hud.ShowHudUI( true );
			}
		}
	}

	override void InitNoteRead(string text = "")
	{
		m_IsWriting = false;
		
		if (m_edit)
		{
			m_edit.Show(false);
		}
		
		if (m_html)
		{
			//TODO add text formating here. Just text string for now
			if (text)
			{
				m_html.Show(true);
				m_html.SetText(text);
				m_SymbolCount = text.Length(); //string.LengthUtf8() ?
				//m_html.SetText("<html><body><p>" + text + "</p></body></html>");
			}
		}
		m_confirm_button.Show(false);
	}

	override void InitNoteWrite(EntityAI paper, EntityAI pen, string text = "")
	{
		m_IsWriting = true;
		
		if (m_edit)
		{
			m_Paper = ItemBase.Cast(paper);
			m_Pen = pen;
			m_PenColor = m_Pen.ConfigGetString("writingColor");
			if (m_PenColor == "")
			{
				m_PenColor = "#000000";
			}
			//m_Handwriting = handwriting;
			
			m_edit.Show(true);
			m_edit.SetText(text);
		}
		
		if (m_html)
		{
			m_html.Show(false);
		}
		m_confirm_button.Show(true);
	}

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_inventory_note.layout");
		m_edit = MultilineEditBoxWidget.Cast( layoutRoot.FindAnyWidget("EditWidget") );
		m_html = HtmlWidget.Cast( layoutRoot.FindAnyWidget("HtmlWidget") );
		m_confirm_button = ButtonWidget.Cast( layoutRoot.FindAnyWidgetById(IDC_OK) );
		
		return layoutRoot;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
	/*	string txt;
		m_edit.GetText(txt);
		Print(m_edit.GetLinesCount());
		Print(txt);
		Print(txt.Length());*/
		
		switch (w.GetUserID())
		{
			case IDC_CANCEL:
				Close();
				return true;
			case IDC_OK:
				if (m_Paper && m_Pen && m_IsWriting)
				{
					string edit_text;
					m_edit.GetText(edit_text);
					edit_text = MiscGameplayFunctions.SanitizeString(edit_text);
				//	Print("edit_text: " + edit_text);
					Param1<string> text = new Param1<string>(edit_text);
					m_Paper.RPCSingleParam(ERPCs.RPC_WRITE_NOTE_CLIENT, text, true);
				}
				Close();
			return true;
		}

		return false;
	}
	
	override void Update( float timeslice )
	{
		super.Update( timeslice );
		
		if( GetGame() && GetGame().GetInput() && GetGame().GetInput().LocalPress("UAUIBack", false) )
		{
			Close();
		}
	}
}

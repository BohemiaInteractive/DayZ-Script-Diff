class WrittenNoteData
{
	protected ItemBase 	m_WritingImplement;
	protected ItemBase	m_Paper;
	//protected int 		m_Handwriting = -1;
	protected string 	m_SimpleText;
	
	void WrittenNoteData(ItemBase parent)
	{
		
	}
	
	void OnRPC( PlayerIdentity sender, int rpc_type, ParamsReadContext  ctx)
	{
		Param1<string> param = new Param1<string>("");
		
		//sent from server, executed on client
		if (rpc_type == ERPCs.RPC_WRITE_NOTE)
		{
			if (ctx.Read(param))
			{
				SetNoteText(param.param1);
			}
			
			g_Game.GetMission().SetNoteMenu(g_Game.GetUIManager().EnterScriptedMenu(MENU_NOTE, GetGame().GetUIManager().GetMenu())); //NULL means no parent
			
			ItemBase pen;
			ItemBase paper;
			//int handwriting;
			
			if (GetNoteInfo(pen,paper))
			{
				g_Game.GetMission().GetNoteMenu().InitNoteWrite(paper,pen,m_SimpleText);
			}
		}
		//sent from client (NoteMenu), executed on server
		if (rpc_type == ERPCs.RPC_WRITE_NOTE_CLIENT)
		{
			if (ctx.Read(param))
			{
				string old_string = m_SimpleText;
				SetNoteText(param.param1);
				DepleteWritingImplement(m_WritingImplement,old_string,m_SimpleText);
			}
		}
		if (rpc_type == ERPCs.RPC_READ_NOTE)
		{
			if (ctx.Read(param))
			{
				SetNoteText(param.param1);
			}
			
			g_Game.GetMission().SetNoteMenu(g_Game.GetUIManager().EnterScriptedMenu(MENU_NOTE, GetGame().GetUIManager().GetMenu())); //NULL means no parent
			g_Game.GetMission().GetNoteMenu().InitNoteRead(m_SimpleText);
		}
	}
	
	void InitNoteInfo(ItemBase pen = null, ItemBase paper = null)
	{
		m_WritingImplement = pen;
		m_Paper = paper;
		//m_Handwriting = handwriting;
	}
	
	bool GetNoteInfo(out ItemBase pen, out ItemBase paper)
	{
		pen = m_WritingImplement;
		paper = m_Paper;
		//handwriting = m_Handwriting;
		return pen && paper;
	}
	
	string GetNoteText()
	{
		return m_SimpleText;
	}
	
	void SetNoteText(string text)
	{
		m_SimpleText = MiscGameplayFunctions.SanitizeString(text);
	}
	
	void DepleteWritingImplement(notnull ItemBase pen,string old_text,string new_text)
	{
		float qty_per_char = 1.0;
		float decrease = Math.Clamp((new_text.Length() - old_text.Length()),0,pen.GetQuantityMax());
		pen.AddQuantity(-(qty_per_char * decrease));
	}
}

//-----------------------
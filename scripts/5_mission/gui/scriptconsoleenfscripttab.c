class ScriptConsoleEnfScriptTab : ScriptConsoleTabBase
{
	protected static ScriptConsoleEnfScriptTab 	m_Instance;
	protected int 								m_EnscriptHistoryRow;
	protected int 								m_EnscriptHistoryRowServer;
	protected ref TStringArray					m_EnscriptConsoleHistory;
	protected ref TStringArray					m_EnscriptConsoleHistoryServer;
	protected PluginLocalEnscriptHistory		m_ModuleLocalEnscriptHistory;
	protected PluginLocalEnscriptHistoryServer	m_ModuleLocalEnscriptHistoryServer;
	protected MultilineEditBoxWidget 			m_EnfScriptEdit;
	protected ButtonWidget 						m_EnfScriptRun;
	protected ButtonWidget 						m_EnfScriptClear;
	protected TextListboxWidget 				m_ScriptOutputListbox;
	protected bool 								m_AllowScriptOutput;
	protected int 								m_RunColor;
	
	static ref TStringArray 					m_ScriptOutputHistory = new TStringArray();
	
	void ScriptConsoleEnfScriptTab(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
	{
		m_Instance 							= this;
		m_ModuleLocalEnscriptHistory 		= PluginLocalEnscriptHistory.Cast(GetPlugin(PluginLocalEnscriptHistory));
		m_ModuleLocalEnscriptHistoryServer 	= PluginLocalEnscriptHistoryServer.Cast(GetPlugin(PluginLocalEnscriptHistoryServer));
		m_EnscriptConsoleHistory 			= m_ModuleLocalEnscriptHistory.GetAllLines();
		m_EnscriptConsoleHistoryServer 		= m_ModuleLocalEnscriptHistoryServer.GetAllLines();
		m_EnfScriptEdit						= MultilineEditBoxWidget.Cast(root.FindAnyWidget("MultilineEditBoxWidget0"));
		m_EnfScriptRun						= ButtonWidget.Cast(root.FindAnyWidget("RunButton"));
		m_EnfScriptClear					= ButtonWidget.Cast(root.FindAnyWidget("ClearButton"));
		m_ScriptOutputListbox 				= TextListboxWidget.Cast(root.FindAnyWidget("ScriptOutputListbox"));
		m_RunColor 							= m_EnfScriptRun.GetColor();
	}

	void ~ScriptConsoleEnfScriptTab()
	{
		m_Instance = null;
	}
	
	override void OnSelected()
	{
		int index = m_EnscriptConsoleHistory.Count() - m_EnscriptHistoryRow - 1;
		if (m_EnscriptConsoleHistory.IsValidIndex(index))
		{
			string text = m_EnscriptConsoleHistory.Get(index);
			m_EnfScriptEdit.SetText(text);
		}
		ReloadScriptOutput();
	}

	static void PrintS(string message)
	{
		Print(message);
		if (m_Instance)
			m_Instance.Add(message);
	}
	
	static void PrintS(bool message)
	{
		PrintS(message.ToString());
	}
	
	static void PrintS(int message)
	{
		PrintS(message.ToString());
	}
	
	static void PrintS(float message)
	{
		PrintS(message.ToString());
	}
	
	static void PrintS(vector message)
	{
		PrintS(message.ToString());
	}
	
	static void PrintS(Object message)
	{
		PrintS(message.ToString());
	}

	
	void Add(string message, bool isReload = false)
	{
		if (message != string.Empty)
		{
			if (m_AllowScriptOutput && m_ScriptOutputListbox)
			{
				if (!isReload)
					m_ScriptOutputHistory.Insert(message);
				m_ScriptOutputListbox.AddItem(String(message), NULL, 0);
				m_ScriptOutputListbox.EnsureVisible(m_ScriptOutputListbox.GetNumItems());
			}
		}
	}
	
	protected void Clear(bool clearFile = false)
	{
		if(clearFile)
			Debug.ClearLogs();
		m_ScriptOutputListbox.ClearItems();
	}
	
	
	
	protected void ReloadScriptOutput()
	{
		m_ScriptOutputListbox.ClearItems();
		m_AllowScriptOutput = true;
		foreach ( string s: m_ScriptOutputHistory)
		{
			Add(s, true);
		}
		m_AllowScriptOutput = false;
	}
	
	void HistoryBack()
	{

		EnscriptHistoryBack();
	}

	void HistoryForward()
	{
		EnscriptHistoryForward();
	}
	
	protected void RunEnscript()
	{
		#ifdef DEVELOPER
		string code;
		m_EnfScriptEdit.GetText(code);
		string codeNoReplace = code;
		_player = PlayerBase.Cast(g_Game.GetPlayer());
		m_AllowScriptOutput = true;
		code.Replace("Print(","ScriptConsoleEnfScriptTab.PrintS(");
		code.Replace("Print (","ScriptConsoleEnfScriptTab.PrintS(");
		bool success = g_Game.ExecuteEnforceScript("void scConsMain() \n{\n" + code + "\n}\n", "scConsMain");
		m_AllowScriptOutput = false;
		ColorRunButton(success);
		
		m_EnscriptConsoleHistory.Insert(codeNoReplace);
		m_ModuleLocalEnscriptHistory.AddNewLine(codeNoReplace);
		#endif
	}
	
	protected void ColorRunButton(bool success)
	{
		if (success)
		{
			m_EnfScriptRun.SetColor(ARGB(255,0,255,0));
		}
		else
		{
			m_EnfScriptRun.SetColor(ARGB(255,255,0,0));
		}
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ResetRunButtonColor,600);
	}
	
	protected void ResetRunButtonColor()
	{
		m_EnfScriptRun.SetColor(m_RunColor);
	}
	
	protected void RunEnscriptServer()
	{
		string code;
		m_EnfScriptEdit.GetText(code);
		m_EnscriptConsoleHistoryServer.Insert(code);
		m_ModuleLocalEnscriptHistoryServer.AddNewLine(code);
		CachedObjectsParams.PARAM1_STRING.param1 = code;
		g_Game.RPCSingleParam(g_Game.GetPlayer(), ERPCs.DEV_RPC_SERVER_SCRIPT, CachedObjectsParams.PARAM1_STRING, true, g_Game.GetPlayer().GetIdentity());
	}
	
	protected void EnscriptHistoryBack()
	{
		int history_index;
		if (m_EnfScriptEdit)
		{
			m_EnscriptHistoryRow++;
			history_index = m_EnscriptConsoleHistory.Count() - m_EnscriptHistoryRow - 1;
			if (history_index > -1)
			{
				m_EnfScriptEdit.SetText(m_EnscriptConsoleHistory.Get(history_index));
			}
			else m_EnscriptHistoryRow--;
		}
	}

	
	protected void EnscriptHistoryForward()
	{
		if (m_EnfScriptEdit)
		{
			int history_index;
			m_EnscriptHistoryRow--;
			history_index = m_EnscriptConsoleHistory.Count() - m_EnscriptHistoryRow - 1;
			if (history_index < m_EnscriptConsoleHistory.Count())
			{
				m_EnfScriptEdit.SetText(m_EnscriptConsoleHistory.Get(history_index));
			}
			else m_EnscriptHistoryRow++;
		}
	}
	
	override void OnRPCEx(int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPCEx(rpc_type, ctx);
		#ifdef DIAG_DEVELOPER
		switch (rpc_type)
		{
			case ERPCs.DEV_RPC_SERVER_SCRIPT_RESULT:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
				{
					ColorRunButton(CachedObjectsParams.PARAM1_BOOL.param1);
				}
				break;
			}
		}
		#endif
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w,x,y,button);
		if (w == m_EnfScriptRun)
		{
			RunEnscript();
			return true;
		}
		else if (w == m_EnfScriptClear)
		{
			m_ScriptOutputListbox.ClearItems();
			m_ScriptOutputHistory.Clear();
			return true;
		}
		
		return false;
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		return false;
	}

	
	override void Show(bool show, ScriptConsoleTabBase selectedHandler)
	{
		if (!show && (selectedHandler.Type() == ScriptConsoleEnfScriptTab || selectedHandler.Type() == ScriptConsoleEnfScriptServerTab))
		{
			//do nothing
		}
		else
		{
			m_Root.Show(show);
			m_Root.Enable(show);
		}
	}
		
}

class ScriptConsoleEnfScriptServerTab : ScriptConsoleEnfScriptTab
{
	override void OnSelected()
	{
		int index = m_EnscriptConsoleHistoryServer.Count() - m_EnscriptHistoryRowServer - 1;
		if (m_EnscriptConsoleHistoryServer.IsValidIndex(index))
		{
			string text = m_EnscriptConsoleHistoryServer.Get(index);
			m_EnfScriptEdit.SetText(text);
		}
		ReloadScriptOutput();
	}

	override protected void EnscriptHistoryBack()
	{
		int history_index;
		if (m_EnfScriptEdit)
		{
			m_EnscriptHistoryRowServer++;
			history_index = m_EnscriptConsoleHistoryServer.Count() - m_EnscriptHistoryRowServer - 1;
			if (history_index > -1)
			{
				m_EnfScriptEdit.SetText(m_EnscriptConsoleHistoryServer.Get(history_index));
			}
			else m_EnscriptHistoryRowServer--;
		}
	}

	override protected void EnscriptHistoryForward()
	{
		if (m_EnfScriptEdit)
		{
			int history_index;

			m_EnscriptHistoryRowServer--;
			history_index = m_EnscriptConsoleHistoryServer.Count() - m_EnscriptHistoryRowServer - 1;
			if (history_index < m_EnscriptConsoleHistoryServer.Count())
			{
				m_EnfScriptEdit.SetText(m_EnscriptConsoleHistoryServer.Get(history_index));
			}
			else m_EnscriptHistoryRowServer++;
		}
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_EnfScriptRun)
		{
			RunEnscriptServer();
			return true;
		}
		else if (w == m_EnfScriptClear)
		{
			m_ScriptOutputListbox.ClearItems();
			m_ScriptOutputHistory.Clear();
			return true;
		}
		
		return false;
	}
	
}

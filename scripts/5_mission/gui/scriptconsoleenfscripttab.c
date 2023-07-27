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
	protected bool 								m_ScriptServer;
	protected bool 								m_AllowScriptOutput;
	protected int 								m_RunColor;
	
	static ref TStringArray 					m_ScriptOutputHistory = new TStringArray();
	
	void ScriptConsoleEnfScriptTab(Widget root, ScriptConsole console, bool server)
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
		
		int index;
		string text;
		
		if (!server)
		{
			//CLIENT
			m_ScriptServer = false;
			index = m_EnscriptConsoleHistory.Count() - m_EnscriptHistoryRow - 1;
			if (m_EnscriptConsoleHistory.IsValidIndex(index))
			{
				text = m_EnscriptConsoleHistory.Get(index);
				//Print("setting text client " + text);
				m_EnfScriptEdit.SetText(text);
			}
			ReloadScriptOutput();
		}
		else
		{
			//SERVER
			m_ScriptServer = true;
			index = m_EnscriptConsoleHistoryServer.Count() - m_EnscriptHistoryRowServer - 1;
			if (m_EnscriptConsoleHistoryServer.IsValidIndex(index))
			{
				text = m_EnscriptConsoleHistoryServer.Get(index);
				//Print("setting text server " + text);
				m_EnfScriptEdit.SetText(text);
			}
			ReloadScriptOutput();
		}
	}
	
	void ~ScriptConsoleEnfScriptTab()
	{
		m_Instance = null;
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
		if (m_ConfigDebugProfile.GetTabSelected() == ScriptConsole.TAB_ENSCRIPT || m_ConfigDebugProfile.GetTabSelected() == ScriptConsole.TAB_ENSCRIPT_SERVER)
		{
			EnscriptHistoryBack();
		}
	}

	void HistoryForward()
	{
		if (m_ConfigDebugProfile.GetTabSelected() == ScriptConsole.TAB_ENSCRIPT || m_ConfigDebugProfile.GetTabSelected() == ScriptConsole.TAB_ENSCRIPT_SERVER)
		{
			EnscriptHistoryForward();
		}
	}
	
	protected void RunEnscript()
	{
		#ifdef DEVELOPER
		string code;
		m_EnfScriptEdit.GetText(code);
		string codeNoReplace = code;
		_player = PlayerBase.Cast(GetGame().GetPlayer());
		m_AllowScriptOutput = true;
		code.Replace("Print(","ScriptConsoleEnfScriptTab.PrintS(");
		code.Replace("Print (","ScriptConsoleEnfScriptTab.PrintS(");
		bool success = GetGame().ExecuteEnforceScript("void scConsMain() \n{\n" + code + "\n}\n", "scConsMain");
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
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ResetRunButtonColor,600);
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
		GetGame().RPCSingleParam(GetGame().GetPlayer(), ERPCs.DEV_RPC_SERVER_SCRIPT, CachedObjectsParams.PARAM1_STRING, true, GetGame().GetPlayer().GetIdentity());
	}
	
	protected void EnscriptHistoryBack()
	{
		int history_index;
		
		if (m_EnfScriptEdit)
		{
			if (!m_ScriptServer)//client
			{
				m_EnscriptHistoryRow++;
				history_index = m_EnscriptConsoleHistory.Count() - m_EnscriptHistoryRow - 1;
				if (history_index > -1)
				{
					m_EnfScriptEdit.SetText(m_EnscriptConsoleHistory.Get(history_index));
				}
				else m_EnscriptHistoryRow--;
			}
			else//server
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
	}

	
	protected void EnscriptHistoryForward()
	{
		if (m_EnfScriptEdit)
		{
			int history_index;
			if (!m_ScriptServer)//client
			{
				m_EnscriptHistoryRow--;
				history_index = m_EnscriptConsoleHistory.Count() - m_EnscriptHistoryRow - 1;
				if (history_index < m_EnscriptConsoleHistory.Count())
				{
					m_EnfScriptEdit.SetText(m_EnscriptConsoleHistory.Get(history_index));
				}
				else m_EnscriptHistoryRow++;
				
			}
			else//server
			{
				m_EnscriptHistoryRowServer--;
				history_index = m_EnscriptConsoleHistoryServer.Count() - m_EnscriptHistoryRowServer - 1;
				if (history_index < m_EnscriptConsoleHistoryServer.Count())
				{
					m_EnfScriptEdit.SetText(m_EnscriptConsoleHistoryServer.Get(history_index));
				}
				else m_EnscriptHistoryRowServer++;
			}
			
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
			if (m_ScriptServer)
				RunEnscriptServer();
			else
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

		
}

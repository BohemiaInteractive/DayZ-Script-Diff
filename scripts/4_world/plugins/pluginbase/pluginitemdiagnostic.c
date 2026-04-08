class PluginItemDiagnosticEventHandler extends ScriptedWidgetEventHandler
{
	PluginItemDiagnostic m_Owner;
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if( ButtonWidget.Cast(w))
		{
			g_Game.GetMission().AddActiveInputExcludes({"menu"});
		}
		return true;

	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if( ButtonWidget.Cast(w))
		{
			g_Game.GetMission().RemoveActiveInputExcludes({"menu"},true);
		}
		
		return true;
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		return m_Owner.OnClick( w, x, y, button );
	}
	
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		return m_Owner.OnMouseButtonDown( w, x, y, button );
	}

	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		return m_Owner.OnMouseButtonUp( w, x, y, button );
	}
}



class PluginItemDiagnostic extends PluginDeveloper
{
	Object m_Entity;

	ref Timer myTimer1;
	ref map<PlayerBase,Object> m_ObserversMap = new map<PlayerBase,Object>;
	ref array<string> m_Agents = new array<string>;
	ref map<string,float> m_Floats = new map<string,float>;
	ref map<string,float> m_VarsNumbersFinalServer = new map<string,float>;
	ref map<string,float> m_VarsFinalClient = new map<string,float>;
	ref array<ref Param> m_Properties = new array<ref Param>;
	bool m_IsActive	= false;
	bool m_ScriptMenuOpened;
	string m_NoteClient;
	ref PluginItemDiagnosticEventHandler m_EventHandler;
	bool m_IsDragging;
	PluginConfigDebugProfile m_ConfigDebugProfile;
	
	Widget m_DebugRootWidget;	
	TextListboxWidget m_DebugAgentListWidget;
	TextWidget m_DebugOutputServer;
	TextWidget m_DebugOutputClient;
	TextListboxWidget m_DebugFloatsProperListWidget;
	TextListboxWidget m_DebugClientVarsWidget;
	TextWidget m_ClassNameWidget;
	TextWidget m_DistanceWidget;
	ItemPreviewWidget m_ItemPreviewWidget;	
	ButtonWidget m_CloseButton;

	Shape m_ItemLine;
	vector m_DraggingOffset;

	void PluginItemDiagnostic()
	{
		#ifndef NO_GUI
		InitializeWidgets();
		ShowWidgets(false);
		#endif
	}

	void ~PluginItemDiagnostic()
	{
		ClearProperties();
		if (m_ItemLine)
		{
			m_ItemLine.Destroy();
			m_ItemLine = null;
		}
	}
	
	override void OnInit()
	{
		m_ConfigDebugProfile = PluginConfigDebugProfile.Cast( GetPlugin(PluginConfigDebugProfile) );
		if ( m_ConfigDebugProfile )
		{
			vector pos = m_ConfigDebugProfile.GetItemDebugPos();
			if (m_DebugRootWidget && pos != vector.Zero)
			{
				float wx = pos[0];
				float wy = pos[1];
				m_DebugRootWidget.SetPos(wx, wy);
			}
		}
	}

	bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (button == 0)
			SetDragging(true);
		if (button == 1)
		{
			if (m_Entity)
			{
				float xx, yy;
				m_DebugRootWidget.GetPos(xx,yy);
				ContextMenu.DisplayContextMenu(x - xx, y - yy, EntityAI.Cast(m_Entity), m_DebugRootWidget, this);
			}
		}
		return true;
	}
	
	bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if (button == 0)
			SetDragging(false);
		return true;
	}
	
	void SetDragging(bool enable)
	{
		if(enable && !m_IsDragging)
			OnDraggingStart();
		else if (!enable && m_IsDragging)
			OnDraggingEnd();
		m_IsDragging = enable;
	}
	
	void OnDraggingStart()
	{
		int mx, my;
		float wx, wy;
		GetMousePos(mx,my);
		m_DebugRootWidget.GetScreenPos(wx,wy);
		m_DraggingOffset[0] = wx - mx;
		m_DraggingOffset[1] = wy - my;
	}
	
	void OnDraggingEnd()
	{
		float wx, wy;
		m_DebugRootWidget.GetScreenPos(wx,wy);
		if (m_ConfigDebugProfile)
		{
			m_ConfigDebugProfile.SetItemDebugPos(Vector(wx,wy,0));
		}
	}
	
	bool OnClick( Widget w, int x, int y, int button )
	{
		SetDragging(false);
		
		if (w == m_CloseButton)
		{
			ToggleDebugWindowEvent();
			return true;
		}
		return true;
	}
	
	void RegisterDebugItem(Object item, PlayerBase player)
	{
		if(!myTimer1)
		{
			myTimer1 = new Timer();
			myTimer1.Run(1, this, "Tick", NULL, true);
		}
		
		if(m_ObserversMap.Contains(player))
		{
			m_ObserversMap.Set(player,item);
		}
		else
		{
			m_ObserversMap.Insert(player,item);
		}
		#ifdef DEVELOPER
		SetDebugDeveloper_item(item);
		#endif
	}
	
	Object GetWatchedItem(PlayerBase player)
	{
		return m_ObserversMap.Get(player);
	}
	
	bool IsActive()
	{
		return m_IsActive;
	}

	void OnScriptMenuOpened(bool opened/*1 - opened , 0 - closed*/)
	{
		m_ScriptMenuOpened = opened;
	}

	void ShowWidgets(bool show)
	{
		m_IsActive = show;
			
		if(m_DebugRootWidget) 
			m_DebugRootWidget.Show(show);
	}

	void OnSelectAction(EntityAI ent, int actionId)
	{
		#ifdef DIAG_DEVELOPER 
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		player.GetActionManager().OnInstantAction(ActionDebug,new Param2<EntityAI,int>(ent,actionId));
		#endif
	}

	void ReleaseFocus()
	{
		g_Game.GetInput().ResetGameFocus();
		g_Game.GetUIManager().ShowUICursor(false);
		if (g_Game.GetUIManager()) 	
		{
			if (g_Game.GetUIManager().IsDialogVisible())
			{
				g_Game.GetUIManager().CloseDialog();
			}
		}
	}
	
	void ToggleDebugWindowEvent()
	{
		if (m_IsActive)
		{
			PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
			g_Game.RPCSingleParam(player, ERPCs.RPC_ITEM_DIAG_CLOSE,null, true);
			ShowWidgets(false);
			ClearWidgets();
			
			g_Game.GetCallQueue( CALL_CATEGORY_GUI ).CallLater( ReleaseFocus, 100);
			//m_IsActive = false;
			SetDragging(false);
		}
		else
		{
			ShowWidgets(true);
			//m_IsActive = true;
		}
	}

	void ClearWidgets()
	{
		m_DebugAgentListWidget.ClearItems();
		m_DebugOutputServer.SetText("");
		m_DebugOutputClient.SetText("");
		m_DebugFloatsProperListWidget.ClearItems();
		m_ItemPreviewWidget.SetItem(NULL);
		m_ClassNameWidget.SetText("");		
	}
	
	void Tick()
	{
		for(int i = 0; i < m_ObserversMap.Count();i++)
		{
			Object item = m_ObserversMap.GetElement(i);
			PlayerBase player = m_ObserversMap.GetKey(i);

			if (item && player) 
			{
				GeneratePropertiesObject(EntityAI.Cast(item));
				SendRPC(item, player);
			}
			else
			{
				Debug.LogError("PluginItemDiagnostic: dead debug record, removing record");
				m_ObserversMap.RemoveElement(i);
			}
		}
		
	}
	
	void ClearProperties()
	{
		m_Properties.Clear();
	}

	void GeneratePropertiesObject(EntityAI item)
	{
		ClearProperties();
		GetLocalProperties(item, m_Properties);
	}
	
	void SendRPC(Object item, PlayerBase player)
	{
		Param1<Object> p1 = new Param1<Object>(item);
		m_Properties.InsertAt(p1,0);
		g_Game.RPC(player,ERPCs.RPC_ITEM_DIAG,m_Properties,true,player.GetIdentity());
		
		if (!g_Game.IsMultiplayer())
		{
			m_Entity = item;
		}
	}
	
	void StopWatchRequest(PlayerBase player)//called from player after an RPC call
	{
		if( m_ObserversMap.Contains(player) )
		{
			m_ObserversMap.Remove(player);
		}
	}

	void OnRPC(Object entity, ParamsReadContext ctx)
	{
		
		if (!g_Game.IsMultiplayer())
		{
			entity = m_Entity;
		}
		else
		{
			m_Entity = entity;
		}
		
		ItemBase item = ItemBase.Cast(entity);		
		#ifdef DEVELOPER
		SetDebugDeveloper_item(entity);
		#endif
		if(!IsActive())
		{
			ShowWidgets(true);
		}

		ctx.Read(CachedObjectsParams.PARAM1_STRING);
		string debug_output_server = CachedObjectsParams.PARAM1_STRING.param1;
		array<ref Param> vars_client;
		array<ref Param> vars_server;
		if (item)
		{
			ctx.Read(CachedObjectsParams.PARAM1_INT);//hardcoded to index 1
			int agents_count 	= CachedObjectsParams.PARAM1_INT.param1;

			ctx.Read(CachedObjectsParams.PARAM1_INT);//hardcoded to index 2
			int individual_items_count 	= CachedObjectsParams.PARAM1_INT.param1;
			
			FillAgentArray(ctx, agents_count);
			
			vars_server = new array<ref Param>;
			FillServerFinalVars(individual_items_count, ctx, vars_server);

			vars_client = new array<ref Param>;
			GetLocalProperties(item, vars_client, true);
		}

		if (EntityAI.Cast(entity))
		{
			DisplayAll(EntityAI.Cast(entity), vars_server, vars_client, debug_output_server );
		}

		if (GetDayZGame().IsInventoryOpen() || g_Game.GetUIManager().FindMenu(MENU_SCRIPTCONSOLE))
		{
			m_DebugRootWidget.SetSort(-1);
		}
		else
		{
			m_DebugRootWidget.SetSort(10);
		}
		m_DebugRootWidget.Show(true);			
	}
	
	void FillServerFinalVars(int count, ParamsReadContext ctx, array<ref Param> params )
	{
		for(int i = 0; i < count; i++)
		{
			Param2<string,float> p2 = new Param2<string,float>("",0);
			
			ctx.Read(p2);
			/*
			string name = CachedObjectsParams.PARAM2_STRING_FLOAT.param1;
			float value = CachedObjectsParams.PARAM2_STRING_FLOAT.param2;
			PrintString(name+","+value.ToString());
			*/
			params.Insert(p2);
		}
		//PrintString("----------------------------------------------------");
	}
	
	void InitializeWidgets()
	{
		m_EventHandler = new PluginItemDiagnosticEventHandler;
		m_EventHandler.m_Owner = this;
		
		if(!m_DebugRootWidget) 
			m_DebugRootWidget = g_Game.GetWorkspace().CreateWidgets("gui/layouts/debug/debug_item.layout");

		m_DebugRootWidget.SetHandler(m_EventHandler);

		m_DebugAgentListWidget = TextListboxWidget.Cast(m_DebugRootWidget.FindAnyWidget("AgentsList"));
		m_DebugOutputServer = TextWidget.Cast(m_DebugRootWidget.FindAnyWidget("DebugOutputServer"));
		m_DebugOutputClient = TextWidget.Cast(m_DebugRootWidget.FindAnyWidget("DebugOutputClient"));
		m_DebugFloatsProperListWidget = TextListboxWidget.Cast(m_DebugRootWidget.FindAnyWidget("FloatsProperList"));
		m_ItemPreviewWidget = ItemPreviewWidget.Cast(m_DebugRootWidget.FindAnyWidget("ItemPreview"));
		m_ClassNameWidget = TextWidget.Cast(m_DebugRootWidget.FindAnyWidget("ClassName"));
		m_DistanceWidget = TextWidget.Cast(m_DebugRootWidget.FindAnyWidget("Distance"));		
		m_CloseButton = ButtonWidget.Cast(m_DebugRootWidget.FindAnyWidget("CloseButton"));
	}

	override void OnUpdate(float delta_time)
	{
		super.OnUpdate(delta_time);
		
		if (!m_Entity || !m_IsActive)
		{
			if (m_ItemLine)
			{
				m_ItemLine.Destroy();
				m_ItemLine = null;
			}
			return;
		}
		
		m_ItemPreviewWidget.Show(!m_ScriptMenuOpened);
		
		if (m_IsDragging)
		{
			int x,y;
			GetMousePos(x,y);
			m_DebugRootWidget.SetPos(x + m_DraggingOffset[0], y + m_DraggingOffset[1]);
		}

		vector pts[2];
		pts[0] = g_Game.GetPlayer().GetPosition();
		pts[1] = m_Entity.GetPosition();
		if (m_ItemLine)
		{
			m_ItemLine.Destroy();
			m_ItemLine = null;
			
		}

		m_ItemLine = Shape.CreateLines(COLOR_BLUE, ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE|ShapeFlags.NOZBUFFER, pts, 2);
		m_DistanceWidget.SetText(vector.Distance(pts[0], pts[1]).ToString()+"m.");
	}
	
	void DisplayAll(EntityAI item, array<ref Param> vars_server, array<ref Param> vars_client, string debug_text_server)
	{
		m_ClassNameWidget.SetText( item.GetType() );
		m_ItemPreviewWidget.SetItem(item);
		UpdateAgentWidget();
		//UpdateFloatWidget();
		if (vars_server && vars_client)
			UpdateNumericalVarsWidget(vars_server, vars_client);
		m_DebugOutputServer.SetText(debug_text_server);
		m_DebugOutputClient.SetText(item.GetDebugText());
	}	

	void FillAgentArray(ParamsReadContext ctx, int agents_count)
	{
		m_Agents.Clear();
		for(int i = 0; i < agents_count; i++)
		{
			if(ctx.Read(CachedObjectsParams.PARAM1_STRING))
			{
				string agent = CachedObjectsParams.PARAM1_STRING.param1;
				m_Agents.Insert(agent);
			}
		}
	}

	void UpdateAgentWidget()
	{
		m_DebugAgentListWidget.ClearItems();
		for(int i = 0; i < m_Agents.Count(); i++)
		{
			string agent = m_Agents.Get(i);
			m_DebugAgentListWidget.AddItem(agent,NULL,0);
		}
	}
	
	void UpdateNumericalVarsWidget(array<ref Param> vars_server, array<ref Param> vars_client)
	{
		m_DebugFloatsProperListWidget.ClearItems();
		for(int i = 0; i < vars_server.Count(); i++)
		{
			Param param_server = vars_server.Get(i);
			Param2<string,float> p2_server = Param2<string,float>.Cast(param_server);
			
			Param param_client = vars_client.Get(i);
			Param2<string,float> p2_client = Param2<string,float>.Cast(param_client);
			
			string name = p2_server.param1;
			m_DebugFloatsProperListWidget.AddItem(name,NULL,0);
			m_DebugFloatsProperListWidget.SetItem(i,p2_server.param2.ToString(),NULL,1);
			m_DebugFloatsProperListWidget.SetItem(i,p2_client.param2.ToString(),NULL,2);
		}
	}
	
	void PrintOut()
	{
		Debug.LogArrayString(m_Agents,"ItemDebug");
		Debug.Log("----------------------","ItemDebug");
	}
	
	//-------------------------
	// QUERY FOR DEBUG PROPERTIES
	//-------------------------
	void GetLocalProperties(EntityAI entity, array<ref Param> properties, bool client = false)
	{
		if(!entity)
			return;
		ItemBase item = ItemBase.Cast(entity);

		if(!client)
		{
			// -------- DEBUG OUTPUT ------
			Param1<string> debug_output = new Param1<string>(entity.GetDebugText());
			properties.Insert(debug_output);
			if(!item)
				return;
			// -------- AGENTS ------------
			int num_of_agents = FillWithAgents(item, properties);
			Param1<int> agents_count = new Param1<int>(num_of_agents);
			properties.InsertAt(agents_count,1);// hard coded index 1 !!
		}
		if(!item)
			return;
		// ------ INDIVIDUAL VARS------
		int number_of_items = 0;
		Param2<string, float> quantity = new Param2<string, float>("quantity", item.GetQuantity() );
		properties.Insert(quantity);
		number_of_items++;
		//-----------------------------
		Param2<string, float> liquid_type = new Param2<string, float>("liquid_type", item.GetLiquidType() );
		properties.Insert(liquid_type);
		number_of_items++;
		//-----------------------------
		Param2<string, float> wetness = new Param2<string, float>("wetness", item.GetWet() );
		properties.Insert(wetness);
		number_of_items++;
		//-----------------------------
		Param2<string, float> temperature = new Param2<string, float>("temperature", item.GetTemperature() );
		properties.Insert(temperature);
		number_of_items++;
		//-----------------------------
		Param2<string, float> frozen = new Param2<string, float>("frozen", item.GetIsFrozen() );
		properties.Insert(frozen);
		number_of_items++;
		//-----------------------------
		Param2<string, float> FTProgress = new Param2<string, float>("F/T pgs", item.GetFreezeThawProgress() );
		properties.Insert(FTProgress);
		number_of_items++;
		//-----------------------------
	#ifdef DEVELOPER
		Param2<string, float> FTChangeTime = new Param2<string, float>("LastChangeTime", item.m_LastFTChangeTime );
		properties.Insert(FTChangeTime);
		number_of_items++;
		//-----------------------------
		Param2<string, float> FTRemainingTime = new Param2<string, float>("FTTimeRemaining", item.m_PresumedTimeRemaining );
		properties.Insert(FTRemainingTime);
		number_of_items++;
	#endif
		//-----------------------------
		Param2<string, float> OverheatPgs = new Param2<string, float>("Overheat pgs", item.GetItemOverheatProgress() );
		properties.Insert(OverheatPgs);
		number_of_items++;
		//-----------------------------
		Edible_Base food;
		if (Class.CastTo(food,item) && food.GetFoodStage() != null)
		{
			Param2<string, float> CookingTime = new Param2<string, float>("CookingTime", food.GetCookingTime() );
			properties.Insert(CookingTime);
			number_of_items++;
			//-----------------------------
			Param2<string, float> DecayTime = new Param2<string, float>("DecayTime (remaining)", food.GetDecayTimer() );
			properties.Insert(DecayTime);
			number_of_items++;
			//-----------------------------
			Param2<string, float> DecayDelta = new Param2<string, float>("DecayDelta (last)", food.GetDecayDelta() );
			properties.Insert(DecayDelta);
			number_of_items++;
		}
		//-----------------------------
		Param2<string, float> energy = new Param2<string, float>("energy", item.GetEnergy() );
		properties.Insert(energy);
		number_of_items++;
		//-----------------------------
		Param2<string, float> health;
		if(!client)
		{
			health = new Param2<string, float>("health", item.GetHealth("", "") );
		}
		else
		{
			health = new Param2<string, float>("NO SYNC",0 );
		}
		properties.Insert(health);
		number_of_items++;
		//-----------------------------
		/*int r,g,b,a;
		item.GetColor(r,g,b,a);
		Param2<string, float> p2r = new Param2<string, float>("R",r);
		properties.Insert(p2r);
		number_of_items++;
		Param2<string, float> p2g = new Param2<string, float>("G",g);
		properties.Insert(p2g);
		number_of_items++;
		Param2<string, float> p2b = new Param2<string, float>("B",b);
		properties.Insert(p2b);
		number_of_items++;
		Param2<string, float> p2a = new Param2<string, float>("A",a);
		properties.Insert(p2a);
		number_of_items++;*/
		//-----------------------------
		if(!client)
		{
			Param1<int> item_count = new Param1<int>(number_of_items);
			properties.InsertAt(item_count,2);
		}
	}

	int FillWithAgents(ItemBase item, array<ref Param> properties)
	{
		if (!item)
		{
			return 0;
		}

		int agents = item.GetAgents();
		if(agents == 0) return 0;
		int num_of_agents = 0;
		ref array<int> agents_aray = new array<int>;
		PluginTransmissionAgents.BuildAgentArray(agents, agents_aray);
		int agents_count = agents_aray.Count();
	
		for(int i = 0; i < agents_count; i++)
		{
			//PluginTransmissionAgents mta = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
			string agent_name = PluginTransmissionAgents.GetNameByID(agents_aray.Get(i));
			Param1<string> param1 = new Param1<string>(agent_name);
			properties.Insert(param1);
			num_of_agents++;
		}

		return num_of_agents;
	}
}
	


class HudDebugWinCharStats extends HudDebugWinBase
{
	TextListboxWidget						m_WgtValues;
	Widget									m_WgtPanel;
	ref array<ref Widget> 					m_StatWidgets = new array<ref Widget>;
	ref map <ref SliderWidget, string> 		m_SliderWidgets = new map<ref SliderWidget, string>;
	ref array<ref TextWidget> 				m_StatValues = new array<ref TextWidget>;
	ref map<ref EditBoxWidget, string> 		m_StatValuesInput = new map<ref EditBoxWidget, string>;
	bool									m_Populated;
	bool									m_ChangingSlider;
	
	//============================================
	// Constructor
	//============================================
	void HudDebugWinCharStats(Widget widget_root)
	{
		m_WgtRoot = widget_root;
		m_WgtPanel = Widget.Cast(m_WgtRoot.FindAnyWidget("Stats") );
		//FitWindow();
	}

	//============================================
	// Destructor
	//============================================
	void ~HudDebugWinCharStats()
	{	
		SetUpdate( false );
	}


	//============================================
	// GetWinType
	//============================================
	override int GetType()
	{
		return HudDebug.HUD_WIN_CHAR_STATS;
	}
	
	//============================================
	// Show 
	//============================================
	override void Show()
	{
		super.Show();
		
		//Print("Show()");
		
		SetUpdate( true );
	}

	//============================================
	// Hide 
	//============================================
	override void Hide()
	{
		super.Hide();
		
		//Print("Hide()");
		
		SetUpdate( false );
	}

	//============================================
	// SetUpdate
	//============================================
	override void SetUpdate( bool state )
	{
		//Disable update on server (PluginDeveloperSync)
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		
		PluginDeveloperSync developer_sync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
		
		//if client, send RPC
		if ( g_Game.IsClient() )
		{
			ref Param1<bool> params = new Param1<bool>( state );
			if ( player )
			{
				player.RPCSingleParam( ERPCs.DEV_STATS_UPDATE, params, true );
				SetRPCSent();
			}
		}
		//else set directly
		else
		{
			if ( developer_sync )
			{
				developer_sync.EnableUpdate( state, ERPCs.DEV_STATS_UPDATE, player );	
			}
		}
	}


	override void Update()
	{
		super.Update();
		
		if (!m_Populated)
			SetupValues();
		
		UpdateValues();
	}
	
	void SetupValues()
	{
		PluginDeveloperSync developerSync = PluginDeveloperSync.Cast(GetPlugin(PluginDeveloperSync));
		
		//clear window
		//ClearValues();
			
		if ( developerSync.m_PlayerStatsSynced.Count() > 0 )
		{
			foreach (SyncedValue syncedValue : developerSync.m_PlayerStatsSynced)
			{
				string name = syncedValue.GetName();
				string value = syncedValue.GetValue().ToString();
				
				AddValue(name, value);
			}

			FitWindow();
			m_Populated = true;
		}
	}
	
	
	void UpdateValues()
	{
		PluginDeveloperSync developerSync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
		if ( developerSync.m_PlayerStatsSynced.Count() > 0 )
		{
			foreach (int i, SyncedValue syncedValue : developerSync.m_PlayerStatsSynced)
			{
				string statName 		= syncedValue.GetName();
				float valueNormalized 	= syncedValue.GetValueNorm();
				float value 			= syncedValue.GetValue();
				
				if ( statName == "BloodType" )
				{
					string type, name;
					bool positive;
					
					name = value.ToString();
					name += "("+BloodTypes.GetBloodTypeName(Math.Round(value), type, positive)+")";
					m_StatValues.Get(i).SetText(name);
				}
				else
				{
					if (statName == "HeatBuffer")
					{
						float heatBufferNormalized = Math.Round(Math.Lerp(-1, 1, valueNormalized) * 1000) * 0.001;
						m_StatValues.Get(i).SetText(string.Format("%1 (%2)", heatBufferNormalized.ToString(), value.ToString()));
					}
					else
						m_StatValues.Get(i).SetText(value.ToString());
					
				}

				if (!m_ChangingSlider)
					m_SliderWidgets.GetKeyByValue(statName).SetCurrent(valueNormalized * 100);
			}
		}
		
	}
	
	void AddValue(string title, string value)
	{
		Widget widget = g_Game.GetWorkspace().CreateWidgets( "gui/layouts/debug/day_z_hud_debug_stat.layout", m_WgtPanel );
		
		TextWidget tw = TextWidget.Cast(widget.FindAnyWidget("StatName"));
		tw.SetText(title);
		m_StatWidgets.Insert(widget);
		
		TextWidget tw_output = TextWidget.Cast(widget.FindAnyWidget("OutputValue"));
		m_StatValues.Insert(tw_output);
		
		EditBoxWidget ebw_input = EditBoxWidget.Cast(widget.FindAnyWidget("InputValue"));
		m_StatValuesInput.Insert(ebw_input, title );
		
		SliderWidget sw = SliderWidget.Cast(widget.FindAnyWidget("StatSlider"));
		m_SliderWidgets.Insert(sw,title );
		
		AutoHeightSpacer WgtModifiersContent_panel_script;
		m_WgtPanel.GetScript( WgtModifiersContent_panel_script );
		WgtModifiersContent_panel_script.Update();
	}

	void ClearValues()
	{
		m_StatWidgets.Clear();
	}

	void FitWindow()
	{
		TextListboxWidget wgt = TextListboxWidget.Cast(m_WgtPanel);
		if(wgt)
			FitWindowByContent( wgt );
	}
	
	bool OnClick( Widget w, int x, int y, int button )
	{	
		if ( w.GetName() == "ResetStats" )
		{
			ResetStats();
			return true;
		}

		return false;
	}
	
	bool OnChange(Widget w, int x, int y, bool finished)
	{
		if ( m_StatValuesInput.Contains(EditBoxWidget.Cast(w)) && finished )
		{
			EditBoxWidget ebw = EditBoxWidget.Cast(w);
			RPCChangeStat(m_StatValuesInput.Get(EditBoxWidget.Cast(w)), ebw.GetText().ToFloat());
			return true;
		}
		if (m_SliderWidgets.Contains(SliderWidget.Cast(w)))
		{
			m_ChangingSlider = true;
			string stat_name = m_SliderWidgets.Get(SliderWidget.Cast(w));
			SliderWidget sw = SliderWidget.Cast(w);
			PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
			for ( int i = 0; i < player.m_PlayerStats.GetPCO().Get().Count(); i++ ) 
			{
				string label = player.m_PlayerStats.GetPCO().Get().Get( i ).GetLabel();
				if(label == stat_name)
				{
					float stat_min = player.m_PlayerStats.GetPCO().Get().Get( i ).GetMin();
					float stat_max = player.m_PlayerStats.GetPCO().Get().Get( i ).GetMax();
					float current_value_norm = sw.GetCurrent() / 100;
					float current_value_abs = stat_min + (stat_max - stat_min) * current_value_norm;
					
					RPCChangeStat(label, current_value_abs);
				}
			}
			//Print("OnChange " + finished);
			if(finished)
				m_ChangingSlider = false;
		}
		
		return false;
	}
	
	
	void ResetStats()
	{
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		
		//if client, send RPC

		ref Param1<bool> params = new Param1<bool>( false );
		if ( player )
		{
			player.RPCSingleParam( ERPCs.DEV_RPC_STATS_RESET, params, true );
		}
	}
	
	void RPCChangeStat(string stat, float value)
	{
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		
		//if client, send RPC

		ref Param2<string, float> params = new Param2<string, float>( stat, value );
		if ( player )
		{
			player.RPCSingleParam( ERPCs.DEV_RPC_STAT_SET, params, true );
		}
	}
	
	
}

class MapMenu extends UIScriptedMenu
{
	protected const string COORD_FORMAT 			= "%1.%2%3";
	protected const int SCALE_RULER_LINE_WIDTH 		= 8;
	protected const int SCALE_RULER_NUM_SEGMENTS 	= 10;

	protected bool 							m_WasChanged;
	protected bool 							m_HasCompass
	protected bool 							m_HasGPS
	protected bool 							m_IsOpenning;
	
	protected float							m_ToolScaleCellSizeCanvasWidth;

	protected ref IngameHud					m_Hud ;
	protected ref MapHandler 				m_MapMenuHandler;
	protected ref MapWidget 				m_MapWidgetInstance;
	protected ref SizeToChild				m_LegendResizer;

	protected ImageWidget 					m_Images;
	protected Widget						m_GPSMarker;
	protected ImageWidget					m_GPSMarkerArrow;
	protected Widget						m_UpperLegendContainer;
	protected Widget						m_ToolsCompassBase;
	protected ImageWidget					m_ToolsCompassArrow;
	protected TextWidget					m_ToolsCompassAzimuth;
	protected TextWidget					m_ToolsScaleContourText;
	protected TextWidget					m_ToolsGPSElevationText;
	protected TextWidget					m_ToolsGPSXText;
	protected TextWidget					m_ToolsGPSYText;
	protected TextWidget					m_ToolsScaleCellSizeText;
	protected CanvasWidget					m_ToolsScaleCellSizeCanvas;
	protected ItemMap 						m_Map;
	//int 									m_MarkerCount;
	
	protected ref MapNavigationBehaviour	m_MapNavigationBehaviour;
	
	override Widget Init()
	{
		layoutRoot 			= GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_map.layout");
		m_Hud 				= IngameHud.Cast(GetGame().GetMission().GetHud());
		m_IsOpenning		= true;
		
		/*MapWidget m = MapWidget.Cast(layoutRoot.FindAnyWidget("Map"));
		if (m)
		{
			m.AddUserMark("2681 4.7 1751", "Lalal", ARGB(255,255,0,0), "\\dz\\gear\\navigation\\data\\map_tree_ca.paa");
			m.AddUserMark("2683 4.7 1851", "Lala2", ARGB(255,0,255,0), "\\dz\\gear\\navigation\\data\\map_bunker_ca.paa");
			m.AddUserMark("2670 4.7 1651", "Lala3", ARGB(255,0,0,255), "\\dz\\gear\\navigation\\data\\map_busstop_ca.paa");
		}*/
		
		Widget mapToolsContainer = layoutRoot.FindAnyWidget("Map_Tools_Container");
		mapToolsContainer.GetScript(m_LegendResizer);

		m_MapWidgetInstance			= MapWidget.Cast(layoutRoot.FindAnyWidget("Map"));
		m_GPSMarker					= layoutRoot.FindAnyWidget("GPSMarkerCircle");
		m_GPSMarkerArrow			= ImageWidget.Cast(layoutRoot.FindAnyWidget("GPSMarkerArrow"));
		m_UpperLegendContainer		= layoutRoot.FindAnyWidget("Tools_Extra");
		layoutRoot.Update();
		m_ToolsCompassBase			= layoutRoot.FindAnyWidget("Tools_Compass_Base");
		m_ToolsCompassArrow			= ImageWidget.Cast(layoutRoot.FindAnyWidget("Tools_Compass_Arrow"));
		m_ToolsCompassAzimuth		= TextWidget.Cast(layoutRoot.FindAnyWidget("Tools_Compass_Azimuth"));
		m_ToolsGPSXText				= TextWidget.Cast(layoutRoot.FindAnyWidget("Tools_GPS_X_Value"));
		m_ToolsGPSYText				= TextWidget.Cast(layoutRoot.FindAnyWidget("Tools_GPS_Y_Value"));
		m_ToolsGPSElevationText		= TextWidget.Cast(layoutRoot.FindAnyWidget("Tools_GPS_Elevation_Value"));
		m_ToolsScaleContourText		= TextWidget.Cast(layoutRoot.FindAnyWidget("Tools_Scale_Contour_Value"));
		m_ToolsScaleCellSizeText	= TextWidget.Cast(layoutRoot.FindAnyWidget("Tools_Scale_CellSize_Value"));
		m_ToolsScaleCellSizeCanvas	= CanvasWidget.Cast(layoutRoot.FindAnyWidget("Tools_Scale_CellSize_Canvas"));
		
		float canvasHeight = 0;
		m_ToolsScaleCellSizeCanvas.GetSize(m_ToolScaleCellSizeCanvasWidth, canvasHeight);

		if (m_MapWidgetInstance)
		{
			float scale;
			vector mapPosition;
			PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
			if (player && !player.GetLastMapInfo(scale, mapPosition))
			{
				vector tempPosition = GetGame().ConfigGetVector(string.Format("CfgWorlds %1 centerPosition", GetGame().GetWorldName()));
				scale = 0.33;
				mapPosition = Vector(tempPosition[0], tempPosition[1], tempPosition[2]);
			}

			m_MapWidgetInstance.SetScale(scale);
			m_MapWidgetInstance.SetMapPos(mapPosition);
			
			m_HasCompass 	= false;
			m_HasGPS 		= false;

			SetGPSMarkerVisibility(false);
			SetGPSDirectionVisibility(false);
			SetCompassUIVisibility(false);
			SetUpperLegendVisibility(true);
			
			if (player)
			{
				m_MapNavigationBehaviour = player.GetMapNavigationBehaviour();
				if (m_MapNavigationBehaviour)
				{
					m_HasGPS = (m_MapNavigationBehaviour.GetNavigationType() & EMapNavigationType.GPS|EMapNavigationType.ALL == 0);				
					m_HasCompass = (m_MapNavigationBehaviour.GetNavigationType() & EMapNavigationType.COMPASS|EMapNavigationType.ALL == 0);
					
					//! gameplay cfg json overrides handling
					m_HasGPS 		= CfgGameplayHandler.GetMapIgnoreNavItemsOwnership() || m_HasGPS;
					m_HasCompass 	= CfgGameplayHandler.GetMapIgnoreNavItemsOwnership() || m_HasCompass;

					if (m_HasGPS)
					{
						SetUpperLegendVisibility(true);
						if (m_HasGPS && CfgGameplayHandler.GetMapDisplayPlayerPosition())
						{
							SetGPSMarkerVisibility(true);
							m_MapWidgetInstance.SetMapPos(m_MapNavigationBehaviour.GetPositionReal());
						}
					}
					
					if (m_HasCompass)
					{
						SetCompassUIVisibility(true);
						SetUpperLegendVisibility(true);
						if (m_HasGPS && CfgGameplayHandler.GetMapDisplayPlayerPosition())
						{
							SetGPSDirectionVisibility(true);
						}
					}
					
					//! override the CfgGameplayHandler.GetMapIgnoreNavItemsOwnership()
					if ((!m_HasGPS && !m_HasCompass) || !CfgGameplayHandler.GetMapDisplayNavigationInfo())
					{
						SetUpperLegendVisibility(false);
					}
				}
			}
			
			m_MapMenuHandler = new MapHandler(m_MapWidgetInstance);
			
			if (m_Hud)
			{
				m_Hud.ShowHudUI(false);
				m_Hud.ShowQuickbarUI(false);
			}
		}
		
		return layoutRoot;
	}
	
	override void InitMapItem(EntityAI item)
	{
		super.InitMapItem(item);
		
		m_Map = ItemMap.Cast(item);
		//m_Map.SyncMapMarkers();
		//m_MarkerCount = m_Map.GetMarkerArray().Count();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		switch (w.GetUserID())
		{
			case IDC_CANCEL:
				CloseMapMenu();
				return true;
		}
		
		return false;
	}
	
	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		super.OnKeyPress(w, x, y, key);
		
		Print(key);
		
		return false;
	}

	//TODO if it does not work well enough, attach some ScriptedWidgetEventHandler to the MapWidget
	/*override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		super.OnDoubleClick(w, x, y, button);
		
		MapWidget m = MapWidget.Cast(layoutRoot.FindAnyWidget("Map"));
		if ( w == m )
		{
			vector screen_to_map = m.ScreenToMap(Vector(x,y,0));
			
			AddMarker(w,screen_to_map,
			
			//m.AddUserMark(screen_to_map, "marker", ARGB(255,0,0,255), "\\dz\\gear\\navigation\\data\\map_tree_ca.paa");
						
			return true;
		}
		
		return false;
	}*/

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		m_ToolsScaleCellSizeCanvas.Clear();

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		
		if (m_MapWidgetInstance)
		{
			if (m_Images)
			{
				m_MapWidgetInstance.RemoveChild(m_Images);
			}

			if (player)
			{
				m_ToolsScaleContourText.SetText(string.Format("%1 m", m_MapWidgetInstance.GetContourInterval()));
				RenderScaleRuler();
				float rulerMaxDistance;
				string rulerUnits;
				ProcessDistanceAndUnits(m_MapWidgetInstance.GetCellSize(m_ToolScaleCellSizeCanvasWidth), rulerMaxDistance, rulerUnits);
				m_ToolsScaleCellSizeText.SetText(string.Format("%1%2", rulerMaxDistance, rulerUnits));

				if (m_MapNavigationBehaviour)
				{
					vector mapPos = m_MapWidgetInstance.MapToScreen(m_MapNavigationBehaviour.GetPositionReal());
					float scale = 1 - m_MapWidgetInstance.GetScale();

					if (m_HasCompass)
					{
						vector rot = player.GetYawPitchRoll();
						float angle = Math.Round(rot[0]);
						if (angle < 0)
						{
							angle = 360 + angle;
						}

						m_GPSMarkerArrow.SetRotation(0, 0, angle);
						m_ToolsCompassArrow.SetRotation(0, 0, angle);
						m_ToolsCompassAzimuth.SetText(string.Format("%1°", angle));
					}

					array<int> coords = MapNavigationBehaviour.OrderedPositionNumbersFromGridCoords(player);
					if (m_HasGPS || CfgGameplayHandler.GetMapIgnoreNavItemsOwnership())
					{
						m_GPSMarker.SetSize(scale * 100, scale * 100);
						m_GPSMarkerArrow.SetSize(scale * 100, scale * 100);
						float sizeX, sizeY;
						m_GPSMarker.GetSize(sizeX, sizeY);
						sizeX = Math.Round(sizeX);
						sizeY = Math.Round(sizeY);
						m_GPSMarker.SetPos(mapPos[0] - sizeX/2, mapPos[1] - sizeY/2);
						m_GPSMarkerArrow.SetPos(mapPos[0] - sizeX/2, mapPos[1] - sizeY/2);
						
						if (coords.Count() == m_MapNavigationBehaviour.DISPLAY_GRID_POS_MAX_CHARS_COUNT * 2 && coords[0] >= 0)
						{
							m_ToolsGPSXText.SetText(string.Format(COORD_FORMAT, coords[0], coords[1], coords[2]));
							m_ToolsGPSYText.SetText(string.Format(COORD_FORMAT, coords[3], coords[4], coords[5]));
						}
						else
						{
							m_ToolsGPSXText.SetText("-.--");
							m_ToolsGPSYText.SetText("-.--");						
						}

						m_ToolsGPSElevationText.SetText(string.Format("%1m", Math.Round(player.GetPosition()[1])));
					}
					else
					{
						m_ToolsGPSXText.SetText("-.--");
						m_ToolsGPSYText.SetText("-.--");
						m_ToolsGPSElevationText.SetText("----m");
					}
				}
			}
		
			bool isClosedWithShortcut = CfgGameplayHandler.GetMapIgnoreMapOwnership() && GetGame().GetInput().LocalPress("UAMapToggle", false);
			if (!m_IsOpenning && GetGame().GetInput() && (GetGame().GetInput().LocalPress("UAUIBack", false) || isClosedWithShortcut))
			{
				if (player)
				{
					player.SetLastMapInfo(m_MapWidgetInstance.GetScale(), m_MapWidgetInstance.GetMapPos());
				}
				
				CloseMapMenu();
			}
			
			m_IsOpenning = false;
		}
	}
	
	void AddMarker(vector pos, int color, int icon = 0)
	{
		m_Map.InsertMarker(pos,"",color,icon);
		m_MapWidgetInstance.AddUserMark(pos, "", color, MapMarkerTypes.GetMarkerTypeFromID(icon));
		m_WasChanged = true;
	}
	
	void RemoveMarker()
	{
		m_WasChanged = true;
	}
	
	override void LoadMapMarkers()
	{
		MapMarker marker;
		for ( int i = 0; i < m_Map.GetMarkerArray().Count(); i++ )
		{
			marker = m_Map.GetMarkerArray().Get(i);
			m_MapWidgetInstance.AddUserMark(marker.GetMarkerPos(),marker.GetMarkerText(),marker.GetMarkerColor(),MapMarkerTypes.GetMarkerTypeFromID(marker.GetMarkerIcon())/*"\\dz\\gear\\navigation\\data\\map_tree_ca.paa"*/);
		}
	}
	
	void CloseMapMenu()
	{
		if (m_WasChanged)
		{
			m_Map.SyncMapMarkers();
			m_WasChanged = false;
		}
		
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player)
		{
			player.SetMapClosingSyncSet(false); //map is closing, server needs to be notified - once
		}

		if (m_Hud)
		{
			m_Hud.ShowHudUI(true);
			m_Hud.ShowQuickbarUI(true);
		}

		if (CfgGameplayHandler.GetMapIgnoreMapOwnership())
		{
			if (!CfgGameplayHandler.GetUse3DMap())
			{
				GetGame().GetMission().RemoveActiveInputExcludes({"map"});
			}
			else
			{
				GetGame().GetMission().RemoveActiveInputExcludes({"loopedactions"});
			}

			GetGame().GetMission().RemoveActiveInputRestriction(EInputRestrictors.MAP);
		}

		Close();
	}

	protected void SetCompassUIVisibility(bool pState)
	{
		if (m_ToolsCompassArrow)
		{
			m_ToolsCompassArrow.Show(pState);
		}
		
		if (m_ToolsCompassAzimuth)
		{
			m_ToolsCompassAzimuth.Show(pState);
		}
	}
	
	protected void SetGPSMarkerVisibility(bool pState)
	{
		if (m_GPSMarker)
		{
			m_GPSMarker.Show(pState);
		}
	}
	
	protected void SetGPSDirectionVisibility(bool pState)
	{
		if (m_GPSMarkerArrow)
		{
			m_GPSMarkerArrow.Show(pState);
		}
	}
	
	protected void SetUpperLegendVisibility(bool pState)
	{
		if (m_UpperLegendContainer)
		{
			m_UpperLegendContainer.Show(pState);
		}
		
		if (m_LegendResizer)
		{
			m_LegendResizer.ResizeParentToChild();
		}
	}
	
	protected void RenderScaleRuler()
	{
		float sizeYShift = 8;
		float segmentLength = m_ToolScaleCellSizeCanvasWidth / SCALE_RULER_NUM_SEGMENTS;
		int lineColor = FadeColors.BLACK;

		for (int i = 1; i <= SCALE_RULER_NUM_SEGMENTS; i++)
		{
			lineColor = FadeColors.BLACK;
			if (i % 2 == 0)
			{
				lineColor = FadeColors.LIGHT_GREY;
			}

			if (i == 1) //! 1st segment
			{
				m_ToolsScaleCellSizeCanvas.DrawLine(0, sizeYShift, segmentLength, sizeYShift, SCALE_RULER_LINE_WIDTH, lineColor);
			}
			else if (i == SCALE_RULER_NUM_SEGMENTS) //! last segment
			{
				m_ToolsScaleCellSizeCanvas.DrawLine(segmentLength * (SCALE_RULER_NUM_SEGMENTS - 1), sizeYShift, segmentLength * SCALE_RULER_NUM_SEGMENTS, sizeYShift, SCALE_RULER_LINE_WIDTH, lineColor);
			}
			else
			{
				m_ToolsScaleCellSizeCanvas.DrawLine(segmentLength * (i - 1), sizeYShift, segmentLength * i, sizeYShift, SCALE_RULER_LINE_WIDTH, lineColor);
			}
		}
	}
	
	protected void ProcessDistanceAndUnits(float num, out float dist, out string units)
	{
		if (num >= 901)
		{	
			num *= 0.001;
			num = Math.Round(num * 10) * 0.1;
			dist = num;
			units = "km";
		}
		else if (num >= 100 && num <= 900)
		{
			num = Math.Ceil(num * 0.1) * 10;
			dist = num;
			units = "m";
		}
		else if (num >= 1)
		{
			num = Math.Floor(num);
			dist = num;
			units = "m";
		}
		else
		{
			num = Math.Ceil(num * 10);
			dist = num;
			units = "cm";
		}
	}
}

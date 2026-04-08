class JsonMissionLoaderData
{
	ref TStringArray MissionPaths;

	static JsonMissionLoaderData GetData()
	{
		JsonMissionLoaderData data;
		
		string path;
		string errorMessage;

		if (GetCLIParam("missionLoaderPath", path) == false)
		{
			path = CFG_FILE_MISSION_LIST;
		}
		
		if (!FileExist(path))
		{
			DayZGame dzg = GetDayZGame();
			
			data = new JsonMissionLoaderData();
			data.MissionPaths = {dzg.GetMissionFolderPath()};
			if (!JsonFileLoader<JsonMissionLoaderData>.SaveFile(path, data, errorMessage))
				ErrorEx(errorMessage);
		}
		else
		{
			if (!JsonFileLoader<JsonMissionLoaderData>.LoadFile(path, data, errorMessage))
				ErrorEx(errorMessage);
		}

		return data;
	}
}

class MissionLoader : UIScriptedMenu
{
	
	protected TextListboxWidget			m_WgtLstMsnList;
	protected ButtonWidget				m_WgtBtnMsnPlay;
	protected ButtonWidget				m_WgtBtnMsnClose;
	protected ref TStringArray			m_ListMissionsNames;
	protected ref JsonMissionLoaderData m_MissionData;
	
	override Widget Init()
	{
		m_MissionData = JsonMissionLoaderData.GetData();
		
		layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/day_z_mission_loader.layout");	
		
		m_WgtLstMsnList = TextListboxWidget.Cast( layoutRoot.FindAnyWidget("wgt_lst_missions") );
		m_WgtBtnMsnPlay = ButtonWidget.Cast( layoutRoot.FindAnyWidget("wgt_btn_mission_play") );
		m_WgtBtnMsnClose = ButtonWidget.Cast( layoutRoot.FindAnyWidget("wgt_btn_mission_close") );

		foreach (string path:m_MissionData.MissionPaths)
		{
			m_WgtLstMsnList.AddItem(path, NULL, 0);
		}
		
		return layoutRoot;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if ( w == m_WgtBtnMsnClose )
		{
			Close();
			
			return true;
		}
		else if ( w == m_WgtBtnMsnPlay )
		{
			int rowIndex = m_WgtLstMsnList.GetSelectedRow();
			string missionPath = m_MissionData.MissionPaths.Get(rowIndex);
			g_Game.PlayMission(missionPath);
			return true;
		}
		return false;
	}
	
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w == m_WgtLstMsnList)
		{
			int rowIndex = m_WgtLstMsnList.GetSelectedRow();
			string missionPath = m_MissionData.MissionPaths.Get(rowIndex);
			g_Game.PlayMission(missionPath);
		}
		return false;
	}
	
	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		super.OnKeyDown(w,x,y,key);
		switch (key)
		{
			case KeyCode.KC_ESCAPE:
			{
				Close();
				return true;
			}
		}
		return false;
	}

}
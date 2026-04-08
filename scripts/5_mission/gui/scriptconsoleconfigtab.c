typedef Param5<bool, string, int, string, int> ConfigParams; // param1 - isCollapsed, param2 - string name, param3 - num of childs, param4 - path, param5 - deep
typedef Param6<bool, string, int, string, int, string> ConfigParamsEx; // param1 - isCollapsed, param2 - string name, param3 - num of childs, param4 - path, param5 - deep

class ScriptConsoleConfigTab : ScriptConsoleTabBase
{
	
	protected static string 						m_ConfigTextField;
	protected static string 						m_VariableTextField;
	protected static ref ConfigParamsEx 			m_ConfigData;
	protected ref TStringArray 						m_BaseConfigClasses = new TStringArray;
	protected ref TStringArray 						m_BaseConfigClassesToggled = new TStringArray;
	protected ref map<CheckBoxWidget, int>			m_ClassCheckboxes = new map<CheckBoxWidget, int>;

	protected EditBoxWidget 		m_ObjectConfigFilter;
	protected EditBoxWidget 		m_VariableConfigFilter;
	protected TextListboxWidget 	m_ConfigHierarchyTextListbox;
	protected TextListboxWidget 	m_ConfigVariablesTextListbox;
	protected ButtonWidget 			m_SelectedRowCopy;
	protected ButtonWidget 			m_DumpParamButton;
	protected TextWidget 			m_ClassPath;
	
	
	protected Widget 				m_WgtClassesConfig;
	protected PluginConfigViewer 	m_ModuleConfigViewer;
	protected int					m_Row;
	
	void ScriptConsoleConfigTab(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
	{
		m_WgtClassesConfig 				= root.FindAnyWidget("ClassesPanel");
		m_DumpParamButton				= ButtonWidget.Cast(root.FindAnyWidget("DumpParamButton"));
		m_ObjectConfigFilter 			= EditBoxWidget.Cast(root.FindAnyWidget("ObjectConfigFilter"));
		m_VariableConfigFilter 			= EditBoxWidget.Cast(root.FindAnyWidget("VariableConfigFilter"));
		m_ConfigHierarchyTextListbox 	= TextListboxWidget.Cast(root.FindAnyWidget("ConfigHierarchy"));
		m_ConfigVariablesTextListbox 	= TextListboxWidget.Cast(root.FindAnyWidget("ConfigVariables"));
		m_SelectedRowCopy 				= ButtonWidget.Cast(root.FindAnyWidget("SelectedRowCopy"));
		m_ClassPath 					= TextWidget.Cast(root.FindAnyWidget("ClassPath"));
		
		if (m_ConfigTextField)
			m_ObjectConfigFilter.SetText(m_ConfigTextField);
		if (m_VariableTextField)
			m_VariableConfigFilter.SetText(m_VariableTextField);
		Init();
	}
	
	void ~ScriptConsoleConfigTab()
	{
		m_VariableTextField = m_VariableConfigFilter.GetText();
		
		foreach (int i:m_ClassCheckboxes)
		{
			Widget w = m_ClassCheckboxes.GetKeyByValue(i);
			w.GetParent().Unlink();
		}
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		if (w == m_ObjectConfigFilter)
		{
			ChangeConfigFilter();
			return true;
		}
		else if (w == m_VariableConfigFilter)
		{
			RenderVariables(m_Row);
			return true;
		}
		
		return false;
	}
	
	override bool OnItemSelected(Widget w, int x, int y, int row, int  column,	int  oldRow, int  oldColumn)
	{
		super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
		
		if (w == m_ConfigHierarchyTextListbox)
		{
			TextListboxWidget wgt = TextListboxWidget.Cast(w);
			wgt.GetItemData(row, 0,m_ConfigData);
			RenderClassPath();
			
			return true;
		}
		return false;
	}
	
	void RenderClassPath()
	{
		m_ClassPath.SetText(m_ModuleConfigViewer.GetBaseClasses(m_ConfigData.param4, m_ConfigData.param2));
	}
	
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w,x,y,button);
		
		CheckBoxWidget cbw = CheckBoxWidget.Cast(w);
		
		if (w == m_DumpParamButton)
		{
			int selectedRowIndex = m_ConfigVariablesTextListbox.GetSelectedRow();
			string paramFinal;
			if (selectedRowIndex > -1)
			{
				string param;
				m_ConfigVariablesTextListbox.GetItemText(selectedRowIndex,0,param);
				int index = param.IndexOf("=");
				if (index > 0)
					paramFinal = param.Substring(0, index).Trim();
			}
			
			int objects_row_index;
			objects_row_index = m_ConfigHierarchyTextListbox.GetSelectedRow();
			if (objects_row_index > -1 && objects_row_index < m_ConfigHierarchyTextListbox.GetNumItems())
			{
				ConfigParamsEx params;
				m_ConfigHierarchyTextListbox.GetItemData(objects_row_index, 0, params);
				string path = params.param4;
				TStringArray pathArr = new TStringArray();
				path.Split(" ", pathArr);
				string relativePath;
				
				foreach (int indx, string s:pathArr)
				{
					if (indx > 2)
					{
						relativePath+= s+" ";
					}
				}
				
				if (relativePath)
					relativePath = relativePath.Trim();
				
				Print(relativePath);
				Print(path);
			}
			
			if (paramFinal)
				DumpParam(paramFinal,relativePath);
			
			return true;
		}
		else if (w == m_ConfigHierarchyTextListbox)
		{
			objects_row_index = m_ConfigHierarchyTextListbox.GetSelectedRow();
			//Print(objects_row_index);

			if (objects_row_index > -1 && objects_row_index < m_ConfigHierarchyTextListbox.GetNumItems())
			{
				ConfigParamsEx config_params;
				m_ConfigHierarchyTextListbox.GetItemData(objects_row_index, 0, config_params);

				if (config_params.param1 == false)
				{
					ExpandHierarchy(objects_row_index);
				}
				else
				{
					CollapseHierarchy(objects_row_index);
				}
				
				RenderVariables(objects_row_index);
				m_Row = objects_row_index; 
			}
			return true;
		}
		else if (cbw && m_ClassCheckboxes.Contains(cbw))
		{
			int checkbox_index = m_ClassCheckboxes.Get(cbw);
			string base_class_name = m_BaseConfigClasses.Get(checkbox_index);
			
			int index_toggled = m_BaseConfigClassesToggled.Find(base_class_name);
			
			int flag, new_flag;
			
			if (m_ConfigDebugProfile)
			{
				flag = m_ConfigDebugProfile.GetConfigClassesFlag();
			}
			
			if (cbw.IsChecked())
			{
				if (index_toggled == -1)//not found
				{
					m_BaseConfigClassesToggled.Insert(base_class_name);
					
					if (m_ConfigDebugProfile)
					{
						new_flag = (flag | (int)Math.Pow(2, checkbox_index));
					}
				}
			}
			else if (index_toggled != -1)
			{
				m_BaseConfigClassesToggled.Remove(index_toggled);
				
				if (m_ConfigDebugProfile)
				{
					new_flag = (flag ^ (int)Math.Pow(2, checkbox_index));
				}
			}
			m_ConfigDebugProfile.SetConfigClassesFlag(new_flag);
			/*
			Print("-----------------------------------------");
			Print("flag = " + new_flag);
			foreach (string text:m_BaseConfigClassesToggled)
			{
				Print(text);
			}
			Print("-----------------------------------------");
			*/
			ClearHierarchy();
		}
		else if (w == m_SelectedRowCopy)
		{
			AddItemToClipboard(m_ConfigVariablesTextListbox);
			return true;
		}
		return false;
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
	}
	
	protected void Init()
	{
		Debug.GetBaseConfigClasses(m_BaseConfigClasses);
		m_ModuleConfigViewer		= PluginConfigViewer.Cast(GetPlugin(PluginConfigViewer));
		int flag1;
		
		if (m_ConfigDebugProfile)
		{
			flag1 = m_ConfigDebugProfile.GetConfigClassesFlag();
		}
		//Print("------------------------------------------------");
		foreach (string s:m_BaseConfigClasses) 
		{
			Widget w = g_Game.GetWorkspace().CreateWidgets("gui/layouts/script_console/config_class_item.layout", m_WgtClassesConfig);
			CheckBoxWidget cbw = CheckBoxWidget.Cast(w.FindAnyWidget("Item"));
			//cbw.SetColor(Colors.RED);
			cbw.SetText(s);
			int indx = m_ClassCheckboxes.Count();
			m_ClassCheckboxes.Insert(cbw,indx);
			//Print("adding "+cbw +" at index:" + indx);
				
			if (flag1 & (int)Math.Pow(2, indx))
			{
				string base_class_name = m_BaseConfigClasses.Get(indx);
				m_BaseConfigClassesToggled.Insert(base_class_name);
				cbw.SetChecked(true);
			}
		}
		
		ClearHierarchy();
		ChangeConfigFilter();
	}
	
	protected void DumpParam(string param, string relativePath)
	{
		if (relativePath)
		{
			relativePath = " " + relativePath;
		}
		for ( int i = 0; i < m_BaseConfigClassesToggled.Count(); i++ )
		{
			string config_path = m_BaseConfigClassesToggled.Get(i);
		
			int objects_count = g_Game.ConfigGetChildrenCount( config_path );
			
			for ( int j = 0; j < objects_count; j++ )
			{
				string child_name;
				g_Game.ConfigGetChildName( config_path, j, child_name );
		
				
				//string dispName; 
						//g_Game.ConfigGetText( config_path + " " + child_name + " displayName",dispName );
				string value; 
				string path = config_path + " " + child_name + relativePath + " " + param;
				if (g_Game.ConfigIsExisting(path))
				{
					g_Game.ConfigGetText( path , value);
					if (value)
						Print(child_name + "," + param + "," + value);
				}
				
				
				/*
				int scope = g_Game.ConfigGetInt( config_path + " " + child_name + " scope" );
				{
					if ( scope == 2)
					{
						
					}
				}*/
			}
		}
	}
	
	protected void ChangeConfigFilter()
	{
		m_ConfigTextField = m_ObjectConfigFilter.GetText();
		
		if (m_ConfigTextField == "")
		{
			ClearHierarchy();
		}
		else
		{
			FindInHierarchy(m_ConfigTextField);
		}
	}
	
	protected void ClearHierarchy()
	{
		m_ConfigHierarchyTextListbox.ClearItems();
		m_ConfigVariablesTextListbox.ClearItems();

		string config_path = "configfile";
		TStringArray variables = m_ModuleConfigViewer.GetConfigHierarchy(config_path);
		for (int i = 0; i < variables.Count(); i++)
		{
			string variable = variables.Get(i);

			for (int j = 0; j < m_BaseConfigClassesToggled.Count(); j++)
			{
				if (variable == m_BaseConfigClassesToggled.Get(j))
				{
					string new_config_path = (config_path + " " + variable).Trim();
					m_ConfigHierarchyTextListbox.AddItem("+ " + variable, new ConfigParamsEx(false, variable, 0, new_config_path, 0,variable), 0);
				}
			}
		}
	}

	protected void FindInHierarchy(string class_name)
	{
		m_ConfigHierarchyTextListbox.ClearItems();
		m_ConfigVariablesTextListbox.ClearItems();

		class_name.ToLower();
		string config_base_path = "configfile";

		string filter_lower = class_name;
		filter_lower.ToLower();

		TStringArray filters = new TStringArray;
		filter_lower.Split(" ", filters);

		for (int i = 0; i < m_BaseConfigClassesToggled.Count(); i++)
		{
			string config_root = m_BaseConfigClassesToggled.Get(i);
			string config_path = config_base_path + " " + config_root;
			TStringArray variables = m_ModuleConfigViewer.GetConfigHierarchy(config_path);

			for (int j = 0; j < variables.Count(); j++)
			{
				string variable = variables.Get(j);
				string variable_lower = variable;
				variable_lower.ToLower();

				for (int k = 0; k < filters.Count(); k++)
				{
					if (variable_lower.Contains(filters.Get(k)))
					{
						string new_config_path = (config_path + " " + variable).Trim();
						m_ConfigHierarchyTextListbox.AddItem("+ " + variable, new ConfigParamsEx(false, variable, 0, new_config_path, 0, config_root), 0);
						break;
					}
				}
			}
		}
	}
	
	protected void ExpandHierarchy(int row)
	{
		if (row <= -1 && row >= m_ConfigHierarchyTextListbox.GetNumItems())
		{
			return;
		}
		
		TStringArray variables;

		// get current row data
		ConfigParamsEx config_params;
		m_ConfigHierarchyTextListbox.GetItemData(row, 0, config_params);

		string config_path = config_params.param4;
		int deep = config_params.param5;
		

		string offset = "";
		for (int i = 0; i < deep; i++)
		{
			offset = offset + "  ";
		}

		// change selected node
		variables = m_ModuleConfigViewer.GetConfigHierarchy(config_path);
		int childrens_count = variables.Count();
		m_ConfigHierarchyTextListbox.SetItem(row, offset + "- " + config_params.param2, new ConfigParamsEx(true, config_params.param2, childrens_count, config_path, deep,config_params.param6), 0);

		offset = offset + "  ";

		// render children
		deep = deep + 1;
		childrens_count = 0;
		for (i = variables.Count() - 1; i >= 0; i--)
		{
			string new_config_path = (config_path + " " + variables.Get(i)).Trim();
			m_ConfigHierarchyTextListbox.AddItem(offset + "+ " + variables.Get(i), new ConfigParamsEx(false, variables.Get(i), childrens_count, new_config_path, deep,config_params.param2), 0, (row + 1));
		}
	}

	// niekde je bug, ked su len 2 polozky a expand/collapse na prvu tak zmaze aj druhu. ak collapse a expand na druhu tak crash lebo sa snazi zmazat riadok co neexistuje
	protected void CollapseHierarchy(int row)
	{
		if (row <= -1 && row >= m_ConfigHierarchyTextListbox.GetNumItems())
		{
			return;
		}
		
		ConfigParamsEx config_params;
		ConfigParamsEx config_params_next;
		m_ConfigHierarchyTextListbox.GetItemData(row, 0, config_params);
		m_ConfigHierarchyTextListbox.GetItemData(row + 1, 0, config_params_next);

		if (!config_params || !config_params_next)
			return;
		int deep = config_params.param5;
		int deep_next = config_params_next.param5;
		int max_count = m_ConfigHierarchyTextListbox.GetNumItems();
		int remove_lines_count = 0;
		// Print(max_count);
		for (int i = row + 1; i < max_count; i++)
		{
			if (deep < deep_next && i <= max_count)
			{
				remove_lines_count = remove_lines_count + 1;
				m_ConfigHierarchyTextListbox.GetItemData(i, 0, config_params_next);
				deep_next = config_params_next.param5;
			}
		}

		// remove remove_lines_count lines from row
		// remove_lines_count = remove_lines_count - 1;
		for (i = 1; i < remove_lines_count; i++)
		{
			int x = row + 1;
			if (x < max_count)
			{
				m_ConfigHierarchyTextListbox.RemoveRow(x);
			}
		}

		string offset = "";
		for (i = 0; i < deep; i++)
		{
			offset = offset + "  ";
		}
		m_ConfigHierarchyTextListbox.SetItem(row, offset + "+ " + config_params.param2, new ConfigParamsEx(false, config_params.param2, 0, config_params.param4, deep,""), 0);
		/* //not sure why this is here, but it's causing issues when collapsing items in config viewer, disabling for now
		if (deep == 0)
		{
			ClearHierarchy();
		}
		*/
	}
	
	protected void RenderVariables(int row)
	{
		if (row > -1 && row < m_ConfigHierarchyTextListbox.GetNumItems())
		{
			string filter_lower = m_VariableConfigFilter.GetText();
			filter_lower.ToLower();
			
			TStringArray filters = new TStringArray();
			filter_lower.Split(" ", filters);
			
			ConfigParamsEx config_params;
			
			m_ConfigHierarchyTextListbox.GetItemData(row, 0, config_params);
			m_ConfigVariablesTextListbox.ClearItems();
			TStringArray variables;
			string path = config_params.param4;
			variables = m_ModuleConfigViewer.GetConfigVariables(path);

			for (int i = 0; i < variables.Count(); i++)
			{
				string var = variables.Get(i);
				
				if (filters.Count() == 0)
				{
					m_ConfigVariablesTextListbox.AddItem(var, NULL, 0);
				}
				else
				{
					foreach (string f: filters)
					{
						var.ToLower();
						if (var.Contains(f))
							m_ConfigVariablesTextListbox.AddItem(var, NULL, 0);
					}
				}
			}
		}
	}
	
}

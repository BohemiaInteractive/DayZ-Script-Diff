typedef string ResourceName;
class ResourceName : string
{
};

#ifdef WORKBENCH

typedef int DateTimeUtcAsInt;
class DateTimeUtcAsInt
{
	int GetSecond()
	{
		return value & 0x3f;
	}

	int GetMinute()
	{
		return (value >> 6) & 0x3f;
	}

	int GetHour()
	{
		return (value >> 12) & 0x1f;
	}

	int GetDay()
	{
		return (value >> 17) & 0x1f;
	}

	int GetMonth()
	{
		return (value >> 22) & 0xf;
	}

	int GetYear()
	{
		return ((value >> 26) & 0x3f) + 2000;
	}
}

class ButtonAttribute
{
	string m_Label;
	bool m_Focused;
	
	void ButtonAttribute(string label = "ScriptButton", bool focused = false)
	{
		m_Label = label;
		m_Focused = focused;
	}
};

/*!
Attribute for Workbench plugin definition:
- `name` - ui name in Script Tools menu
- `description` - tooltip
- `shortcut` - shortcut in simple text form e.g. "ctrl+g"
- `icon` - relative path to icon file (32x32 png)
- `wbModules` - list of strings representing Workbench modules where this tool should be avalaible (e.g. {"ResourceManager", "ScriptEditor"}). Leave null or empty array for any module.
- `resourceTypes` - list of resource types to offer this plugin in context menu (e.g. {"conf", "dds", "layout"})
*/
class WorkbenchPluginAttribute
{
	string m_Name;
	string m_Icon;
	string m_Shortcut;
	string m_Description;
	string m_Category;
	int m_AwesomeFontCode; //! https://fontawesome.com/cheatsheet/

	ref array<string> m_WBModules;
	ref array<string> m_ResourceTypes; 

	void WorkbenchPluginAttribute(string name, string description = "", string shortcut = "", string icon = "", array<string> wbModules = null, string category = "", int awesomeFontCode = 0, array<string> resourceTypes = null)
	{
		m_Name = name;
		m_Icon = icon;
		m_Shortcut = shortcut;
		m_Description = description;
		m_WBModules = wbModules;
		m_Category = category;
		m_AwesomeFontCode = awesomeFontCode;
		m_ResourceTypes = resourceTypes;
	}
}

//! Attribute for Workbench tool definition.
class WorkbenchToolAttribute: WorkbenchPluginAttribute
{
}

#endif
class CreditsLoader
{
	protected static const string JSON_FILE_PATH = "scripts/data/credits.json";
	
	static void CreateTestJson()
	{
		string nameDepartment	= "Department";
		string nameSection		= "Section";
		string nameLine			= "Line";
		
		JsonDataCredits data = new JsonDataCredits();
		data.Departments = new array<ref JsonDataCreditsDepartment>;
		
		for (int index_dep = 0; index_dep < 3; ++index_dep)
		{
			JsonDataCreditsDepartment department = new JsonDataCreditsDepartment();
			department.Sections = new array<ref JsonDataCreditsSection>();
			department.DepartmentName = (nameDepartment +" "+ index_dep);
			
			for (int index_sec = 0; index_sec < 4; ++index_sec)
			{
				JsonDataCreditsSection section = new JsonDataCreditsSection();
				section.SectionLines = new array<string>;
				section.SectionName = (nameSection +" "+ index_sec);
				
				int linesCount = Math.RandomInt(3, 10);
				for (int i = 0; i < linesCount; ++i)
				{
					section.SectionLines.Insert(nameLine +" "+ i);
				}	
				
				department.Sections.Insert(section);
			}
			
			data.Departments.Insert(department);
		}

		string errorMessage;
		if (!JsonFileLoader<ref JsonDataCredits>.SaveFile(JSON_FILE_PATH, data, errorMessage))
			ErrorEx(errorMessage);
	}
	
	static JsonDataCredits GetData()
	{
		string errorMessage;
		JsonDataCredits data;
		
		if (!JsonFileLoader<ref JsonDataCredits>.LoadFile(JSON_FILE_PATH, data, errorMessage))
			ErrorEx(errorMessage);
		
		return data;
	}
}

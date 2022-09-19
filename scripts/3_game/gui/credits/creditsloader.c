class CreditsLoader
{
	protected static const string JSON_FILE_PATH = "Scripts/Data/credits.json";
	
	static void CreateTestJson()
	{
		string test_name_department	= "Department";
		string test_name_setion		= "Section";
		string test_name_line		= "Line";
		
		JsonDataCredits data = new JsonDataCredits;
		data.Departments = new array<ref JsonDataCreditsDepartment>;
		
		for( int index_dep = 0; index_dep < 3; ++index_dep )
		{
			ref JsonDataCreditsDepartment data_department = new JsonDataCreditsDepartment;
			data_department.Sections = new array<ref JsonDataCreditsSection>;			
			data_department.DepartmentName = (test_name_department +" "+ index_dep);			
			
			for( int index_sec = 0; index_sec < 4; ++index_sec )
			{
				ref JsonDataCreditsSection data_section = new JsonDataCreditsSection;
				data_section.SectionLines = new array<string>;
				data_section.SectionName = (test_name_setion +" "+ index_sec);
				
				int lines_count = Math.RandomInt(3, 10);
				for( int i = 0; i < lines_count; ++i )
				{
					data_section.SectionLines.Insert( test_name_line +" "+ i );
				}	
				
				data_department.Sections.Insert( data_section );
			}
			
			data.Departments.Insert( data_department );
		}
		
		JsonFileLoader<ref JsonDataCredits>.JsonSaveFile( JSON_FILE_PATH, data );
	}
	
	static JsonDataCredits GetData()
	{
		ref JsonDataCredits data;
		JsonFileLoader<ref JsonDataCredits>.JsonLoadFile( JSON_FILE_PATH, data );
		
		return data;
	}
};


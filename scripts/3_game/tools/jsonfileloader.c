class JsonFileLoader<Class T>
{
	protected static const int READ_FILE_LENGTH = 100000000;
	
	protected static ref JsonSerializer m_Serializer = new JsonSerializer();
	
	static bool LoadFile(string filename, out T data, out string errorMessage)
	{
		if (FileExist(filename))
		{
			FileHandle handle = OpenFile(filename, FileMode.READ);
			if (handle == 0)
			{
				errorMessage = string.Format("Cannot open file \"%1\" for reading", filename);
				return false;
			}
			
			string fileContent;
			ReadFile(handle, fileContent, READ_FILE_LENGTH);

			CloseFile(handle);
			
			if (!m_Serializer)
				m_Serializer = new JsonSerializer();
			
			string error;
			if (!m_Serializer.ReadFromString(data, fileContent, error))
			{
				errorMessage = string.Format("Cannot load data from \"%1\":\n%2", filename, error);
				return false;
			}
			
			return true;
		}
		else
		{
			errorMessage = string.Format("File \"%1\" does not exist, check the provided path", filename, error);
			return false;
		}
	}
	
	static bool SaveFile(string filename, T data, out string errorMessage)
	{
		string fileContent;

		if (!m_Serializer)
			m_Serializer = new JsonSerializer();

		string makeDataError;
		if (!MakeData(data, fileContent, makeDataError, true))
		{
			errorMessage = string.Format("Cannot save data to file \"%1\", %2", filename, makeDataError);
			return false;
		}	

		FileHandle handle = OpenFile(filename, FileMode.WRITE);
		if (handle == 0)
		{
			errorMessage = string.Format("Cannot open file \"%1\" for writing", filename);
			return false;
		}

		FPrint(handle, fileContent);
		CloseFile(handle);

		return true;
	}
	
	static bool LoadData(string string_data, out T data, out string errorMessage)
	{
		string error;

		if (!m_Serializer)
			m_Serializer = new JsonSerializer();
		
		if (!m_Serializer.ReadFromString(data, string_data, error))
		{
			errorMessage = string.Format("Cannot load provided JSON data (deserialization error) - check syntax: %1", error);
			return false;
		}
		
		return true;
	}
	
	static bool MakeData(T inputData, out string outputData, out string errorMessage, bool prettyPrint = true)
	{
		if (!m_Serializer)
			m_Serializer = new JsonSerializer();
		
		if (!m_Serializer.WriteToString(inputData, prettyPrint, outputData))
		{
			errorMessage = "Cannot create JSON from provided data (serialization error)";
			return false;
		}
		
		return true;
	}
	
	//! DEPRECATED
	//! ----------

	//! #ifndef DIAG_DEVELOPER
	
	//! use JsonFileLoader::LoadFile instead
	static void JsonLoadFile(string filename, out T data)
	{
		
		if (FileExist(filename))
		{
			string file_content;
			string line_content;
			string error;
			
			FileHandle handle = OpenFile(filename, FileMode.READ);
			if (handle == 0)
				return;

			while (FGets(handle,  line_content) >= 0)
			{
				file_content += line_content;
			}
			
			CloseFile(handle);
			
			if (!m_Serializer)
				m_Serializer = new JsonSerializer();
			
			if (!m_Serializer.ReadFromString(data, file_content, error))
				ErrorEx(string.Format("Cannot load data from \"%1\":\n%2", filename, error));
		}
	}

	//! use JsonFileLoader::SaveFile instead
	static void JsonSaveFile(string filename, T data)
	{
		string file_content;
		if (!m_Serializer)
			m_Serializer = new JsonSerializer();
		
		m_Serializer.WriteToString(data, true, file_content);
		
		FileHandle handle = OpenFile(filename, FileMode.WRITE);
		if (handle == 0)
			return;
		
		FPrint(handle, file_content);
		CloseFile(handle);
	}

	//!use JsonFileLoader::LoadData instead
	static void JsonLoadData(string string_data, out T data)
	{
		string error;
		if (!m_Serializer)
			m_Serializer = new JsonSerializer();
		
		if (!m_Serializer.ReadFromString(data, string_data, error))
			ErrorEx(string.Format("Cannot load data %1", error));
	}

	//!use JsonFileLoader::MakeData instead
	static string JsonMakeData(T data)
	{
		string string_data;
		if (!m_Serializer)
			m_Serializer = new JsonSerializer();
		
		m_Serializer.WriteToString(data, true, string_data);
		return string_data;
	}

	//#endif
}

class UUIDApi
{
	/**
	\brief Creates a 128bit UUID using system APIs
	@code
		int uuid[4];
		UUIDApi.Generate(uuid);
	@endcode
	 */
	static proto void Generate(inout int id[4]);

	/**
	\brief Output the UUID as standard format string (eg. 123e4567-e89b-12d3-a456-426655440000)
	@code
		int uuid[4];
		Print(UUIDApi.FormatString(uuid));
	
		>> "123e4567-e89b-12d3-a456-426655440000"
	@endcode
	 */
	static proto string FormatString(int id[4]);

};

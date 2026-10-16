class DoOnce
{
    protected static ref map<string, bool> m_Done;

    protected static void Ensure()
    {
        if (!m_Done)
            m_Done = new map<string, bool>();
    }

    // Returns true only on the first successful execution during this runtime.
    static bool Run(string key, Class target, string fnName, Param params = null)
    {
        Ensure();

        if (!key || !target || !fnName)
            return false;

        if (m_Done.Contains(key))
            return false;

        m_Done.Set(key, true);

        // Call any method on any object by name
        bool called = g_Game.GameScript.CallFunctionParams(target, fnName, null, params);

        return called;
    }

    static bool HasRun(string key)
    {
        Ensure();

        return m_Done.Contains(key);
    }
	
	static void Clear(string key)
	{
		Ensure();

		if (m_Done.Contains(key))
			m_Done.Remove(key);
	}
}
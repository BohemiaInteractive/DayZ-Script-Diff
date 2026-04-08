class SimpleCircularBuffer<Class T>
{
	private int m_Pointer 			= 0;
	private int m_Size 				= 0;
	private ref array<T> m_Samples 	= new array<T>();

	/**
	\brief Initialize Simple Circular Buffer Cyclic Buffer
		\param pSize \p size of the buffer
		\param pDefaultValue \p initial value stored in buffer
	*/
	void SimpleCircularBuffer(int pSize, T pDefaultValue)
	{
		m_Size = pSize;

		for (int i = 0; i < m_Size; ++i)
			m_Samples.Insert(pDefaultValue);
	}

	/**
	\brief Add new value to buffer
		\param newSample \p value that will be added to buffer
		\return average value from the buffer
	@code
		ref SimpleCircularBuffer<float> m_Buffer = SimpleCircularBuffer<float>(2, -1);
		m_Buffer.Add(0.1);
	@endcode
	*/
	void Add(T newSample)
    {
        m_Samples[m_Pointer++] = newSample;
        if (m_Pointer == m_Size)
            m_Pointer = 0;
    }

	/**
	\brief Returns value from given index
		\param pIndex \p size of the buffer
	*/
	T Get(int pIndex)
	{
		return m_Samples[pIndex];
	}

	/**
	\brief Returns array of values stored in buffer
		\param pIndex \p size of the buffer
	*/
	array<T> GetValues()
	{
		return m_Samples;
	}
}
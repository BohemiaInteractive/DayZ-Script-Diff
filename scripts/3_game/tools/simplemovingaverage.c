class SimpleMovingAverage<Class T>
{
	private T m_Sum 				= 0;
	private int m_Pointer 			= 0;
	private int m_Size 				= 0;
	private ref array<T> m_Samples 	= new array<T>();

	/**
	\brief Initialize Simple Moving Average Cyclic Buffer
		\param pSize \p size of the buffer
		\param pDefaultValue \p initial value stored in buffer
	*/
	void SimpleMovingAverage(int pSize, T pDefaultValue = 0)
	{
		m_Size = pSize;

		for (int i = 0; i < m_Size; i++)
		{
			m_Samples.Insert(pDefaultValue);
        	m_Sum += m_Samples[i];
		}
	}

	/**
	\brief Add new value and return average value from buffer
		\param newSample \p value that will be added to buffer
		\return average value from the buffer
	@code
		ref SimpleMovingAverage<float> m_Buffer = SimpleMovingAverage<float>(10, 0.5);
		float sma = m_Buffer.Add(0.2);
	@endcode
	*/
	T Add(T newSample)
    {
        m_Sum = m_Sum - m_Samples[m_Pointer] + newSample;
        m_Samples[m_Pointer++] = newSample;
        if (m_Pointer == m_Size)
		{
            m_Pointer = 0;
		}

        return m_Sum / m_Size;
    }
}
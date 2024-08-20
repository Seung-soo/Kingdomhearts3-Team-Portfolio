#pragma once

namespace Engine
{
	class CTagFinder
	{
	public:
		CTagFinder(const wchar_t* pTag) : m_pTag(pTag) {}
		~CTagFinder() = default;

	public:
		template <typename T>
		bool operator() (T& Pair)
		{
			if (0 == lstrcmp(Pair.first, m_pTag))
				return true;
			return false;
		}
	private:
		const wchar_t*		m_pTag = nullptr;
	};

	class CTagCharFinder
	{
	public:
		CTagCharFinder(const char* pTag) : m_pTag(pTag) {}
		~CTagCharFinder() = default;

	public:
		template <typename T>
		bool operator() (T& Pair)
		{
			if (0 == strcmp(Pair.first, m_pTag))
				return true;
			return false;
		}
	private:
		const char*		m_pTag = nullptr;
	};

	template <typename T>
	void Safe_Delete(T& pPoint)
	{
		if (nullptr != pPoint)
		{
			delete pPoint;
			pPoint = nullptr;
		}
	}

	template <typename T>
	void Safe_Delete_Array(T& pPoint)
	{
		if (nullptr != pPoint)
		{
			delete[] pPoint;
			pPoint = nullptr;
		}
	}

	template <typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
			dwRefCnt = pInstance->AddRef();

		return dwRefCnt;
	}

	template <typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	template <typename T>
	_bool	DepthSort(T Dest, T Src)
	{
		return Dest->Get_Depth() > Src->Get_Depth();
	}

	template <typename T>
	_float4	XMStoreFloat4RT(T vVector)
	{
		_float4 vFloat4RT;
		XMStoreFloat4(&vFloat4RT, vVector);
		return vFloat4RT;
	}

	template <typename T>
	_float3	XMStoreFloat3RT(T vVector)
	{
		_float3 vFloat3RT;
		XMStoreFloat3(&vFloat3RT, vVector);
		return vFloat3RT;
	}

	template <typename T>
	_float4x4	XMStoreFloat4x4RT(T mat)
	{
		_float4x4 vFloat4x4RT;
		XMStoreFloat4x4(&vFloat4x4RT, mat);
		return vFloat4x4RT;
	}

	template <typename T>
	inline T LinearInterpolation(T from, T to, float cp) // cp = 0 ~ 1
	{
		// return from * (1.0 - cp) + to * cp;
		return from + (to - from) * cp;
	}

	template <typename T>
	inline T BezierCurve(T from, T via, T to, float cp)
	{
		T t1 = LinearInterpolation(from, via, cp);
		T t2 = LinearInterpolation(via, to, cp);

		return LinearInterpolation(t1, t2, cp);
	}

	template <typename T>
	_float RandomRange(T r1, T r2)
	{
		T random = ((T)rand()) / (T)RAND_MAX;
		T diff = r2 - r1;
		T val = random * diff;

		return r1 + val;
	}
}
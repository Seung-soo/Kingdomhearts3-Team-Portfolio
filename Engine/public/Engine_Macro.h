#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define D3D11COLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))


#define GET_INSTANCE(CLASSNAME)	[](){					\
	CLASSNAME* pInstance = CLASSNAME::GetInstance();	\
	return pInstance;									\
	}();

#define BEGIN(NAMESPACE)	namespace NAMESPACE {
#define END					}

#define MSGBOX(MESSAGE)	MessageBox(0, TEXT(MESSAGE), TEXT("System Message"), MB_OK);

#define NO_COPY(CLASSNAME)										\
		private:												\
		CLASSNAME(const CLASSNAME&);							\
		CLASSNAME& operator = (const CLASSNAME&);				

#define MAKE_SINGLETON(CLASSNAME)							\
		NO_COPY(CLASSNAME)										\
		private:												\
		static CLASSNAME*	m_pInstance;						\
		public:													\
		static CLASSNAME*	GetInstance( void );				\
		static unsigned long DestroyInstance( void );					

#define READY_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = NULL;				\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == m_pInstance) {						\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned long CLASSNAME::DestroyInstance( void ) {		\
			unsigned long dwRefCnt = 0;							\
			if(nullptr != m_pInstance)	{						\
				dwRefCnt = m_pInstance->Release();				\
				if(0 == dwRefCnt)								\
				m_pInstance = nullptr;							\
			}													\
			return dwRefCnt;									\
		}

#define	RETURN_FAIL(FUNCTION)									\
		FUNCTION												\
		{														\
			MSGBOX("FAIL");										\
			__debugbreak();										\
			return E_FAIL;										\
		}

#define	RETURN_NULL(FUNCTION)									\
		FUNCTION												\
		{														\
			MSGBOX("FAIL");										\
			__debugbreak();										\
			return nullptr;										\
		}

#define	RETURN_NEGATIVE(FUNCTION)								\
		FUNCTION												\
		{														\
			MSGBOX("FAIL");										\
			__debugbreak();										\
			return -1;											\
		}

#define	RETURN_FALSE(FUNCTION)									\
		FUNCTION												\
		{														\
			MSGBOX("FAIL");										\
			__debugbreak();										\
			return false;										\
		}

#define	CHECK_FAILED(FUNCTION)	if (FAILED(FUNCTION))			\
		{														\
			MSGBOX("FAIL");										\
			__debugbreak();										\
			return E_FAIL;										\
		}



#define	CHECK_NULL(VARIABLE)	if (nullptr == VARIABLE)		\
		{														\
			MSGBOX("FAIL");										\
			__debugbreak();										\
			return E_FAIL;										\
		}

#define BREAKPOINT	MSGBOX("FAIL"); __debugbreak()
#define BREAKPOINTEFAIL


#define	RESULT_ERROR				-1				// 에러났을 때
#define	RESULT_NOPROBLEM			0				// 문제없을 때
#define	RESULT_DEAD					1				// 오브젝트가 죽었을 때
#define RESULT_POOL					2				// 오브젝트 풀로 되돌릴 때



#define vecx(vector)				vector.m128_f32[0]
#define vecy(vector)				vector.m128_f32[1]
#define vecz(vector)				vector.m128_f32[2]
#define vecw(vector)				vector.m128_f32[3]

#define __vector(x, y, z, w)		XMVectorSet(x, y, z, w)

#define _axisX						XMVectorSet(1.f, 0.f, 0.f, 0.f)
#define _axisY						XMVectorSet(0.f, 1.f, 0.f, 0.f)
#define _axisZ						XMVectorSet(0.f, 0.f, 1.f, 0.f)
#define _zeroPoint					XMVectorSet(0.f, 0.f, 0.f, 1.f)

#define _axisX4						_float4(1.f, 0.f, 0.f, 0.f)
#define _axisY4						_float4(0.f, 1.f, 0.f, 0.f)
#define _axisZ4						_float4(0.f, 0.f, 1.f, 0.f)
#define _zeroPoint4					_float4(0.f, 0.f, 0.f, 1.f)

#define DISTANCE(VECTOR)			XMVectorGetX(XMVector4Length(VECTOR))
#define DIRECTIONAL(Vec1, Vec2)		XMVector4Normalize(Vec1 - Vec2)

#define DOT3(vector1, vector2)		XMVectorGetX(XMVector3Dot(vector1, vector2))
#define CROSS3(vector1, vector2)	XMVector3Cross(vector1, vector2)

#define DOT4(vector1, vector2)		XMVectorGetX(XMVector4Dot(vector1, vector2))
#define CROSS4(vector1, vector2)	XMVector4Cross(vector1, vector2)

#define VecNLZ3(float3)				XMVector3Normalize(XMLoadFloat3(&float3))
#define VecNLZ4(float4)				XMVector3Normalize(XMLoadFloat4(&float4))



// Color //

// = _float4
#define _red4						_float4(1.f, 0.f, 0.f, 1.f)
#define _green4						_float4(0.f, 1.f, 0.f, 1.f)
#define _blue4						_float4(0.f, 0.f, 1.f, 1.f)

#define _white4						_float4(1.f, 1.f, 1.f, 1.f)
#define _black4						_float4(0.f, 0.f, 0.f, 1.f)

#define _vsgray4					_float4(0.3f, 0.3f, 0.3f, 1.f)

#define _cyan4						_float4(0.f, 1.f, 1.f, 1.f)
#define _magenta4					_float4(1.f, 0.f, 1.f, 1.f)
#define _yellow4					_float4(1.f, 1.f, 0.f, 1.f)

#define _hotpink4					_float4(1.f, 0.f, 0.5f, 1.f)
#define _lightpink4					_float4(1.f, 0.6796f, 0.7851f, 1.f)
#define _darkred4					_float4(0.5f, 0.f, 0.f, 1.f)
#define _mildred4					_float4(1.f, 0.1f, 0.1f, 1.f)

#define _lightblue4					_float4(0.5976f, 0.8476f, 0.9140f, 1.f)
#define _mildblue4					_float4(0.f, 0.5f, 1.f, 1.f)

// = _vector
#define _redV						XMVectorSet(1.f, 0.f, 0.f, 1.f)
#define _greenV						XMVectorSet(0.f, 1.f, 0.f, 1.f)
#define _blueV						XMVectorSet(0.f, 0.f, 1.f, 1.f)

#define _whiteV						XMVectorSet(1.f, 1.f, 1.f, 1.f)
#define _blackV						XMVectorSet(0.f, 0.f, 0.f, 1.f)

#define _vsgrayV					XMVectorSet(0.3f, 0.3f, 0.3f, 1.f)

#define _cyanV						XMVectorSet(0.f, 1.f, 1.f, 1.f)
#define _magentaV					XMVectorSet(1.f, 0.f, 1.f, 1.f)
#define _yellowV					XMVectorSet(1.f, 1.f, 0.f, 1.f)

#define _hotpinkV					XMVectorSet(1.f, 0.f, 0.5f, 1.f)
#define _lightpinkV					XMVectorSet(1.f, 0.6796f, 0.7851f, 1.f)
#define _darkredV					XMVectorSet(0.5f, 0.f, 0.f, 1.f)
#define _mildredV					XMVectorSet(1.f, 0.1f, 0.1f, 1.f)

#define _lightblueV					XMVectorSet(0.5976f, 0.8476f, 0.9140f, 1.f)
#define _mildblueV					XMVectorSet(0.f, 0.5f, 1.f, 1.f)

// Si's Personal //
#define _vec3norm(vector)			XMVector3Normalize(vector)
#define _breakpoint					\
{									\
	MSGBOX("FAIL");					\
	__debugbreak(); }

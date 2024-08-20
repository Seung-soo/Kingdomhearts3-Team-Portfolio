#pragma once

namespace Engine
{
	typedef unsigned char				_ubyte;
	typedef signed char					_byte;

	typedef unsigned short				_ushort;
	typedef signed short				_short;

	typedef unsigned int				_uint;
	typedef signed int					_int;

	typedef unsigned long				_ulong;
	typedef signed long					_long;

	typedef float						_float;
	typedef double						_double;

	typedef bool						_bool;

	typedef wchar_t						_tchar;

	typedef XMFLOAT2					_float2;

	typedef struct tagFloat3_Derived final : public XMFLOAT3
	{
		tagFloat3_Derived() = default;
		tagFloat3_Derived(_float fX) :
			XMFLOAT3(fX, fX, fX) {}
		tagFloat3_Derived(_float fX, _float fY, _float fZ) :
			XMFLOAT3(fX, fY, fZ) {}
		tagFloat3_Derived operator+(tagFloat3_Derived& _rFloat3)
		{
			tagFloat3_Derived Temp;
			XMStoreFloat3(&Temp, XMLoadFloat3(this) + XMLoadFloat3(&_rFloat3));
			return Temp;
		}
		tagFloat3_Derived operator-(tagFloat3_Derived& _rFloat3)
		{
			tagFloat3_Derived Temp;
			XMStoreFloat3(&Temp, XMLoadFloat3(this) - XMLoadFloat3(&_rFloat3));
			return Temp;
		}
		tagFloat3_Derived operator*(tagFloat3_Derived& _rFloat3)
		{
			tagFloat3_Derived Temp;
			XMStoreFloat3(&Temp, XMLoadFloat3(this) * XMLoadFloat3(&_rFloat3));
			return Temp;
		}
		tagFloat3_Derived operator=(XMFLOAT3& _rFloat3)
		{
			tagFloat3_Derived Temp;
			XMVECTOR vTemp = XMLoadFloat3(&_rFloat3);
			XMStoreFloat3(&Temp, vTemp);
			return Temp;
		}
	}_float3;

	typedef struct tagFloat4_Derived final : public XMFLOAT4
	{
		tagFloat4_Derived() = default;
		tagFloat4_Derived(_float fX) :
			XMFLOAT4(fX, fX, fX, fX) {}
		tagFloat4_Derived(_float3 vFloat3, _float fW) :
			XMFLOAT4(vFloat3.x, vFloat3.y, vFloat3.z, fW) {}
		tagFloat4_Derived(_float fX, _float fY, _float fZ, _float fW) :
			XMFLOAT4(fX, fY, fZ, fW) {}
		tagFloat4_Derived operator+(tagFloat4_Derived& _rFloat4)
		{
			tagFloat4_Derived Temp;
			XMStoreFloat4(&Temp, XMLoadFloat4(this) + XMLoadFloat4(&_rFloat4));
			return Temp;
		}
		tagFloat4_Derived operator-(tagFloat4_Derived& _rFloat4)
		{
			tagFloat4_Derived Temp;
			XMStoreFloat4(&Temp, XMLoadFloat4(this) - XMLoadFloat4(&_rFloat4));
			return Temp;
		}
		tagFloat4_Derived operator*(tagFloat4_Derived& _rFloat4)
		{
			tagFloat4_Derived Temp;
			XMStoreFloat4(&Temp, XMLoadFloat4(this) * XMLoadFloat4(&_rFloat4));
			return Temp;
		}
		tagFloat4_Derived operator=(XMFLOAT4& _rFloat4)
		{
			tagFloat4_Derived Temp;
			XMVECTOR vTemp = XMLoadFloat4(&_rFloat4);
			XMStoreFloat4(&Temp, vTemp);
			return Temp;
		}
	}_float4;

	typedef XMVECTOR					_vector;
	typedef FXMVECTOR					_fvector;

	typedef XMFLOAT4X4					_float4x4;

	typedef XMMATRIX					_matrix;
	typedef FXMMATRIX					_fmatrix;
	typedef CXMMATRIX					_cmatrix;

	typedef SimpleMath::Ray _Ray;


}
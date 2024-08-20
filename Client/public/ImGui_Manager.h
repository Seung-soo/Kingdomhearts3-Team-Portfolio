#pragma once

#undef new

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#undef new

#include "Base.h"
#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Engine)
class CShader;
class CTransform;
class CModel;
class CCollider;
class CStateMachine;
class CRenderer;
class CMonster;
END

BEGIN(Client)

class CImGui_Manager final : public CBase
{
	MAKE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public: /* MainApp ���� ���ƾ� �� �⺻���� �Լ���*/
	void ImGui_SetUp();
	void ImGui_Start();
	void ImGui_Tick();
	void ImGui_Render();

public: /* ���̾ƿ� �ٹ̱� ���� �Լ��� */
	void SmallSeparator();	// �� ���� �ø���(�۰�)
	void BigSeparator();	// �� ���� �ø���(ũ��)
	void ColoredText(_float3 color, const char* text);	// �ؽ�Ʈ �� �ֱ�
	void ColoredButtonBegin(int r = 180, int g = 100, int b = 0);	// ��ư �� �ֱ� ����
	void ColoredButtonEnd();										// ��ư �� �ֱ� ����
public:
	void	SetGameObjectPosition(string Tag, CTransform* pTransform, _float fMin = -1000.f, _float fMax = 1000.f);
	void	SetGameObjectScale(string Tag, CTransform* pTransform, _float fMin = 0.1f, _float fMax = 10.f);
	void	SetGameObjectRotation(string Tag, CTransform* pTransform);
	void	SetGameObjectAnimation(CModel* pModel);
	void	SetGameObjectShaderPass(string Tag, CShader* pShader, _int* iCurPass);
	void	SetGameObjectCollider(string Tag, CCollider* pCollider);
	void	CheckMonsterState(string Tag, CStateMachine* pStateMachine);
	void	GetDistanceFromMouse(CTransform* pTransform);
		
public: // For LUT
	void Set_LUTTexIndex(CRenderer* pRenderer);

private: /* ���������� ����� �Լ��� */
	void	OnOffBasicWindow();
	void	ShowSystemInfo();
	void	SetGameSpeed();
	void	SelectLevel();
	void	CheckWindowCheckbox();
	void	ShowGameObjectImGuiController(); // ���ӿ�����Ʈ���� ImGui ��Ʈ�ѷ��� ��Ʈ��
	void	SetFrustumRange(); // ��ü�� ����ü Range ����
	void	AssimpLoad();
	void	MouseLock();
	void	PauseSound();
	void	SetWeaponDamageMax();

#ifdef _DEBUG
	void	OnOffDebugRender();
#endif

private: /* ������� */
	ID3D11Device*			m_pDevice        = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	CGameInstance*			m_pGameInstance  = nullptr;

	const char* m_levels[LEVEL_END + 1] = {
		"LEVEL_STATIC", "LEVEL_LOADING", "LEVEL_LOGO",
		"LEVEL_Twilight", "Level_Rapunzel", "Level_Rapunzel_Boss", "Level_Frozen", "Level_KeyGrave", "Level_FinalBoss",
		"(M)LEVEL_MAPTOOL", "(E)LEVEL_EFFECTTOOL", "(C)LEVEL_COLLIDERTOOL", "(T)LEVEL_TEST",
		"LEVEL_END"
	};
	_uint m_iCurLevelIndex = 0;

	_bool m_bMouseLock     = false;
	_bool m_bIsMouseLocked = false;
	_bool m_bIgnoreNavi    = false;
	_bool m_bPauseSound    = false;

public:
	_bool show_basic_winodw            = true;
	_bool show_demo_window             = false;
	_bool show_levelcontrol_window     = false;
	_bool show_maptool_window          = true;
	_bool show_terraintool_window	   = false;
	_bool show_cameratool_window       = false;
	_bool show_monstergenerator_window = true;
	_bool show_objectplacetool_window  = false;
	_bool show_mapcollider             = false;

public:
	void virtual Free() override;
};

END
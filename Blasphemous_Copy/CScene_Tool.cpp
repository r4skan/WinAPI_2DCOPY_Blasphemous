#include "framework.h"
#include "CScene_Tool.h"
#include "CTile.h"
#include "resource.h"
#include "CScene.h"
#include "CD2DImage.h"
#include "commdlg.h"
#include "CUI.h"
#include "CPanelUI.h"
#include "CButtonUI.h"
#include "CTileButton.h"
#include "CPrayerTable.h"
#include <fstream>
#include "json/json.h"

INT_PTR CALLBACK TileWinProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK TileInfoWinProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

CScene_Tool::CScene_Tool()
{
	m_pMap = nullptr;

	m_hWnd = 0;
	m_iIdx = 0;
	m_hTileInfoWnd = 0;
	m_iInfoWndCount = 0;
	m_gTile = GROUP_TILE::NONE;
	m_velocity = 500;
	m_iTileX = 0;
	m_iTileY = 0;
	m_fptSelectedPos = {};
}

CScene_Tool::~CScene_Tool()
{
	vector<CGameObject*>& vecObj = GetObjGroup(GROUP_GAMEOBJ::PLAYER);
	if (vecObj.size() > 0)
		vecObj.erase(vecObj.begin());
}

void CScene_Tool::update()
{
	CScene::update();

	if (PRESS_KEY_DOWN(VK_TAB))
	{
		ChangeToNextScene(GROUP_SCENE::CHURCH);
	}

	if (PRESS_KEY('A'))
	{
		CCameraManager::GetInst()->Scroll(fVector2D(-1, 0), m_velocity);
	}
	if (PRESS_KEY('D'))
	{
		CCameraManager::GetInst()->Scroll(fVector2D(1, 0), m_velocity);
	}
	if (PRESS_KEY('W'))
	{
		CCameraManager::GetInst()->Scroll(fVector2D(0, -1), m_velocity);
	}
	if (PRESS_KEY('S'))
	{
		CCameraManager::GetInst()->Scroll(fVector2D(0, 1), m_velocity);
	}

	SetTileIdx();
	SetTileGroup();
}

void CScene_Tool::render()
{
	const vector<CGameObject*>& vecTile = GetObjGroup(GROUP_GAMEOBJ::TILE);
	for (UINT i = 0; i < vecTile.size(); i++)
	{
		vecTile[i]->render();
	}

	PrintMap();
	//PrintMap(fPoint(1, 1));
	PrintTileLine();
	PrintTileGroup();

	const vector<CGameObject*>& vecUI = GetObjGroup(GROUP_GAMEOBJ::UI);
	for (UINT i = 0; i < vecUI.size(); i++)
	{
		vecUI[i]->render();
	}
}

void CScene_Tool::Enter()
{
	m_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_TILEBOX), hWnd, TileWinProc);
	ShowWindow(m_hWnd, SW_SHOW);

	CreateTile(20, 20);
	CreateTilePanel();
	//CreateObjPanel();

	CCameraManager::GetInst()->SetLookAt(fPoint(WINSIZE_X / 2.f, WINSIZE_Y / 2.f));
	CCameraManager::GetInst()->FollowTargetObj(nullptr, false, false);
	CCameraManager::GetInst()->SetBoundary(false);

	CUIManager::GetInst()->SetKeyControl(false);
}

void CScene_Tool::Exit()
{
	EndDialog(m_hWnd, IDOK);
	ClearAll();
}

void CScene_Tool::SetIdx(UINT idx)
{
	m_iIdx = idx;
}

void CScene_Tool::SetTileIdx()
{
	if (PRESS_KEY(VK_LBUTTON) || PRESS_KEY(VK_RBUTTON))
	{
		fPoint fptMousePos = MousePos();
		fptMousePos = CCameraManager::GetInst()->GetRealPos(fptMousePos);

		int iTileX = m_iTileX;
		int iTileY = m_iTileY;

		int iCol = (int)fptMousePos.x / CTile::SIZE_TILE;
		int iRow = (int)fptMousePos.y / CTile::SIZE_TILE;

		if (fptMousePos.x < 0.f || iTileX <= iCol ||
			fptMousePos.y < 0.f || iTileY <= iRow)
		{
			return;		// 타일이 없는 위치 무시
		}

		UINT iIdx = iRow * iTileX + iCol;
		m_fptSelectedPos = fPoint((float)fptMousePos.x, (float)fptMousePos.y);
		const vector<CGameObject*>& vecTile = GetObjGroup(GROUP_GAMEOBJ::TILE);
		if (PRESS_KEY(VK_LBUTTON) && !CUIManager::GetInst()->IsFocused())
			((CTile*)vecTile[iIdx])->SetImgIdx(m_iIdx);
		else if (PRESS_KEY(VK_RBUTTON))
			((CTile*)vecTile[iIdx])->SetImgIdx(0);
	}
}

void CScene_Tool::SetGroup(GROUP_TILE group)
{
	m_gTile = group;
}

void CScene_Tool::SetTileGroup()
{
	if (PRESS_KEY(VK_LBUTTON) || PRESS_KEY(VK_RBUTTON))
	{
		fPoint fptMousePos = MousePos();
		fptMousePos = CCameraManager::GetInst()->GetRealPos(fptMousePos);

		int iTileX = m_iTileX;
		int iTileY = m_iTileY;

		int iCol = (int)fptMousePos.x / CTile::SIZE_TILE;
		int iRow = (int)fptMousePos.y / CTile::SIZE_TILE;

		if (fptMousePos.x < 0.f || iTileX <= iCol ||
			fptMousePos.y < 0.f || iTileY <= iRow)
		{
			return;		// 타일이 없는 위치 무시
		}

		UINT iIdx = iRow * iTileX + iCol;
		const vector<CGameObject*>& vecTile = GetObjGroup(GROUP_GAMEOBJ::TILE);
		if (PRESS_KEY(VK_LBUTTON && !CUIManager::GetInst()->IsFocused()))
		{
			((CTile*)vecTile[iIdx])->SetGroup(m_gTile);
			
			if (m_iInfoWndCount == 0 && m_gTile == GROUP_TILE::SPAWNPOINT)
			{
				m_hTileInfoWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_SETTILEINFO), m_hTileInfoWnd, TileInfoWinProc);
				ShowWindow(m_hTileInfoWnd, SW_SHOW);
				m_iInfoWndCount++;
			}
		}
		else if (PRESS_KEY(VK_RBUTTON))
			((CTile*)vecTile[iIdx])->SetGroup(GROUP_TILE::NONE);
	}
}

void CScene_Tool::CreateTile(UINT xSize, UINT ySize)
{
	ClearGroup(GROUP_GAMEOBJ::TILE);

	m_iTileX = xSize;
	m_iTileY = ySize;

	CD2DImage* pImg = CResourceManager::GetInst()->LoadD2DImage(L"Tile", L"texture\\Map\\Tileset\\tilemap.bmp");

	for (UINT y = 0; y < ySize; y++)
	{
		for (UINT x = 0; x < xSize; x++)
		{
			CTile* pTile = new CTile();
			pTile->SetPos(fPoint((float)(x * CTile::SIZE_TILE), (float)(y * CTile::SIZE_TILE)));
			pTile->SetX(x);
			pTile->SetY(y);
			pTile->SetD2DImage(pImg);
			AddObject(pTile, GROUP_GAMEOBJ::TILE);
		}
	}
}

void CScene_Tool::SaveTile(const wstring& strPath)
{
	FILE* pFile = nullptr;

	_wfopen_s(&pFile, strPath.c_str(), L"wb");		// w : write, b : binary
	assert(pFile);

	UINT xCount = m_iTileX;
	UINT yCount = m_iTileY;
	UINT tileCount = 0;

	const vector<CGameObject*>& vecTile = GetObjGroup(GROUP_GAMEOBJ::TILE);

	for (UINT i = 0; i < vecTile.size(); i++)
	{
		CTile* pTile = (CTile*)vecTile[i];
		if (0 != pTile->GetIdx() || GROUP_TILE::NONE != pTile->GetGroup())
			tileCount++;
	}

	fwrite(&xCount, sizeof(UINT), 1, pFile);
	fwrite(&yCount, sizeof(UINT), 1, pFile);
	fwrite(&tileCount, sizeof(UINT), 1, pFile);

	for (UINT i = 0; i < vecTile.size(); i++)
	{
		CTile* pTile = (CTile*)vecTile[i];
		if (0 != pTile->GetIdx() || GROUP_TILE::NONE != pTile->GetGroup())
			((CTile*)vecTile[i])->Save(pFile);
	}

	fclose(pFile);
}

void CScene_Tool::LoadTile(const wstring& strPath)
{
	ClearGroup(GROUP_GAMEOBJ::TILE);

	FILE* pFile = nullptr;

	_wfopen_s(&pFile, strPath.c_str(), L"rb");      // w : write, b : binary
	assert(pFile);

	UINT xCount = 0;
	UINT yCount = 0;
	UINT tileCount = 0;

	fread(&xCount, sizeof(UINT), 1, pFile);
	fread(&yCount, sizeof(UINT), 1, pFile);
	fread(&tileCount, sizeof(UINT), 1, pFile);

	CreateTile(xCount, yCount);

	const vector<CGameObject*>& vecTile = GetObjGroup(GROUP_GAMEOBJ::TILE);
	CD2DImage* pImg = CResourceManager::GetInst()->LoadD2DImage(L"Tile", L"texture\\tile\\tilemap.bmp");
	CTile* pTile = new CTile;

	for (UINT i = 0; i < tileCount; i++)
	{
		pTile->Load(pFile);
		UINT iIdx = pTile->GetY() * xCount + pTile->GetX();
		((CTile*)vecTile[iIdx])->SetX(pTile->GetX());
		((CTile*)vecTile[iIdx])->SetY(pTile->GetY());
		((CTile*)vecTile[iIdx])->SetImgIdx(pTile->GetIdx());
		((CTile*)vecTile[iIdx])->SetGroup(pTile->GetGroup());
	}

	fclose(pFile);
	delete pTile;
}

void CScene_Tool::SaveTileData()
{
	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(OPENFILENAME);  // 구조체 사이즈.
	ofn.hwndOwner = hWnd;					// 부모 윈도우 지정.
	wchar_t szName[256] = {};
	ofn.lpstrFile = szName; // 나중에 완성된 경로가 채워질 버퍼 지정.
	ofn.nMaxFile = sizeof(szName); // lpstrFile에 지정된 버퍼의 문자 수.
	ofn.lpstrFilter = L"ALL\0*.*\0tile\0*.tile"; // 필터 설정
	ofn.nFilterIndex = 0; // 기본 필터 세팅. 0는 all로 초기 세팅됨. 처음꺼.
	ofn.lpstrFileTitle = nullptr; // 타이틀 바
	ofn.nMaxFileTitle = 0; // 타이틀 바 문자열 크기. nullptr이면 0.
	wstring strTileFolder = CPathManager::GetInst()->GetContentPath();
	strTileFolder += L"tile";
	ofn.lpstrInitialDir = strTileFolder.c_str(); // 초기경로. 우리는 타일 저장할거기 때문에, content->tile 경로로 해두자.
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // 스타일

	if (GetSaveFileName(&ofn))
	{
		SaveTile(szName);
	}
}

void CScene_Tool::LoadTileData()
{
	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(OPENFILENAME);  // 구조체 사이즈.
	ofn.hwndOwner = hWnd; // 부모 윈도우 지정.
	wchar_t szName[256] = {};
	ofn.lpstrFile = szName; // 나중에 완성된 경로가 채워질 버퍼 지정.
	ofn.nMaxFile = sizeof(szName); // lpstrFile에 지정된 버퍼의 문자 수.
	ofn.lpstrFilter = L"ALL\0*.*\0tile\0*.tile"; // 필터 설정
	ofn.nFilterIndex = 0; // 기본 필터 세팅. 0는 all로 초기 세팅됨. 처음꺼.
	ofn.lpstrFileTitle = nullptr; // 타이틀 바
	ofn.nMaxFileTitle = 0; // 타이틀 바 문자열 크기. nullptr이면 0.
	wstring strTileFolder = CPathManager::GetInst()->GetContentPath();
	strTileFolder += L"tile";
	ofn.lpstrInitialDir = strTileFolder.c_str(); // 초기경로. 우리는 타일 저장할거기 때문에, content->tile 경로로 해두자.
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // 스타일

	if (GetOpenFileName(&ofn))
	{
		LoadTile(szName);
	}
}

void CScene_Tool::LoadMap()
{
	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(OPENFILENAME);  // 구조체 사이즈.
	ofn.hwndOwner = hWnd; // 부모 윈도우 지정.
	wchar_t szName[256] = {};
	ofn.lpstrFile = szName; // 나중에 완성된 경로가 채워질 버퍼 지정.
	ofn.nMaxFile = sizeof(szName); // lpstrFile에 지정된 버퍼의 문자 수.
	ofn.lpstrFilter = L"ALL\0*.*\0Image\0*.png"; // 필터 설정
	ofn.nFilterIndex = 0; // 기본 필터 세팅. 0는 all로 초기 세팅됨. 처음꺼.
	ofn.lpstrFileTitle = nullptr; // 타이틀 바
	ofn.nMaxFileTitle = 0; // 타이틀 바 문자열 크기. nullptr이면 0.
	wstring strTileFolder = CPathManager::GetInst()->GetContentPath();
	strTileFolder += L"map";
	ofn.lpstrInitialDir = strTileFolder.c_str(); // 초기경로. 우리는 타일 저장할거기 때문에, content->tile 경로로 해두자.
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // 스타일

	if (GetOpenFileName(&ofn))
	{
		m_pMap = CResourceManager::GetInst()->LoadToolD2DImage(szName);
		SetCurEditName(szName);
	}
}

void ClickTileGroupButton(DWORD_PTR param1, DWORD_PTR param2)
{
	// param1 : Scene_tool
	// param2 : CButtonUI
	CScene_Tool* scene_tool = (CScene_Tool*)param1;
	CButtonUI* button = (CButtonUI*)param2;
	scene_tool->ClickTileGroup(button);
}

void CScene_Tool::ClickTileGroup(CButtonUI* button)
{
	if (m_gTile == GROUP_TILE::NONE)
	{
		m_gTile = GROUP_TILE::GROUND;
		button->SetText(L"GROUND");
	}
	else if (m_gTile == GROUP_TILE::GROUND)
	{
		m_gTile = GROUP_TILE::WALL;
		button->SetText(L"WALL");
	}
	else if (m_gTile == GROUP_TILE::WALL)
	{
		m_gTile = GROUP_TILE::PLATFORM;
		button->SetText(L"PLATFORM");
	}
	else if (m_gTile == GROUP_TILE::PLATFORM)
	{
		m_gTile = GROUP_TILE::SPAWNPOINT;
		button->SetText(L"SPAWNPOINT");
	}
	else if (m_gTile == GROUP_TILE::SPAWNPOINT)
	{
		m_gTile = GROUP_TILE::NONE;
		button->SetText(L"NONE");
	}
}

void ClickTileButton(DWORD_PTR param1, DWORD_PTR param2)
{
	// param1 : Scene_tool
	// param2 : CTileButton
	CScene_Tool* scene_tool = (CScene_Tool*)param1;
	CTileButton* button = (CTileButton*)param2;
	scene_tool->ClickTile(button);
}

void CScene_Tool::ClickTile(CTileButton* button)
{
	SetIdx(button->GetIdx());
}

void CScene_Tool::SetCurEditName(const wstring& curEdit)
{
	WCHAR editPath[255] = L"";
	wcscat_s(editPath, 255, curEdit.c_str());

	// 경로 문자열 길이
	int strSize = (int)wcslen(editPath);
	int dest = 0;

	for (int i = strSize - 1; i > 0; --i)
	{
		// 처음으로 \\을 찾았다면
		if ('\\' == editPath[i])
		{
			dest = ++i;
			break;
		}
	}

	if (dest == 0)
	{
		m_strCurEdit = curEdit;
		return;
	}

	WCHAR strName[255] = L"";
	int count = 0;
	for (int i = dest; i < strSize - 4; ++i)
	{
		strName[count] = editPath[i];
		count++;
	}

	m_strCurEdit = strName;
}

//void ClickObjButton(DWORD_PTR param1, DWORD_PTR param2)
//{
//	// param1 : Scene_tool
//	// param2 : CGameObject*
//	CScene_Tool* scene_tool = (CScene_Tool*)param1;
//	CGameObject* obj = (CGameObject*)param2;
//	obj->SetPos(CCameraManager::GetInst()->GetRealPos(MousePos()));
//}

//void CScene_Tool::CreateObjPanel()
//{
//	CPanelUI* panelObjMaster = new CPanelUI;
//	panelObjMaster->SetPos(fPoint(0.f, 0.f));
//	panelObjMaster->SetScale(fPoint(400.f, 600.f));
//	AddObject(panelObjMaster, GROUP_GAMEOBJ::UI);
//	RegisterUI(panelObjMaster);
//
//	CButtonUI* btnPrayObj = new CButtonUI;
//	btnPrayObj->SetPos(fPoint(10.f, 10.f));
//	btnPrayObj->SetImage(
//		CResourceManager::GetInst()->LoadD2DImage(L"PrayerTable", L"texture\\Map\\PrayerTable\\priedieu_stand.png")
//	);
//	float width = (float)btnPrayObj->GetImageInfo()->GetWidth();
//	float height = (float)btnPrayObj->GetImageInfo()->GetHeight();
//	btnPrayObj->SetScale(fPoint(width, height));
//	btnPrayObj->SetClickCallBack(ClickObjButton, (DWORD_PTR)this, (DWORD_PTR));
//	panelObjMaster->AddChild(btnPrayObj);
//
//	CButtonUI* acolyteObjBtn = new CButtonUI;
//	acolyteObjBtn->SetPos(fPoint(btnPrayObj->GetFinalPos().x + btnPrayObj->GetScale().x + 20.f, 10.f));
//	acolyteObjBtn->SetImage(
//		CResourceManager::GetInst()->LoadD2DImage(L"PrayerTable", L"texture\\Map\\PrayerTable\\priedieu_stand.png")
//	);
//	acolyteObjBtn->SetScale(fPoint(width, height));
//	panelObjMaster->AddChild(acolyteObjBtn);
//}

void CScene_Tool::CreateTilePanel()
{
	CPanelUI* panelTile = new CPanelUI;
	panelTile->SetName(L"panelTile");
	panelTile->SetScale(fPoint(400.f, 600.f));
	panelTile->SetPos(fPoint(WINSIZE_X - 450.f, 50.f));

	CD2DImage* pImg = CResourceManager::GetInst()->LoadD2DImage(L"Tile", L"texture\\tile\\tilemap.bmp");
	for (UINT y = 0; y < 12; y++)
	{
		for (UINT x = 0; x < 12; x++)
		{
			CTileButton* btnTile = new CTileButton;
			btnTile->SetScale(fPoint(CTile::SIZE_TILE, CTile::SIZE_TILE));
			btnTile->SetPos(fPoint((float)x * CTile::SIZE_TILE, (float)y * CTile::SIZE_TILE));
			btnTile->SetPos(btnTile->GetPos() + fPoint(8.f, 8.f));
			btnTile->SetImage(pImg);
			btnTile->SetIdx(y * 12 + x);
			btnTile->SetClickCallBack(ClickTileButton, (DWORD_PTR)this, (DWORD_PTR)btnTile);
			panelTile->AddChild(btnTile);
		}
	}

	CButtonUI* btnTileGroup = new CButtonUI;
	btnTileGroup->SetScale(fPoint(100.f, 50.f));
	btnTileGroup->SetPos(fPoint(50.f, 500.f));
	btnTileGroup->SetText(L"NONE");
	btnTileGroup->SetClickCallBack(ClickTileGroupButton, (DWORD_PTR)this, (DWORD_PTR)btnTileGroup);
	panelTile->AddChild(btnTileGroup);

	AddObject(panelTile, GROUP_GAMEOBJ::UI);
}

void CScene_Tool::PrintMap(const fPoint& ratio)
{
	if (nullptr == m_pMap)
		return;

	fPoint pos = CCameraManager::GetInst()->GetLookAt();
	pos = pos - fPoint(WINSIZE_X / 2.f, WINSIZE_Y / 2.f);

	CRenderManager::GetInst()->RenderImage(
		m_pMap,
		0 - pos.x,
		0 - pos.y,
		ratio.x * m_pMap->GetWidth() - pos.x,
		ratio.y * m_pMap->GetHeight() - pos.y
	);
}

void CScene_Tool::PrintTileLine()
{
	fPoint pos = CCameraManager::GetInst()->GetLookAt();
	pos = pos - fPoint(WINSIZE_X / 2.f, WINSIZE_Y / 2.f);

	// 가로줄 출력
	for (UINT y = 0; y <= m_iTileY; y++)
	{
		CRenderManager::GetInst()->RenderLine(
			fPoint(0 - pos.x, y * CTile::SIZE_TILE - pos.y),
			fPoint(CTile::SIZE_TILE * m_iTileX - pos.x, y * CTile::SIZE_TILE - pos.y)
		);
	}

	// 세로줄 출력
	for (UINT x = 0; x <= m_iTileX; x++)
	{
		CRenderManager::GetInst()->RenderLine(
			fPoint(x * CTile::SIZE_TILE - pos.x, 0 - pos.y),
			fPoint(x * CTile::SIZE_TILE - pos.x, CTile::SIZE_TILE * m_iTileY - pos.y)
		);
	}
}

void CScene_Tool::PrintTileGroup()
{
	fPoint pos = CCameraManager::GetInst()->GetLookAt();
	pos = pos - fPoint(WINSIZE_X / 2.f, WINSIZE_Y / 2.f);

	const vector<CGameObject*>& vecTile = GetObjGroup(GROUP_GAMEOBJ::TILE);
	CTile* pTile;

	for (UINT i = 0; i < vecTile.size(); i++)
	{
		pTile = (CTile*)vecTile[i];
		if (GROUP_TILE::GROUND == pTile->GetGroup())
		{
			CRenderManager::GetInst()->RenderEllipse(
				pTile->GetPos().x + CTile::SIZE_TILE / 2.f - pos.x,
				pTile->GetPos().y + CTile::SIZE_TILE / 2.f - pos.y,
				CTile::SIZE_TILE / 2.f,
				CTile::SIZE_TILE / 2.f,
				RGB(255, 0, 0),
				3.f
			);
		}
		else if (GROUP_TILE::WALL == pTile->GetGroup())
		{
			CRenderManager::GetInst()->RenderEllipse(
				pTile->GetPos().x + CTile::SIZE_TILE / 2.f - pos.x,
				pTile->GetPos().y + CTile::SIZE_TILE / 2.f - pos.y,
				CTile::SIZE_TILE / 2.f,
				CTile::SIZE_TILE / 2.f,
				RGB(0, 255, 0),
				3.f
			);
		}
		else if (GROUP_TILE::SPAWNPOINT == pTile->GetGroup())
		{
			CRenderManager::GetInst()->RenderEllipse(
				pTile->GetPos().x + CTile::SIZE_TILE / 2.f - pos.x,
				pTile->GetPos().y + CTile::SIZE_TILE / 2.f - pos.y,
				CTile::SIZE_TILE / 2.f,
				CTile::SIZE_TILE / 2.f,
				RGB(0, 0, 255),
				3.f
			);
		}
		else if (GROUP_TILE::PLATFORM == pTile->GetGroup())
		{
			CRenderManager::GetInst()->RenderEllipse(
				pTile->GetPos().x + CTile::SIZE_TILE / 2.f - pos.x,
				pTile->GetPos().y + CTile::SIZE_TILE / 2.f - pos.y,
				CTile::SIZE_TILE / 2.f,
				CTile::SIZE_TILE / 2.f,
				RGB(255, 0, 255),
				3.f
			);
		}
	}
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK TileWinProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_SAVE)
		{
			CScene* pCurScene = CSceneManager::GetInst()->GetCurrentScene();

			CScene_Tool* pToolScene = dynamic_cast<CScene_Tool*>(pCurScene);
			assert(pToolScene);

			pToolScene->SaveTileData();

			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDC_LOAD)
		{
			CScene* pCurScene = CSceneManager::GetInst()->GetCurrentScene();

			CScene_Tool* pToolScene = dynamic_cast<CScene_Tool*>(pCurScene);
			assert(pToolScene);

			pToolScene->LoadTileData();

			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDC_MAP)
		{
			CScene* pCurScene = CSceneManager::GetInst()->GetCurrentScene();

			CScene_Tool* pToolScene = dynamic_cast<CScene_Tool*>(pCurScene);
			assert(pToolScene);

			pToolScene->LoadMap();

			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_SIZE)
		{
			int x = GetDlgItemInt(hDlg, IDC_EDIT_SIZEX, nullptr, false);
			int y = GetDlgItemInt(hDlg, IDC_EDIT_SIZEY, nullptr, false);

			CScene* pCurScene = CSceneManager::GetInst()->GetCurrentScene();

			CScene_Tool* pToolScene = dynamic_cast<CScene_Tool*>(pCurScene);
			assert(pToolScene);

			pToolScene->ClearGroup(GROUP_GAMEOBJ::TILE);
			pToolScene->CreateTile(x, y);
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK TileInfoWinProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			CScene* pCurScene = CSceneManager::GetInst()->GetCurrentScene();

			CScene_Tool* pToolScene = dynamic_cast<CScene_Tool*>(pCurScene);
			assert(pToolScene);

			WCHAR strName[255];
			GetDlgItemTextW(hDlg, IDC_EDIT_NAME, strName, 255);
			char name[255];
			wcstombs_s(nullptr, name, strName, 255);

			wstring path = CPathManager::GetInst()->GetContentPath();
			path += L"data\\";
			if (pToolScene->GetCurEditName().size() > 0)
				path += pToolScene->GetCurEditName();
			path += L"_SpawnPoint.json";

			ifstream stream;
			stream.open(path);

			Json::Value root;
			if (stream)
			{
				stream >> root;

				root["ObjectName"].append(name);
				root["PositionX"].append(pToolScene->GetSelectedPos().x);
				root["PositionY"].append(pToolScene->GetSelectedPos().y);
			}
			else
			{
				root["ObjectName"].append(name);
				root["PositionX"].append(pToolScene->GetSelectedPos().x);
				root["PositionY"].append(pToolScene->GetSelectedPos().y);
			}

			// 원하는 수정을 거친 후 formating json
			Json::StyledWriter writer;
			string result = writer.write(root);

			// output to json file
			ofstream output_file(path);
			output_file << result;
			output_file.close();

			pToolScene->SetInfoWndCount(0);
			pToolScene->SpawnObjects(pToolScene, name);

			EndDialog(hDlg, LOWORD(wParam));

			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			CScene* pCurScene = CSceneManager::GetInst()->GetCurrentScene();

			CScene_Tool* pToolScene = dynamic_cast<CScene_Tool*>(pCurScene);
			assert(pToolScene);
			
			pToolScene->SetInfoWndCount(0);
			EndDialog(hDlg, LOWORD(wParam));

			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
#include "framework.h"
#include "CTile.h"
#include "CD2DImage.h"

CTile::CTile()
{
	m_pImg = nullptr;
	m_iIndex = 0;
	SetScale(fPoint(SIZE_TILE, SIZE_TILE));
}

CTile::~CTile()
{
}

CTile* CTile::Clone()
{
	return new CTile(*this);
}

void CTile::update()
{
}

void CTile::render()
{
	if (nullptr == m_pImg) return;

	UINT iWidth = m_pImg->GetWidth();
	UINT iHeight = m_pImg->GetHeight();

	UINT iMaxRow = iHeight / SIZE_TILE;
	UINT iMaxCol = iWidth / SIZE_TILE;

	UINT iCurX = (m_iIndex % iMaxCol);
	UINT iCurY = (m_iIndex / iMaxCol) % iMaxRow;

	fPoint fptRenderPos = CCameraManager::getInst()->GetRenderPos(m_fptPos);

	CRenderManager::getInst()->RenderFrame(
		m_pImg,
		fptRenderPos.x,
		fptRenderPos.y,
		m_fptScale.x,
		m_fptScale.y,
		iCurX * m_fptScale.x,
		iCurY * m_fptScale.y,
		m_fptScale.x,
		m_fptScale.y
	);
}

void CTile::SetTexture(CD2DImage* pImg)
{
	m_pImg = pImg;
}

void CTile::SetTileIndex(int index)
{
	m_iIndex = index;
}

void CTile::Save(FILE* pFile)
{
	fwrite(&m_iIndex, sizeof(int), 1, pFile);
}

void CTile::Load(FILE* pFile)
{
	fread(&m_iIndex, sizeof(int), 1, pFile);
}

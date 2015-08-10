
#include "SoundManager.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//=========================================================================
// 1. �Լ���: CGSoundManager(HWND hWnd = NULL )
// 2. ��   ��: ������
// 3. �Է��μ�: HWND �ڵ�
// 4. ��ȯ��: ����
//=========================================================================
CGSoundManager::CGSoundManager(HWND hWnd)
	: m_SoundBufferIndex(0)
{
	m_hWnd = hWnd; // �ڵ鰪�ʱ�ȭ
	memset(m_ArrlpDSBuf, 0, sizeof(int) * SOUNDBUFFER_SIZE);    // �����ʱ�ȭ
}

//=========================================================================
// 1. �Լ���: ~CGSoundManager
// 2. ��   ��: �Ҹ���
// 3. �Է��μ�: ����
// 4. ��ȯ��: ����
//=========================================================================
CGSoundManager::~CGSoundManager(void)
{
	AllStop();
	for (int i = 0; i < SOUNDBUFFER_SIZE; i++)
	{
		if (m_ArrlpDSBuf[i] != NULL)
		{
			m_ArrlpDSBuf[i]->Release();
			m_ArrlpDSBuf[i] = NULL;
		}
	}
}

//=========================================================================
// 1. �Լ���: SoundManagerInit
// 2. ��   ��: SoundManager������ϱ������ʱ�ȭ
// 3. �Է��μ�: ����
// 4. ��ȯ��: ����
//=========================================================================
void CGSoundManager::DirectSoundInit(HWND hWnd)
{
	if (m_hWnd == NULL)
		m_hWnd = hWnd;

	// ���̷�Ʈ���尴ü����
	HRESULT dsrval = ::DirectSoundCreate(NULL, &m_lpDS, NULL);
	if (dsrval != DS_OK)
	{
		DirectSoundError(L"��ü��������");
		return;
	}
	// ���̷�Ʈ������ȯ���������Ѵ�.
	dsrval = m_lpDS->SetCooperativeLevel(m_hWnd, DSSCL_PRIORITY);

	if (dsrval != DS_OK) DirectSoundError(L"��弳������");
}


//=========================================================================
// 1. �Լ���: DirectSoundError
// 2. ��   ��: �����޼������
// 3. �Է��μ�: char* �����޼���
// 4. ��ȯ��: ����
//=========================================================================
void CGSoundManager::DirectSoundError(TCHAR *errorString)
{
	exit(1);
}


//=========================================================================
// 1. �Լ���: AddSoundBuffer
// 2. ��   ��: SoundBuffer �迭���߰��Ѵ�
// 3. �Է��μ�: LPSoundManagerBUFFER ������۱���ü
// 4. ��ȯ��: �迭��index ��ġ��ȯ
//=========================================================================
int  CGSoundManager::AddSoundBuffer(LPDIRECTSOUNDBUFFER buffer)
{
	if (m_SoundBufferIndex < SOUNDBUFFER_SIZE - 1)
	{
		m_ArrlpDSBuf[m_SoundBufferIndex] = buffer;
	}
	return m_SoundBufferIndex++;
}

//=========================================================================
// 1. �Լ���: DeleteSoundBuffer
// 2. ��   ��: ������۹迭���ش�index ����
// 3. �Է��μ�: int iDel_index
// 4. ��ȯ��: ����
//=========================================================================
void CGSoundManager::DeleteSoundBuffer(int iDel_index)
{

	if (m_ArrlpDSBuf[iDel_index] == NULL)  return;

	m_ArrlpDSBuf[iDel_index]->Release();
	m_SoundBufferIndex--;

	for (int i = iDel_index; i < m_SoundBufferIndex; i++)
	{
		m_ArrlpDSBuf[i] = m_ArrlpDSBuf[i + 1];
	}
	m_ArrlpDSBuf[m_SoundBufferIndex] = NULL;
}

//=========================================================================
// 1. �Լ���: LoadWav
// 2. ��   ��: ������۹迭���ش�index ����
// 3. �Է��μ�: char* wav (���ϸ�& path) , int Flag( �ɼ�)
// 4. ��ȯ��: LPSoundManagerBUFFER ������۱���ü
//=========================================================================
LPDIRECTSOUNDBUFFER CGSoundManager::LoadWav(TCHAR * wav)
{
	int ret;
	WORD                    wExtraAlloc;
	UINT                    i, DataIn;
	LPDIRECTSOUNDBUFFER     lpDSTemp;
	HMMIO                   hmmioIn;
	MMIOINFO                mmioinfoIn;
	MMCKINFO                ckInfo, ckInfoRIFF;
	LPWAVEFORMATEX          lpWaveFormat = 0;
	PCMWAVEFORMAT           pcmWaveFormat;
	LPBYTE                  lpData;
	LPVOID                  lpWrapPtr;
	DWORD                   dwBSize, dwWavSize, dwDataSize, dwWrapBSize;
	DSBUFFERDESC            dsbd;

	// ȭ��������
	hmmioIn = mmioOpen(wav, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (!hmmioIn) DirectSoundError(L"WAV ȭ��������������");
	// ����ȭ����RIFF WAVEȭ������Ȯ���Ѵ�   
	ckInfoRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if (mmioDescend(hmmioIn, &ckInfoRIFF, NULL, MMIO_FINDRIFF) != 0)
	{
		DirectSoundError(L"RIFF WAVE ȭ���̾ƴմϴ�");
	}
	// 'fmt' Chunk����Ȯ���Ѵ�
	ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hmmioIn, &ckInfo, &ckInfoRIFF, MMIO_FINDCHUNK) != 0)
	{
		DirectSoundError(L"ȭ����fmt Chunk���ƴմϴ�");
	}
	// ����ȭ�����д´�
	ret = mmioRead(hmmioIn, (HPSTR)&pcmWaveFormat, (long) sizeof(pcmWaveFormat));
	if (ret != (long) sizeof(pcmWaveFormat))
	{
		DirectSoundError(L"ȭ���б����");
	}
	// ����ȭ����PCM��������Ȯ���Ѵ�
	if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
		wExtraAlloc = 0;
	else
	{
		ret = mmioRead(hmmioIn, (LPSTR)&wExtraAlloc, (long) sizeof(wExtraAlloc));
		if (ret != (long) sizeof(wExtraAlloc))
		{
			DirectSoundError(L"ȭ���б����");
		}
	}

	lpWaveFormat = (LPWAVEFORMATEX)malloc(sizeof(WAVEFORMATEX) + wExtraAlloc);
	memcpy(lpWaveFormat, &pcmWaveFormat, sizeof(pcmWaveFormat));
	lpWaveFormat->cbSize = wExtraAlloc;
	if (wExtraAlloc != 0)
	{
		ret = mmioRead(hmmioIn,
			(LPSTR)(((BYTE*)&(lpWaveFormat->cbSize)) + sizeof(wExtraAlloc)),
			(long)(wExtraAlloc));
		if (ret != (long) sizeof(wExtraAlloc))
		{
			DirectSoundError(L"ȭ���б����");
		}
	}

	if (mmioAscend(hmmioIn, &ckInfo, 0) != MMSYSERR_NOERROR)
	{
		DirectSoundError(L"����");
	}


	if (mmioSeek(hmmioIn, ckInfoRIFF.dwDataOffset + sizeof(FOURCC), SEEK_SET) == -1)
	{
		DirectSoundError(L"����");
	}
	// 'data' Chunk��ã�´�
	MMCKINFO    pckIn;
	pckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hmmioIn, &pckIn, &ckInfoRIFF, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
	{
		DirectSoundError(L"Data Chunk���ƴմϴ�");
	}
	dwDataSize = pckIn.cksize;
	lpData = NULL;

	// ����ƽ2�����۸���������ر���ü�������Ѵ�.
	// �������������º���, �¿�뷱��, ���ļ������̰����ϵ����Ѵ�
	memset(&dsbd, 0, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;
	dsbd.dwBufferBytes = dwDataSize;
	dwWavSize = lpWaveFormat->cbSize + sizeof(WAVEFORMATEX);

	dsbd.lpwfxFormat = (LPWAVEFORMATEX)malloc(dwWavSize);
	memcpy(dsbd.lpwfxFormat, lpWaveFormat, dwWavSize);
	free(lpWaveFormat);

	// 2��������۸������Ѵ�.
	if (m_lpDS->CreateSoundBuffer(&dsbd, &lpDSTemp, NULL) != DS_OK)
	{
		DirectSoundError(L"������ۻ�������");
	}

	free(dsbd.lpwfxFormat);
	// �����ȹ��ۿ������Ѵ�
	ret = lpDSTemp->Lock(0, dwDataSize, (void**)&lpData, &dwBSize,
		&lpWrapPtr, &dwWrapBSize, 0L);
	if (ret != DS_OK)
	{
		DirectSoundError(L"����");
	}
	dwDataSize = dwBSize;
	// ����ȭ���ǻ���(�����Ȳ)���˾ƿ´�
	if (mmioGetInfo(hmmioIn, &mmioinfoIn, 0) != MMSYSERR_NOERROR)
	{
		DirectSoundError(L"����");
	}

	DataIn = dwDataSize;
	if (DataIn > pckIn.cksize) DataIn = pckIn.cksize;
	pckIn.cksize -= DataIn;

	for (i = 0; i < DataIn; i++)
	{
		if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
		{
			ret = mmioAdvance(hmmioIn, &mmioinfoIn, MMIO_READ);
			if (ret != MMSYSERR_NOERROR) DirectSoundError(L"����");
		}
		*((BYTE*)lpData + i) = *((BYTE*)mmioinfoIn.pchNext++);
	}
	if (mmioSetInfo(hmmioIn, &mmioinfoIn, 0) != MMSYSERR_NOERROR)
	{
		DirectSoundError(L"����");
	}
	lpDSTemp->Unlock(lpData, dwBSize, lpWrapPtr, dwWrapBSize);
	return lpDSTemp;
}


//=========================================================================
// 1. �Լ���: Stop
// 2. ��   ��: ����
// 3. �Է��μ�: int index ����������ε���
// 4. ��ȯ��: ����
//=========================================================================
void CGSoundManager::Stop(int index)
{
	m_ArrlpDSBuf[index]->Stop();
}

//=========================================================================
// 1. �Լ���: AllStop
// 2. ��   ��: ����
// 3. �Է��μ�: ����
// 4. ��ȯ��: ����
//=========================================================================
void CGSoundManager::AllStop()
{
	for (int i = 0; i < GetBufferSize(); i++)
	{
		m_ArrlpDSBuf[i]->Stop();
	}
}


//=========================================================================
// 1. �Լ���: IsPlaying
// 2. ��   ��: ����������ΰ����Ǵ�
// 3. �Է��μ�: int index ����������ε���
// 4. ��ȯ��: BOOL   TRUE = ������϶�  FALSE = ������̾ƴҶ�
//=========================================================================
BOOL CGSoundManager::IsPlaying(int index)
{
	DWORD   dwPlayCursol;
	DWORD   dwWriteCursol;

	m_ArrlpDSBuf[index]->GetCurrentPosition(&dwPlayCursol, &dwWriteCursol);

	if (dwPlayCursol != 0) return TRUE;

	return FALSE;
}


//=========================================================================
// 1. �Լ���: Play
// 2. ��   ��: �������
// 3. �Է��μ�: int index ����������ε���, LOOP ( TRUE : �ݺ����   FALSE(�⺻��) : 1���ݺ�
// 4. ��ȯ��: ����
//=========================================================================
void CGSoundManager::Play(int index, BOOL Loop)
{
	if (IsPlaying(index) == TRUE) return;
	if (IsBufIndex(index))
	{
		if (IsPlaying(index))
		{
			Stop(index);
		}
		m_ArrlpDSBuf[index]->SetCurrentPosition(0);
		m_ArrlpDSBuf[index]->Play(0, 0, (Loop == TRUE) ? 1 : 0);
	}
}

//=========================================================================
// 1. �Լ���: IsBufIndex
// 2. ��   ��: ���۰�Empty��������Ȯ��
// 3. �Է��μ�: ����������ε���
// 4. ��ȯ��: TRUE : NULL�̾ƴҰ��  FALSE : NULL�ϰ��
//=========================================================================
BOOL CGSoundManager::IsBufIndex(int index)
{
	if (m_ArrlpDSBuf[index] == NULL)
		return FALSE;
	return TRUE;
}


//=========================================================================
// 1. �Լ���: SetPan
// 2. ��   ��: �¿�뷱������
// 3. �Է��μ�: ����������ε���, �뷱����(-10000~10000)
// 4. ��ȯ��: ����
//=========================================================================
void CGSoundManager::SetPan(int index, long pan)
{
	if (m_lpDS == NULL)return;
	if (IsBufIndex(index))
		m_ArrlpDSBuf[index]->SetPan(pan);
}

//=========================================================================
// 1. �Լ���: SetVol
// 2. ��   ��: ��������
// 3. �Է��μ�: ����������ε���, ������(-10000~10000)
// 4. ��ȯ��: ����
//=========================================================================
void CGSoundManager::SetVol(int index, long vol)
{
	if (m_lpDS == NULL)return;
	if (vol<-10000)  vol = -10000;
	if (vol>10000)   vol = 10000;
	if (IsBufIndex(index)) m_ArrlpDSBuf[index]->SetVolume(vol);
}

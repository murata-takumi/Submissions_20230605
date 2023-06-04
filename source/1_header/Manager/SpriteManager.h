#pragma once
#include "Application.h"

#include <DirectXTK12-master/Inc/CommonStates.h>
#include <DirectXTK12-master/Inc/ResourceUploadBatch.h>
#include <DirectXTK12-master/Inc/SpriteBatch.h>
#include <DirectXTK12-master/Inc/SpriteFont.h>

#include <sstream>
#include <time.h>

/// <summary>
/// �摜�╶���t�H���g���Ǘ�����N���X
/// </summary>
class Dx12Wrapper;
class SpriteManager
{
	template<typename T>
	using ComPtr = ComPtr<T>;

private:
	unique_ptr<GraphicsMemory> _gmemory;									//�O���t�B�b�N�X������
	unique_ptr<SpriteFont> _spriteFont;										//�t�H���g�\���p�I�u�W�F�N�g
	unique_ptr<SpriteBatch> _spriteBatch;									//�X�v���C�g�i�摜�j�\���p�I�u�W�F�N�g

	Dx12Wrapper& _dx12;														//Dx12Wrapper�C���X�^���X

	ID3DBlob* _psBlob = nullptr;											//�s�N�Z���V�F�[�_�[�p�f�[�^

	SIZE _winSize;															//�E�B���h�E�T�C�Y
	LONG _width;															//��ʂ̕�
	LONG _height;															//��ʂ̍���

	ID3D12DescriptorHeap* _heapForSpriteFont = nullptr;						//�t�H���g�E�摜�p�q�[�v
	D3D12_CPU_DESCRIPTOR_HANDLE _tmpCPUHandle;								//�q�[�v�n���h��(CPU)
	D3D12_GPU_DESCRIPTOR_HANDLE _tmpGPUHandle;								//�q�[�v�n���h��(GPU)
	UINT _incrementSize;													//�n���h���̃A�h���X�̍���

	map<string,D3D12_GPU_DESCRIPTOR_HANDLE> _GPUHandles;					//�n���h��(GPU)�̃x�N�g��

	RECT _backGroundRect;													//�w�i�p��`
	RECT _startButtonRect;													//�^�C�g����ʂł̃X�^�[�g�{�^���p��`
	RECT _endButtonRect;													//�^�C�g����ʂł̏I���{�^���p��`
	RECT _loadingRect;														//���[�h���A�C�R���p��`
	RECT _titleRect;														//�^�C�g����ʂ̕�����p��`

	LONG _buttonLeft,_buttonRight,_titleWidth,_titleHeight;

	ComPtr<ID3D12RootSignature> _spriteRS;									//�X�v���C�g�`��p���[�g�V�O�l�`��

	int _x;																	//�}�E�X��X���W
	int _y;																	//�}�E�X��Y���W

	HRESULT CreateSpriteRS();												//SpriteBatch�p���[�g�V�O�l�`�����쐬����֐�
	HRESULT CreateUIBufferView(const wchar_t* path, string key);			//UI�p�̉摜�̃o�b�t�@�[�E�r���[���쐬����֐�

	XMFLOAT2 GetCenterPos(
		RECT rect, const wchar_t* wstr, float rectWidth, float rectHeight);	//��`���當�͂𒆉������ɏo������W���擾����

	bool IsInRect(RECT rect);												//�}�E�X�J�[�\������`���ɑ��݂��邩�m�F����֐�

	void InitSpriteDevices();												//�X�v���C�g�E������\���p�I�u�W�F�N�g������������֐�
	void ShiftHandles();													//CPU,GPU�p�n���h�������炷�֐�
public:
	SpriteManager(Dx12Wrapper& dx12,LONG width,LONG height);	//�R���X�g���N�^

	bool TitleIsOnStart();										//�^�C�g����ʂł̃X�^�[�g�{�^���̏�Ƀ}�E�X�����邩�`�F�b�N
	bool TitleIsOnEnd();										//�^�C�g����ʂł̏I���{�^���̏�Ƀ}�E�X�����邩�`�F�b�N

	void AdjustSpriteRect();									//��ʃT�C�Y�̕ύX�����m���ċ�`�𒲐�����

	void TitleDraw();											//�^�C�g����ʂł�UI��`��

	void BackGroundDraw();										//�w�i��`��
	void LoadingDraw();											//���[�h��ʂł̕`��
	void CursorDraw();											//�}�E�X�J�[�\����`��

	void Commit();												//�O���t�B�b�N�X���������R�}���h���X�g�ɃZ�b�g
	void SetMousePosition(int x,int y);							//�}�E�X���W���X�V
};
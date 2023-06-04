#include "Manager/InputManager.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
InputManager::InputManager()
{
	_keyboard = make_unique<Keyboard>();	//Keyboard�C���X�^���X��������
	_mouse = make_unique<Mouse>();			//Mouse�C���X�^���X��������

	UpdateInput();							//���͂��X�V
}

/// <summary>
/// ���͏����X�V����֐�
/// </summary>
void
InputManager::UpdateInput()
{
	//�L�[�{�[�h�̓��͂��X�V
	_keyState = _keyboard->GetState();
	_keyboardButtons.Update(_keyState);

	//�}�E�X�̓��͂��X�V
	_mouseState = _mouse->GetState();	
	_tracker.Update(_mouseState);
}

/// <summary>
/// �L�[��Ԃ�Ԃ��֐�
/// </summary>
/// <returns>�L�[���</returns>
Keyboard::State
InputManager::KeyState()
{
	return _keyState;
}

/// <summary>
/// �}�E�X��Ԃ�Ԃ��֐�
/// </summary>
/// <returns>�}�E�X���</returns>
Mouse::State
InputManager::MouseState()
{
	return _mouseState;
}

/// <summary>
/// �}�E�X��Ԃ�Ԃ��֐�(�{�^�����������E�������u�Ԃ��擾)
/// </summary>
/// <returns>�}�E�X���</returns>
Mouse::ButtonStateTracker
InputManager::MouseTracker()
{
	return _tracker;
}

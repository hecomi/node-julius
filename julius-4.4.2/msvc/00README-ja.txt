Julius�̃R���p�C�����@
=======================

���̃t�@�C���ł� Microsoft Visual Studio 2013 �ȍ~�ɂ����� Julius �̃R��
�p�C�����@�ɂ��ĉ�����܂��D�܂��CGUI�ŃT���v���A�v���P�[�V�����ł�
��"SampleApp" �� Julius �̃��b�p�[�N���X�̒�`�ɂ��Ă�������܂��D�R
���p�C���ƃe�X�g�̕��@��m�肽�����͈ȉ����������������D

�{�o�[�W������ Microsoft Visual Studio 2013 �� Windows10 �ɂē���m�F
���Ă��܂��D

Julius ��V���Ɏg�p����ꍇ�C�������f���C���ꃂ�f���C����� Julius �̐�
����L�q���� jconf �t�@�C�����K�v�ƂȂ�܂��D�ڍׂ͈ȉ����������������D


1. ����
========

Julius�𓮂������߂ɂ͉������f���C���ꃂ�f����2�̃��f���ƁCJulius ��
�ݒ���L�q���� jconf �t�@�C�����K�v�ƂȂ�܂��D���f���̎d�l�C�T�|�[�g��
�́C�g�����̏ڍׁC���萫�Ȃǂɂ��Ă� Julius �̃E�F�u�y�[�W���Q�Ƃ���
���������D�Ȃ��C�E�F�u�y�[�W�ł͓��{��̕W�����f�����܂Ƃ߂��f�B�N�e�[
�V�����L�b�g��z�z���Ă��܂��D

�Ȃ��C�f�B�N�e�[�V�����L�b�g�� "SampleApp" �Ŏg�p����ꍇ�Cjconf �ɋL�q
����Ă��� -charconv �I�v�V�������폜���Ă���g�p���Ă��������D


2. �r���h
==============

VisualStudio 2013�ȍ~�� "JuliusLib.sln" ���ŊJ���C�r���h���Ă��������D
"Debug" ���邢�� "Release" �f�B���N�g���̒��� "julius.exe", "adintool.exe" �����
"SampleApp.exe" ����������܂��D


3. �e�X�g
==========

3.1  julius.exe
-----------------

"julius.exe" �� Win32 �̃R���\�[���A�v���P�[�V�����ł��D�R�}���h�v����
�v�g�� jconf �t�@�C�����w�肷�邱�ƂŎ��s���邱�Ƃ��ł��܂��D

    % julius.exe -C xxx.jconf

3.2  SampleApp.exe
-------------------

"SampleApp.exe" �̓V���v���� Julius ���b�p�[�N���X�� JuliusLib ���C�u��
�����g�p���� Julius ��GUI�ŃT���v���A�v���P�[�V�����ł��D

�g�p����ɂ́CSampleApp ���N����C���j���[����g�p������ jconf �t�@�C��
���J���C���������j���[����G���W���̎��s���w�肵�܂��DJulius �͎q�X���b
�h�Ƃ��ē��삵�C�������͊J�n��F�����ʏo�͂Ȃǂ̊e�C�x���g�����C�����b
�Z�[�W�ɕ`�悵�܂��D

���ʂ̕\���ɖ�肪����ꍇ�CSampleApp.cpp ��98�s�ɂ��郍�P�[���̐ݒ���C
�g�p���錾�ꃂ�f���ɍ��킹�ĕύX���ăR���p�C�����Ȃ����Ă��������D

Julius �̏o�͂� "juliuslog.txt" �ɕۑ�����܂��D���� Julius �ɃG���[��
�N�����ꍇ�C���̃t�@�C�����`�F�b�N���Ă��������D

�Ȃ��CSampleApp �Ńf�B�N�e�[�V�����L�b�g���g�p����ꍇ�Cjconf �t�@�C��
�ɋL�q����Ă��� -charconv �I�v�V�������폜���Ă���g�p���Ă��������D


4. Julius �N���X
=================

SampleApp �ł̓V���v���ȃN���X��`�ł��� "Julius.cpp" �� "Julius.h" ��
�g�p���Ă��܂��D������ Windows �̃��b�Z�[�W�`���� JuliusLib �̋@�\��
���p���邽�߂� "cJulius" �Ƃ������b�p�[�N���X���`���Ă��܂��D����͈�
���̂悤�ɃA�v���P�[�V�����ŗ��p���邱�Ƃ��ł��܂��D

-----------------------------------------------------------------
#include "Julius.h"

cJulius julius;

....

// Windows Procedure callback
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch( message ) {
	case WM_CREATE:
	    // start Julius when the main window is created
	    julius.initialize( "fast.jconf" );
	    julius.startProcess( hWnd );
	    break;
	case WM_JULIUS:
            // Julius events
	    switch( LOWORD( wParam ) ) {
		case JEVENT_AUDIO_READY: ...
		case JEVENT_RECOG_BEGIN: ...
		case JEVENT_RESULT_FINAL:....
	    }
	.....
    }
    ...
}
-----------------------------------------------------------------

�ڍׂ�SampleApp.cpp��Julius.cpp���������������D


5.  �\�[�X���̕����R�[�h�ɂ���
=================================

Julius �̃\�[�X�R�[�h�ł͓��{��̕����� EUC-JP �ŋL�q���Ă��܂��D������
���� MSVC++ �œǂ݂����ꍇ�CUTF-8 �֕ϊ����Ă��������D


6.  �X�V����
=============

2016/8/19 (ver.4.4)

	VisualStudio 2013 �ɍ��킹�ĕύX�D
	PortAudio �� zlib �̃\�[�X�𓯍�
	adintool ��ǉ�

2010/12 (ver.4.1.5.1)

	���C�Z���X�֌W�̏C��
	���[�h�~�[�̏C��
	�w�b�_�̏C��

2009/11 (ver.4.1.3)

	����

#pragma once

// �ȉ��̃}�N���́A�Œ���K�v�ȃv���b�g�t�H�[�����`���܂��B�Œ���K�v�ȃv���b�g�t�H�[���Ƃ́A
// �A�v���P�[�V���������s���邽�߂ɕK�v�ȋ@�\��������ł��Â��o�[�W������ Windows �� Internet Explorer �Ȃ�
// �������܂��B�����̃}�N���́A�w�肵���o�[�W�����ƁA����ȑO�̃o�[�W�����̃v���b�g�t�H�[����ŗ��p�ł��邷�ׂĂ̋@�\��L���ɂ��邱�Ƃɂ���� 
// ���삵�܂��B

// ���Ŏw�肳�ꂽ��`�̑O�ɑΏۃv���b�g�t�H�[�����w�肵�Ȃ���΂Ȃ�Ȃ��ꍇ�A�ȉ��̒�`��ύX���Ă��������B
// �قȂ�v���b�g�t�H�[���ɑΉ�����l�Ɋւ���ŐV���ɂ��ẮAMSDN ���Q�Ƃ��Ă��������B
#ifndef WINVER                          // �Œ���K�v�ȃv���b�g�t�H�[���� Windows Vista �ł��邱�Ƃ��w�肵�܂��B
#define WINVER 0x0600           // ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_WINNT            // �Œ���K�v�ȃv���b�g�t�H�[���� Windows Vista �ł��邱�Ƃ��w�肵�܂��B
#define _WIN32_WINNT 0x0600     // ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_WINDOWS          // �Œ���K�v�ȃv���b�g�t�H�[���� Windows 98 �ł��邱�Ƃ��w�肵�܂��B
#define _WIN32_WINDOWS 0x0410 // ����� Windows Me �܂��͂���ȍ~�̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_IE                       // �Œ���K�v�ȃv���b�g�t�H�[���� Internet Explorer 7.0 �ł��邱�Ƃ��w�肵�܂��B
#define _WIN32_IE 0x0700        // ����� IE �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

�T���v���v���O�C��


�� �v���O�C���ɂ���

Julius-4.1 ���v���O�C�����g����悤�ɂȂ�܂����D
���̃f�B���N�g���ɂ́C�v���O�C���̃T���v���\�[�X���܂܂�Ă��܂��D

�v���O�C���̎d�g�݂�T�v�C�����ȂǑS�ʓI�Ȏ����ɂ��ẮC
Juliusbook ���������������D
�T���v���\�[�X�ɂ́C�֐��̎d�l���R�����g�Ƃ��ď�����Ă��܂��D


�� �t�@�C���\��

    00readme.txt		���̃t�@�C��
    plugin_defs.h		�v���O�C���p��`�w�b�_
    adin_oss.c			�������̓v���O�C���̃T���v���FOSS�}�C�N����
    audio_postprocess.c		�����㏈���v���O�C���̃e���v���[�g
    fvin.c			�����ʓ��̓v���O�C���̃e���v���[�g
    feature_postprocess.c	�����ʌ㏈���v���O�C���̃e���v���[�g
    calcmix.c			�K�E�X���z�W���v�Z�v���O�C���̃T���v��
    Makefile			Linux �p Makefile


�� �v���O�C���̎d�l�ƃR���p�C���ɂ���

�v���O�C���t�@�C���̊g���q�� .jpi �ł��D���Ԃ́C���L�I�u�W�F�N�g�t�@�C
���ł��DLinux �� cygwin �ł��ƁC�ȉ��̂悤�ɂ��ăR���p�C���ł��܂��D

    % gcc -shared -o result.jpi result.c

cygwin �ŃR���p�C�������v���O�C�����Ccygwin �������ł����삳����ɂ́C
-mno-cygwin �����܂��D

    % gcc -shared -mno-cygwin -o result.jpi result.c

Mac OS X (darwin) �ł͈ȉ��̂悤�ɃR���p�C�����܂��D

    % gcc -bundle -flat_namespace -undefined suppress -o result.jpi result.c


�� Julius �Ƀv���O�C����ǂݍ��܂�����@

Julius �̃I�v�V���� "-plugindir dirname" ���g���܂��Ddirname �ɂ̓v��
�O�C����u���Ă���f�B���N�g�����w�肵�Ă��������D
�w�肵���f�B���N�g�����ɂ���S�Ă� .jpi �t�@�C�����ǂݍ��܂�܂��D

�Ȃ��C�I�v�V�������g������v���O�C�������݂���̂ŁC"-plugindir" ��
�ݒ�̂ł��邾���ŏ��̂ق��Ŏw�肵�������悢�ł��傤�D


�� ����e�X�g���̂P�iresult.jpi�j

result.c �́C�F�����ʂ̕�������󂯎���ďo�͂���ȒP�ȃv���O�C���ł��D
Julius ���R���p�C����C�ȉ��̂悤�ɂ��Ď����Ă݂܂��傤�D

	% cd plugin (���̃f�B���N�g��)
	% make result.jpi
	% cd ..
	% ./julius/julius ... -plugindir plugin

�Ȃ��CMac OS X �ł͈ȉ��̂悤�� Makefile.darwin �����g���������D

        % make -f Makefile.darwin result.jpi


�� ����e�X�g���̂Q�i�I�[�f�B�I���̓v���O�C���j

adin_oss.c �́COSS API ���g�������͂̊g�����s���v���O�C���ł��D
Julius �{�̂���� "-input myadin" �őI���ł��܂��D
Julius ���R���p�C����C�ȉ��̂悤�ɂ��Ď����Ă݂܂��傤�D

	% cd plugin (���̃f�B���N�g��)
	% make adin_oss.jpi
	% cd ..
	% ./julius/julius -plugindir plugin -input myadin

�܂��C�������̓v���O�C���� adintool �� adinrec ������Ăяo���܂��D

	% ./adinrec/adinrec -plugindir plugin -input myadin



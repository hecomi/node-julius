    mkbinhmm

MKBINHMM(1)                                                        MKBINHMM(1)



���O
           mkbinhmm
          - �o�C�i�� HMM �ϊ�

�T�v
       mkbinhmm [-htkconf HTKConfigFile] {hmmdefs_file} {binhmm_file}

DESCRIPTION
       mkbinhmm �́CHTK�̃A�X�L�[�`����HMM��`�t�@�C�����CJulius�p�̃o�C�i ��
       �`���֕ϊ����܂��D������g�����Ƃ� Julius�̋N�������������邱�Ƃ��ł���
       ���D

       ���̉������f���̓������o�������o�̓t�@�C���̃w�b�_�ɖ��ߍ��ނ��Ƃ��ł�
       �܂��D���ߍ��ނɂ́C�w�K���ɓ����ʒ��o�ɗp���� HTK Config �t�@�C����
       "-htkconf" �Ŏw�肵�܂��D�w�b�_�ɒ��o�����𖄂ߍ��ނ��ƂŁC �F�����Ɏ�
       ���I�ɕK�v�ȓ������o�p�����[�^���Z�b�g�����̂ŁC�֗��ł��D

       ���͂Ƃ��āCHTK�A�X�L�[�`���̂ق��ɁC���ɕϊ��ς݂�Julius�p�o�C�i��HMM
       ��^���邱�Ƃ��ł��܂��D-htkconf �ƕ��p����΁C �����̃o�C�i��HMM�ɓ���
       �ʒ��o�����p�����[�^�𖄂ߍ��ނ��Ƃ��ł��܂��D

       mkbinhmm �� gzip ���k���ꂽHMM��`�t�@�C�������̂܂ܓǂݍ��߂܂��D

OPTIONS
        -htkconf  HTKConfigFile
           �w�K���ɓ����ʒ��o�Ɏg�p����HTK Config�t�@�C�����w�肷��D�w�肳 ��
           ���ꍇ�C���̒��̐ݒ�l���o�̓t�@�C���̃w�b�_�ɖ��ߍ��܂��D ���͂�
           ���Ƀw�b�_������ꍇ�㏑�������D

       hmmdefs_file
           �ϊ����̉������f����`�t�@�C�� (MMF)�DHTK ASCII �`���C���邢��
           Julius �o�C�i���`���D

       hmmdefs_file
           Julius�p�o�C�i���`���t�@�C���̏o�͐�D

EXAMPLES
       HTK ASCII �`���� HMM ��`���o�C�i���`���ɕϊ�����F
       HTK�̐ݒ�t�@�C�� Config �̓��e���w�b�_�ɏ�������ŏo�́F
       �Â��o�C�i���`���t�@�C���Ƀw�b�_��񂾂��ǉ�����F

SEE ALSO
        julius ( 1 ) ,
        mkbingram ( 1 )

COPYRIGHT
       Copyright (c) 1991-2013 ���s��w �͌�������

       Copyright (c) 1997-2000 ��񏈗��U�����Ƌ���(IPA)

       Copyright (c) 2000-2005 �ޗǐ�[�Ȋw�Z�p��w�@��w ���쌤����

       Copyright (c) 2005-2013 ���É��H�Ƒ�w Julius�J���`�[��

LICENSE
       Julius �̎g�p�����ɏ����܂��D



                                  19/12/2013                       MKBINHMM(1)

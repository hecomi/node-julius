    mkbingram

MKBINGRAM(1)                                                      MKBINGRAM(1)



���O
           mkbingram
          - �o�C�i�� N-gram �ϊ�

�T�v
       mkbingram [-nlr forward_ngram.arpa] [-nrl backward_ngram.arpa]
                 [-d old_bingram_file] {output_bingram_file}

DESCRIPTION
       mkbingram �́CARPA�`���� N-gram ��`�t�@�C����Julius�p�̃o�C�i��N-gram
       �t�@�C���ɕϊ�����c�[���ł��D���炩���ߕϊ����Ă������ƂŁCJulius�̋N
       ����啝�ɍ������ł��܂��D

       Julius-4���CN-gram �͑O�����C�������C���邢�͗������w��ł���悤
       �ɂȂ�܂����Dmkbingram �ł��C�ǂ��炩��������Ńo�C�i��N-gram���쐬��
       �邱 �Ƃ��ł��܂��D�܂��C�������w�肵���ꍇ�́C�����2��N-gram�͈��
       �̃o �C�i��N-gram�Ɍ�������܂��D

       �O����N-gram�݂̂��w�肳�ꂽ�Ƃ��Cmkbingram �� �O����N-gram��������o
       �C�i��N-gram�𐶐����܂��D���̃o�C�i��N-gram���g���Ƃ��CJulius �͂���
       ���� 2-gram ���g���đ�1�p�X���s���C��2 �p �X�ł͂��̑O�����m��������
       ���̊m�����C�x�C�Y���ɏ]���ĎZ�o���Ȃ���F�����s���܂��D

       �����N-gram�݂̂��w�肳�ꂽ�Ƃ��Cmkbingram�͌�����N-gram��������o
       �C�i��N-gram�𐶐����܂��D���̃o�C�i��N-gram���g���Ƃ��CJulius �͂���
       ���̌���� 2-gram ����x�C�Y���ɏ]���ĎZ�o���Ȃ����1�p�X�̔F�����s���C
       ��2�p�X�ł͌���� N-gram���g�����F�����s���܂��D

       �������w�肳�ꂽ�Ƃ��́C�O����N-gram����2-gram�ƌ����N-gram����������
       ���o�C�i��N-gram����������܂��DJulius�ł͂��̑O����2-gram�ő�1�p�X��
       �s���C�����N-gram�ő�2�p�X���s���܂��D�Ȃ��� N-gram �͓���̃R�[�p�X
       ���瓯 ��̏����i�J�b�g�I�t�l�C�o�b�N�I�t�v�Z���@���j�Ŋw�K����Ă���C
       ����̌�b�������Ă���K�v������܂��D

       �Ȃ��Cmkbingram �� gzip ���k���ꂽ ARPA �t�@�C�������̂܂ܓǂݍ��߂�
       ���D

       �܂��C�o�[�W���� 4.2.3���o�C�i��N-gram���̕����R�[�h�̕ϊ����� �\�ɂ�
       ��܂����D

       �o�[�W���� 3.x �ȑO�ō쐬�����o�C�i��N-gram�́C���̂܂� 4.0 �ł��ǂ߂�
       ���Dmkbingram �� -d �ŗ^���邱�ƂŁC�Â��o�C�i���`�� ��V�����o�C�i���`
       ���ɕϊ����邱�Ƃ��ł��܂��D�Ȃ��C4.0 �ȍ~�� mkbingram �ō쐬�����o�C�i
       ��N-gram�t�@�C����3.x �ȑO�̃o�[�W�����ł� �g���܂���̂ł����ӂ�����
       ���D

OPTIONS
        -nlr  forward_ngram.arpa
           �O�����ileft-to-right�j��ARPA�`�� N-gram �t�@�C����ǂݍ���

        -nrl  backward_ngram.arpa
           �������iright-to-left�j��ARPA�`�� N-gram �t�@�C����ǂݍ���

        -d  old_bingram_file
           �o�C�i��N-gram��ǂݍ��ށi�Â��o�C�i���`���̕ϊ��p�j

        -swap
           �����L�� <s> �ƕ����L�� </s> �����ւ���D

        -c  from to
           �o�C�i��N-gram���̕����R�[�h��ϊ�����D�ifrom, to�͕����R�[�h��\
           ��������j

       output_bingram_file
           �o�͐�̃o�C�i��N-gram�t�@�C����

EXAMPLES
       ARPA�`���� N-gram ���o�C�i���`���ɕϊ�����i�O����+�������j�F
       ARPA�`���̑O���� 4-gram ���o�C�i���`���ɕϊ�����i�O�����̂݁j�F
       �Â��o�C�i��N-gram�t�@�C�������݂̌`���ɕϊ�����F

SEE ALSO
        julius ( 1 ) ,
        mkbinhmm ( 1 )

COPYRIGHT
       Copyright (c) 1991-2013 ���s��w �͌�������

       Copyright (c) 1997-2000 ��񏈗��U�����Ƌ���(IPA)

       Copyright (c) 2000-2005 �ޗǐ�[�Ȋw�Z�p��w�@��w ���쌤����

       Copyright (c) 2005-2013 ���É��H�Ƒ�w Julius�J���`�[��

LICENSE
       Julius �̎g�p�����ɏ����܂��D



                                  19/12/2013                      MKBINGRAM(1)

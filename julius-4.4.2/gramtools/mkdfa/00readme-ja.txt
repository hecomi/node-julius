    mkdfa.pl

MKDFA.PL(1)                                                        MKDFA.PL(1)



���O
           mkdfa.pl
          - Julius �`���̕��@���I�[�g�}�g���ɕϊ�����R���p�C��

�T�v
       mkdfa.pl [options...] {prefix}

DESCRIPTION
       mkdfa.pl �� Julius �̕��@�R���p�C���ł��D�L�q���ꂽ���@�t�@�C��
       (.grammar) �ƌ�b�t�@�C�� (.voca) ����CJulius�p�̗L����ԃI�[�g�}�g��
       �t�@�C�� (.dfa) ����єF������ (.dict) �𐶐����܂��D�J�e�S�����Ɛ���
       ��̊e�t�@�C���ŗp������J�e�S�� ID �ԍ��Ƃ̑Ή��� .term �t�@�C����
       ���ďo�͂���܂��D

       �e�t�@�C���`���̏ڍׂɂ��ẮC�ʓr�h�L�������g�������������D

       prefix �́C.grammar �t�@�C������� .voca�t�@�C���� �v���t�B�b�N�X������
       �Ƃ��ė^���܂��Dprefix.grammar�� prefix.voca����prefix.dfa�C
       prefix.dict����� prefix.term����������܂��D

       �o�[�W���� 3.5.3 �ȍ~�� Julius �ɕt���� mkdfa.pl �́C dfa_minimize ���
       ���Ŏ����I�ɌĂяo���̂ŁC �o�͂���� .dfa �͏�ɍŏ�������Ă��܂��D

OPTIONS
        -n
           �������o�͂��Ȃ��D.voca ������ .grammar �݂̂� .dfa �ɕϊ����� ����
           ���ł���D

ENVIRONMENT VARIABLES
        TMP �܂��� TEMP
           �ϊ����Ɉꎞ�t�@�C����u���f�B���N�g�����w�肷��D �w�肪�����ꍇ�C
           /tmp, /var/tmp, /WINDOWS/Temp, /WINNT/Temp �̏��ōŏ��Ɍ���������
           �����g�p�����D

EXAMPLES
       ���@�t�@�C�� foo.grammar, foo.voca�� �΂��Ĉȉ������s���邱�Ƃ�
       foo.dfa�� foo.voca�����foo.term���o�͂����D

SEE ALSO
        julius ( 1 ) ,
        generate ( 1 ) ,
        nextword ( 1 ) ,
        accept_check ( 1 ) ,
        dfa_minimize ( 1 )

DIAGNOSTICS
       mkdfa.pl �͓����� mkfa ����� dfa_minimize ���Ăяo���܂��D���s���C����
       ��̎��s�t�@ �C�����C���� mkdfa.pl �Ɠ����f�B���N�g���ɒu���Ă���K�v��
       ����܂��D ������Julius �ɓ�������Ă��܂��D

COPYRIGHT
       Copyright (c) 1991-2013 ���s��w �͌�������

       Copyright (c) 1997-2000 ��񏈗��U�����Ƌ���(IPA)

       Copyright (c) 2000-2005 �ޗǐ�[�Ȋw�Z�p��w�@��w ���쌤����

       Copyright (c) 2005-2013 ���É��H�Ƒ�w Julius�J���`�[��

LICENSE
       Julius �̎g�p�����ɏ����܂��D



                                  19/12/2013                       MKDFA.PL(1)

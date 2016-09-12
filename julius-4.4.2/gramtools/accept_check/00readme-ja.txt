    accept_check

ACCEPT_CHECK(1)                                                ACCEPT_CHECK(1)



���O
           accept_check
          - ���@�ɂ�����P���̎�/��󗝃`�F�b�N

�T�v
       accept_check [-t] [-s spname] [-v] {prefix}

DESCRIPTION
       accept_check �́C���@�ŕ��̎󗝁E��󗝂𔻒肷��c�[���ł��D���͕W��
       ���͂���^���܂��D�󗝂��ׂ�������s���e�L�X�g�t�@�C���ɂ܂Ƃ߂ď���
       �Ă����C�����accept_check �̕W�����͂ɗ^���邱�ƂŁC���̕��@
       (prefix.dfa ����� prefix.dict) �ɂ����ĖړI�̕����󗝂���邩�ǂ�����
       �o�b�`�I�Ƀ`�F�b�N�ł��܂��D

       ���s�ɂ� .dfa, .dict, .term �̊e�t�@�C�����K�v�ł��D ���炩����
       mkdfa.pl �Ő������Ă����ĉ������D

       �ΏۂƂ��镶�́C���@�̌�b�P��(.voca�̑�1�t�B�[���h)�ŋ󔒂ŋ�؂��ė^
       ���܂��D�ŏ��ƍŌ�ɂ͑����̏ꍇ silB, silE ���K�v�ł��邱�ƂɋC����
       �ĉ������D�܂��C �V���[�g�|�[�Y�P��͕��Ɋ܂߂Ȃ��ł��������D

       ����\�L�̒P�ꂪ��������ꍇ�Caccept_check �͂��̉\�ȉ��߂̑S�Ă̑g
       �ݍ��킹�ɂ��Ē��ׁC�ǂꂩ�P�̃p�^�[���ł��󗝉\�ł���Ύ󗝁C��
       �ׂẴp�^�[���Ŏ󗝕s�\�ł���Ύ󗝕s�\�Ƃ��܂��D

OPTIONS
        -t
           �P��ł͂Ȃ��J�e�S�����œ��́E�o�͂���D

        -s  spname
           �X�L�b�v���ׂ��V���[�g�|�[�Y�P��̖��O���w�肷��D (default: "sp")

        -v
           �f�o�b�O�o�́D

EXAMPLES
       vfr (�t�B�b�e�B���O�^�X�N�p���@) �ł̎��s��F

           % accept_check vfr
           Reading in dictionary...done
           Reading in DFA grammar...done
           Mapping dict item <-> DFA terminal (category)...done
           Reading in term file (optional)...done
           42 categories, 99 words
           DFA has 135 nodes and 198 arcs
           -----
           please input word sequence>silB �� �� ���� ������ silE
           wseq: silB �� �� ���� ������ silE
           cate: NS_B COLOR_N (NI|NI_AT) SURU_V KUDASAI_V NS_E
           accepted
           please input word sequence>


SEE ALSO
        mkdfa.pl ( 1 ) ,
        generate ( 1 ) ,
        nextword ( 1 )

COPYRIGHT
       Copyright (c) 1991-2013 ���s��w �͌�������

       Copyright (c) 1997-2000 ��񏈗��U�����Ƌ���(IPA)

       Copyright (c) 2000-2005 �ޗǐ�[�Ȋw�Z�p��w�@��w ���쌤����

       Copyright (c) 2005-2013 ���É��H�Ƒ�w Julius�J���`�[��

LICENSE
       Julius �̎g�p�����ɏ����܂��D



                                  19/12/2013                   ACCEPT_CHECK(1)

    nextword

NEXTWORD(1)                                                        NEXTWORD(1)



���O
           nextword
          - DFA ���@�Łi�t�����Ɂj���P���\������c�[��

�T�v
       nextword [-t] [-r] [-s spname] [-v] {prefix}

DESCRIPTION
       nextword �́Cmkdfa.pl �ɂ���ĕϊ����ꂽ DFA ���@ ��ŁC�^����ꂽ����
       ���ɑ΂��Đڑ������鎟�P��̏W�����o�͂��܂��D

       ���s�ɂ� .dfa, .dict, .term �̊e�t�@�C�����K�v�ł��D ���炩����
       mkdfa.pl �Ő������Ă����ĉ������D

       �I���ӁI mkdfa.pl �ŏo�͂���镶�@�́C���� ���@�ƈقȂ�C���̌�납��O
       �Ɍ������t�����̕��@�ƂȂ��Ă��܂��D ����́CJulius �̑�2�p�X�Ō�����
       �̒T�����s�����߂ł��D ���̂��߁Cnextword �ŗ^���镔�������t�����ƂȂ�
       �܂��D

OPTIONS
        -t
           �P��ł͂Ȃ��J�e�S�����œ��́E�o�͂���D

        -r
           �P����t���ɓ��͂���D

        -s  spname
           �X�L�b�v���ׂ��V���[�g�|�[�Y�P��̖��O���w�肷��D (default: "sp")

        -v
           �f�o�b�O�o�́D

EXAMPLES
       vfr (�t�B�b�e�B���O�^�X�N�p���@) �ł̎��s��F

           % nextword vfr
           Reading in dictionary...done
           Reading in DFA grammar...done
           Mapping dict item <-> DFA terminal (category)...done
           Reading in term file (optional)...done
           42 categories, 99 words
           DFA has 135 nodes and 198 arcs
           -----
           wseq > �� ���� ������ silE
           [wseq: �� ���� ������ silE]
           [cate: (NI|NI_AT) SURU_V KUDASAI_V NS_E]
           PREDICTED CATEGORIES/WORDS:
                       KEIDOU_A (�h�� �n�� )
                       BANGOU_N (�� )
                         HUKU_N (�� ���� ���� )
                      PATTERN_N (�`�F�b�N �c�� ���� ...)
                         GARA_N (�� )
                        KANZI_N (���� )
                          IRO_N (�F )
                        COLOR_N (�� �� �� ...)
           wseq >


SEE ALSO
        mkdfa.pl ( 1 ) ,
        generate ( 1 ) ,
        accept_check ( 1 )

COPYRIGHT
       Copyright (c) 1991-2013 ���s��w �͌�������

       Copyright (c) 1997-2000 ��񏈗��U�����Ƌ���(IPA)

       Copyright (c) 2000-2005 �ޗǐ�[�Ȋw�Z�p��w�@��w ���쌤����

       Copyright (c) 2005-2013 ���É��H�Ƒ�w Julius�J���`�[��

LICENSE
       Julius �̎g�p�����ɏ����܂��D



                                  19/12/2013                       NEXTWORD(1)

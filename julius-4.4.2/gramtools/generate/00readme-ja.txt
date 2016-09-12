    generate

GENERATE(1)                                                        GENERATE(1)



���O
           generate
          - ���@���當�������_����������

�T�v
       generate [-v] [-t] [-n num] [-s spname] {prefix}

DESCRIPTION
       generate �͕��@�ɏ]���ĕ��������_���ɐ������܂��D

       ���s�ɂ� .dfa, .dict, .term �̊e�t�@�C�����K�v�ł��D ���炩����
       mkdfa.pl �Ő������Ă����ĉ������D

OPTIONS
        -t
           �P��ł͂Ȃ��J�e�S�����ŏo�͂���D

        -n  num
           �������镶�̐����w�肷�� (default: 10)

        -s  spname
           �����ɂ����ăX�L�b�v���ׂ��V���[�g�|�[�Y�P��̖��O���w�肷��D
           (default: "sp")

        -v
           �f�o�b�O�o�́D

EXAMPLES
       vfr (�t�B�b�e�B���O�^�X�N�p���@) �ł̎��s��F

           % generate vfr
           Reading in dictionary...done
           Reading in DFA grammar...done
           Mapping dict item <-> DFA terminal (category)...done
           Reading in term file (optional)...done
           42 categories, 99 words
           DFA has 135 nodes and 198 arcs
            -----
           silB ��߂܂� silE
           silB �I�����܂� silE
           silB �V���c �� �X�[�c �� ���ꂵ�� ������ silE
           silB �X�[�c �� �J�b�^�[ �� ���� �F �� ���ꂵ�� ������ silE
           silB �������� ������ silE
           silB ���� �� �o���Ă����� ������ silE
           silB �o���Ă����� ������ silE
           silB �߂��� ������ silE
           silB �X�[�c �� �V���c �� ���ꂵ�� ������ silE
           silB �㒅 �� �� �� ���� ������ silE


SEE ALSO
        mkdfa.pl ( 1 ) ,
        generate-ngram ( 1 )

COPYRIGHT
       Copyright (c) 1991-2013 ���s��w �͌�������

       Copyright (c) 1997-2000 ��񏈗��U�����Ƌ���(IPA)

       Copyright (c) 2000-2005 �ޗǐ�[�Ȋw�Z�p��w�@��w ���쌤����

       Copyright (c) 2005-2013 ���É��H�Ƒ�w Julius�J���`�[��

LICENSE
       Julius �̎g�p�����ɏ����܂��D



                                  19/12/2013                       GENERATE(1)

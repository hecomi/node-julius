    jcontrol

JCONTROL(1)                                                        JCONTROL(1)



���O
           jcontrol
          - Julius���W���[�����[�h�p�̃T���v���N���C�A���g

�T�v
       jcontrol {hostname} [portnum]

DESCRIPTION
       jcontrol �́C���W���[�����[�h�œ��삵�Ă��� julius �ɐڑ����CAPI���
       �ăR���g���[������ȒP�ȃR���\�[���v���O�����ł��DJulius �ւ̈ꎞ��~
       ��ĊJ�Ȃǂ̃R�}���h�̑��M�C����� Julius ����̔F�����ʂ≹���C�x���g
       �� ���b�Z�[�W��M���s�����Ƃ��ł��܂��D

       �N����Cjcontrol �́C�w��z�X�g��ɂ����āu���W���[�����[�h�v�œ��쒆
       ��Julius �ɑ΂��C�ڑ������݂܂��D�ڑ��m����Cjcontrol �̓��[�U�[�����
       �R�}���h���͂���у��b�Z�[�W��M�҂���ԂƂȂ�܂��D

       jcontrol �� ���[�U�[�����͂����R�}���h�����߂��C�Ή�����API�R�}���h��
       Julius �֑��M���܂��D�܂��CJulius ����F�����ʂ���̓g���K��� �Ȃ� ��
       ���b�Z�[�W�����M����Ă����Ƃ��́C���̓��e��W���o�͂֏����o���܂��D

       ���W���[�����[�h�̎d�l�ɂ��ẮC�֘A�����������������D

OPTIONS
        hostname
           �ڑ���̃z�X�g��

        portnum
           �|�[�g�ԍ��i�f�t�H���g�F10500�j

COMMANDS
       jcontrol �͕W�����͂���1�s���R�}���h��������󂯎��D �R�}���h�̈ꗗ
       �͈ȉ��̒ʂ�D

   ���쐧��
       pause
           Julius�̔F������𒆒f�����C�ꎞ��~��ԂɈڍs������D�ꎞ��~�� ��
           �ɂ���Julius�́C���Ƃ��������͂������Ă��F���������s��Ȃ��D �����
           �Ԃ̉����F�������̓r���ł��̃R�}���h���󂯎�����ꍇ�C Julius �͂�
           �̔F���������I��������C�ꎞ��~��ԂɈڍs����D

       terminate

           pause�Ɠ������CJulius�̔F������𒆒f�����C �ꎞ��~��ԂɈڍs����
           ��D�����Ԃ̉����F�������̓r���ł��̃R �}���h���󂯎�����ꍇ�C��
           �̓��͂�j�����đ����Ɉꎞ��~��Ԃ� �ڍs����D

       resume
           Julius ���ꎞ��~��Ԃ���ʏ��Ԃֈڍs�����C�F�����ĊJ������D

       inputparam arg
           ���@�؂�ւ����ɉ������͂ł������ꍇ�̓��͒������̈������w��D
           "TERMINATE", "PAUSE", "WAIT"�̂��������ꂩ���w��D

       version
           Julius �Ƀo�[�W�����������Ԃ�����D

       status
           Julius ����V�X�e���̏�� (active / sleep) ��񍐂�����D

   ���@�E�P��F���֘A
       graminfo
           �J�����g�v���Z�X���ێ����Ă��镶�@�̈ꗗ���N���C�A���g�֏o�͂���
           ��D

       changegram prefix
           �J�����g�v���Z�X�̔F�����@�� "prefix.dfa" �� "prefix.dict" �ɓ����
           ����D�J�����g�v�� �Z�X���̕��@�͑S�ď�������C�w�肳�ꂽ���@�ɒu��
           �����D

           �J�����g�v���Z�X���Ǘ��P��F���̏ꍇ�C "prefix" �̕ς��Ɏ����t�@
           �C���݂̂� "filename.dict" �̌`�Ŏw�肷��D

       addgram prefix
           �F�����@�Ƃ��� "prefix.dfa" �� "prefix.dict" ���J�����g�v���Z�X�ɒ�
           ������D

           �J�����g�v���Z�X���Ǘ��P��F���̏ꍇ�C "prefix" �̕ς��Ɏ����t�@
           �C���݂̂� "filename.dict" �̌`�Ŏw�肷��D

       deletegram gramlist
           �J�����g�v���Z�X����w�肳�ꂽ���@���폜����D���@�̎w��́C�� �@��
           �i�ǉ����� prefix�j���C���邢�� Julius ���瑗���� GRAMINFO���ɂ�
           �镶�@ ID �Ŏw�肷��D�����̕��@���폜�������ꍇ�́C���@��������
           ��ID���J ���}�ŋ�؂��ĕ����w�肷��iID�ƕ��@�������݂��Ă��悢�j�D

       deactivategram gramlist
           �J�����g�v���Z�X�̎w�肳�ꂽ���@���ꎞ�I�ɖ����ɂ���D�����ɂ��ꂽ
           ���@�́C�G���W�����ɕێ����ꂽ�܂܁C�F����������͈ꎞ�I�ɏ��O����
           ��D ���������ꂽ���@�� activategram �ōĂїL�����ł���D

           ���@�̎w��́C���@���i�ǉ����� prefix�j���C���邢��Julius ���瑗��
           ��� GRAMINFO���ɂ��镶�@ ID �Ŏw�肷��D�� ���̕��@���w�肵������
           ���́C���@����������ID���J���}�ŋ�؂��� �����w�肷��iID�ƕ��@����
           ���݂��Ă��悢�j�D

       activategram gramlist
           �J�����g�v���Z�X�Ŗ���������Ă��镶�@��L��������D ���@�̎w��
           �́C���@���i�ǉ����� prefix�j���C���邢��Julius ���瑗�� ���
           GRAMINFO���ɂ��镶�@ ID �Ŏw�肷��D�� ���̕��@���w�肵�����ꍇ
           �́C���@����������ID���J���}�ŋ�؂��� �����w�肷��iID�ƕ��@������
           �݂��Ă��悢�j�D

       addword grammar_name_or_id dictfile
           dictfile �̒��g���C�J�����g�v���Z�X�̎w�肳�ꂽ���@�ɒǉ�����D

       syncgram
           addgram �� deletegram �Ȃǂɂ�镶�@�̍X�V�𑦎��ɍs���D �����m�F�p
           �ł���D

   �v���Z�X�֘A�̃R�}���h
       Julius-4 �ł͕������f���̓����F�����s����D���̏ꍇ�C �F���v���Z�X
       ("-SR" �Ŏw�肳�ꂽ�F�������C���X�^���X) ���ƂɃ��W���[���N���C�A���g��
       �瑀����s�����Ƃ��ł���D

       �N���C�A���g����͂ǂꂩ��̃v���Z�X���u�J�����g�v���Z�X�v�Ƃ��� ����
       ���Ă���D���@�֘A�̖��߂̓J�����g�v���Z�X�ɑ΂��čs����D

       listprocess
           Julius �Ɍ��݃G���W���ɂ���F���v���Z�X�̈ꗗ�𑗐M������D

       currentprocess procname
           �J�����g�v���Z�X���w�肳�ꂽ���O�̃v���Z�X�ɐ؂�ւ���D

       shiftprocess
           �J�����g�v���Z�X���z�؂�ւ�����D�Ă΂�邽�тɂ��̎��̃v���Z�X
           �� �J�����g�v���Z�X���؂�ւ��D

       addprocess jconffile
           �G���W���ɔF���v���Z�X��V���ɒǉ�����D�^���� jconffile �́C�ʏ��
           ���̂ƈႢ�C �������ނ� LM �ݒ���܂ނ��̂ł���K�v������D��
           ���C���ۂɑ����� �̂̓p�X���݂̂ł���C�t�@�C���ǂݍ��݂�Julius��
           �ōs���邽�߁C �t�@�C���p�X�� Julius ���猩����ꏊ���w�肷��K�v
           ���L��D

           �ǉ����ꂽ LM ����єF���v���Z�X�́Cjconffile �̖��O�� �v���Z�X����
           �Ȃ�D

       delprocess procname
           �w�肳�ꂽ���O�̔F���v���Z�X���G���W������폜����D

       deactivateprocess procname
           �w�肳�ꂽ���O�̔F���v���Z�X���C�ꎞ�I�ɖ���������D���������� ���v
           ���Z�X�͎���ȍ~�̓��͂ɑ΂��ĔF����������X�L�b�v�����D ��������
           �ꂽ�v���Z�X�� activateprocess �� �ĂїL�����ł���D

       activateprocess procname
           �w�肳�ꂽ���O�̔F���v���Z�X��L��������D

EXAMPLES
       Julius ����̃��b�Z�[�W�� "> " ���s�̐擪�ɂ��Ă��̂܂ܕW���o�͂ɏo��
       ����܂��D�ȉ��͎��s��ł��D
       ��L�̂悤�ɂ��� Julius �����W���[�����[�h�ŋN��������C jcontrol ����
       �̃z�X�g�����w�肵�ċN�����܂��D
       �������͂��s���΁C�C�x���g���e�⌋�ʂ� jcontrol ���ɑ��M����܂��D
       jcontrol �ɑ΂��ăR�}���h����͂���i�Ō�� Enter �������j�ƁC Julius
       �ɃR�}���h�����M����CJulius �����䂳��܂��D

       �ڂ����v���g�R���ɂ��ẮC�֘A�������Q�Ƃ��Ă��������D

SEE ALSO
        julius ( 1 )

COPYRIGHT
       Copyright (c) 1991-2013 ���s��w �͌�������

       Copyright (c) 1997-2000 ��񏈗��U�����Ƌ���(IPA)

       Copyright (c) 2000-2005 �ޗǐ�[�Ȋw�Z�p��w�@��w ���쌤����

       Copyright (c) 2005-2013 ���É��H�Ƒ�w Julius�J���`�[��

LICENSE
       Julius �̎g�p�����ɏ����܂��D



                                  19/12/2013                       JCONTROL(1)

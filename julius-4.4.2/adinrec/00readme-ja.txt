    adinrec

ADINREC(1)                                                          ADINREC(1)



���O
           adinrec
          - �P���b�̉������̓f�[�^���t�@�C���ɋL�^����

�T�v
       adinrec [options...] {filename}

DESCRIPTION
       adinrec �́C������Ԃ���莞�ԓ��̗�������ƃp���[�i�U�����x���j�̂���
       ���l�Ɋ�Â��Đ؂�o���C�t�@�C���ɋL�^����D�f�t�H���g�ł͕W���f�o�C�X
       ��p���ă}�C�N���͂���^�����邪�C-input �I�v�V������ �f�o�C�X��I����
       �\�ł���D�܂��v���O�C�����͂��I���ł���D

       �T���v�����O���g���͔C�ӂɐݒ�\�ł���D�^���`���� 16bit, 1 channel
       �ł���C�����o�����t�@�C���`���� Microsoft WAV �`���ł���D ���ɓ���
       ���O�̃t�@�C�������݂���ꍇ�͏㏑�������D

       �t�@�C������ "-" ���w�肷��Ǝ�荞�񂾉����f�[�^��W���o�͂֏o �͂�
       ��D���̏ꍇ�f�[�^�`���� RAW �`���ɂȂ�D

OPTIONS
       Julius �̑S�ẴI�v�V�������w��\�ł���D�w�肳�ꂽ���̂̂����C ����
       ���͂Ɋ֌W����I�v�V�����݈̂�����D�ȉ��ɁCadinrec �Ǝ��� �I�v�V����
       �Ɗ֌W���� Julius �I�v�V�����ɕ����ĉ������D

   adinrec specific options
        -freq  Hz
           �����̃T���v�����O���g�� (Hz) ���w�肷��D(default: 16,000)

        -raw
           RAW�t�@�C���`���ŏo�͂���D

   Concerning Julius options
        -input  {mic|rawfile|adinnet|stdin|netaudio|esd|alsa|oss}
           �������̓\�[�X��I������D�����g�`�t�@�C���̏ꍇ�� file���邢
           ��rawfile���w �肷��D�N����Ƀv�����v�g���\���̂ŁC����ɑ΂���
           �t�@�C ��������͂���Dadinnet �ł́C adintool �Ȃǂ̃N���C�A���g�v
           ���Z�X���特�� �f�[�^���l�b�g���[�N�o�R�Ŏ󂯎�邱�Ƃ��ł���D
           netaudio ��DatLink�̃T�[�o����C stdin�͕W�����͂��特�����͂��s
           ���D esd�́C�����f�o�C�X�̋��L��i�Ƃ��đ����� Linux�̃f�X�N�g�b�v
           ���ŗ��p����Ă��� EsounD daemon ������͂���D

        -lv  thres
           �U�����x���̂������l�D�l�� 0 ���� 32767 �͈̔͂Ŏw�肷��D
           (default: 2000)

        -zc  thres
           ��������̂������l�D�l�͂P�b������̌������Ŏw�肷��D (default:
           60)

        -headmargin  msec
           ������ԊJ�n���̃}�[�W���D�P�ʂ̓~���b�D (default: 300)

        -tailmargin  msec
           ������ԏI�����̃}�[�W���D�P�ʂ̓~���b�D (default: 400)

        -zmean
           ���͉����X�g���[���ɑ΂��Ē��������������s���D�S�Ẳ��������� �̑O
           �i�Ƃ��ď��������D

        -smpFreq  Hz
           �����̃T���v�����O���g�� (Hz) ���w�肷��D(default: 16,000)

        -48
           48kHz�œ��͂��s���C16kHz�Ƀ_�E���T���v�����O����D ����� 16kHz ��
           ���f�����g�p���Ă���Ƃ��̂ݗL���ł���D �_�E���_���v�����O�̓����@
           �\�� sptk ���� �ڐA���ꂽ�D (Rev. 4.0)

        -NA  devicename
           DatLink �T�[�o�̃f�o�C�X�� (-input netaudio).

        -adport  port_number

           -input adinnet �g�p���C�ڑ����󂯕t���� adinnet �̃{�[�g�ԍ����w��
           ����D(default: 5530)

        -nostrip
           ������荞�ݎ��C�f�o�C�X��t�@�C���ɂ���ẮC�����g�`���ɐU�� ��
           "0" �ƂȂ�t���[�������݂��邱�Ƃ�����DJulius �͒ʏ�C���� ���͂�
           �܂܂�邻�̂悤�ȃt���[������������D���̗�T���v�������� ���܂���
           ���Ȃ��ꍇ�C���̃I�v�V�������w�肷�邱�ƂŎ��������� ���������邱��
           ���ł���D

        -C  jconffile
           jconf�ݒ�t�@�C����ǂݍ��ށD�t�@�C���̓��e�����̏ꏊ�ɓW�J�����D

        -plugindir  dirlist
           �v���O�C����ǂݍ��ރf�B���N�g�����w�肷��D�����̏ꍇ�� �R�����ŋ�
           �؂��ĕ��ׂĎw�肷��D

ENVIRONMENT VARIABLES
        ALSADEV
           (�}�C�N���͂� alsa �f�o�C�X�g�p��) �^���f�o�C�X�����w�肷��D �w��
           ���Ȃ��ꍇ�� "default"�D

        AUDIODEV
           (�}�C�N���͂� oss �f�o�C�X�g�p��) �^���f�o�C�X�����w�肷��D �w�肪
           �Ȃ��ꍇ�� "/dev/dsp"�D

        PORTAUDIO_DEV
           (portaudio V19 �g�p��) �^���f�o�C�X�����w�肷��D ��̓I�Ȏw����@
           �� adinrec �̏��������Ƀ��O�ɏo�͂����̂ŎQ�Ƃ̂��ƁD

        LATENCY_MSEC
           Linux (alsa/oss) ����� Windows �ŁC�}�C�N���͎��̒x�����Ԃ��~ ���b
           �P�ʂŎw�肷��D�Z���l��ݒ肷�邱�Ƃœ��͒x�����������ł� ��
           ���CCPU �̕��ׂ��傫���Ȃ�C�܂����ɂ���Ă̓v���Z�X��OS�� ������
           �s����ɂȂ邱�Ƃ�����D�œK�Ȓl��OS ��f�o�C�X�ɑ傫�� �ˑ���
           ��D�f�t�H���g�l�͓�����Ɉˑ�����D

SEE ALSO
        julius ( 1 ) ,
        adintool ( 1 )

COPYRIGHT
       Copyright (c) 1991-2013 ���s��w �͌�������

       Copyright (c) 1997-2000 ��񏈗��U�����Ƌ���(IPA)

       Copyright (c) 2000-2005 �ޗǐ�[�Ȋw�Z�p��w�@��w ���쌤����

       Copyright (c) 2005-2013 ���É��H�Ƒ�w Julius�J���`�[��

LICENSE
       Julius �̎g�p�����ɏ����܂��D



                                  19/12/2013                        ADINREC(1)

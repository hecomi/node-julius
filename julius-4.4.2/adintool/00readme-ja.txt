    adintool

ADINTOOL(1)                                                        ADINTOOL(1)



���O
           adintool
          - �����g�`�f�[�^�̋L�^�E�����E���M�E��M�c�[��

�T�v
       adintool {-in inputdev} {-out outputdev} [options...]

DESCRIPTION
       adintool �́C�����g�`�f�[�^���̉�����Ԃ̌��o����ыL�^��A���I�ɍs ��
       �c�[���ł��D���͉����ɑ΂��ė�������ƐU�����x���Ɋ�Â�������Ԍ� �o��
       �����s���C������ԕ�����A���o�͂��܂��D

       adintool �� adinrec �̍��@�\�łł��D�����f�[�^�̓��͌��Ƃ��āC�}�C�N ��
       �́E �����g�`�t�@�C���E�W�����́E�l�b�g���[�N����(adinnet �T�[�o�[ ���[
       �h)���I���ł��܂��DJulius �� -input �I�v�V������ �g�p�\�ŁC�v���O�C��
       ���͂��I���ł��܂��D

       �o�͐�Ƃ��āC�����g�`�t�@�C���E�W���o�́E�l�b�g���[�N�o��(adinnet �N
       ���C�A���g���[�h)���I���ł��܂��D���Ƀl�b�g���[�N�o�́iadinnet �N���C
       �A���g���[�h�j�ł́C julius �փl�b�g���[�N�o�R�ŉ����𑗐M���ĉ����F��
       �����邱�Ƃ��ł��܂��D

       ���͉����͉�����Ԃ��ƂɎ�����������C�����o�͂���܂��D������Ԃ̐� ��
       �o���ɂ� adinrec �Ɠ����C��莞�ԓ��̗�������ƃp���[�i�U�����x���j ��
       �������l��p���܂��D������ԊJ�n�Ɠ����ɉ����o�͂��J�n����܂��D�o �͂�
       ���ăt�@�C���o�͂�I�񂾏ꍇ�́C�A�ԃt�@�C�����Ō��o���ꂽ��Ԃ��� �ɕ�
       �����܂��D

       �T���v�����O���g���͔C�ӂɐݒ�\�ł��D�^���`���� 16bit, 1 channel
       �ŁC�����o�����t�@�C���`���� Microsoft WAV �`���ł��D ���ɓ������O��
       �t�@�C�������݂���ꍇ�͏㏑������܂��D

OPTIONS
       Julius �̑S�ẴI�v�V�������w��\�ł���D�w�肳�ꂽ���̂̂����C ����
       ���͂Ɋ֌W����I�v�V�����݈̂�����D�ȉ��ɁCadintool �� �I�v�V��
       ���C����їL���� Julius �I�v�V�������������D

   adintool specific options
        -freq  Hz
           �����̃T���v�����O���g�� (Hz) ���w�肷��D(default: 16,000)

        -in  inputdev
           ������ǂݍ��ޓ��̓f�o�C�X���w�肷��D"mic" �Ń}�C�N���́C "file"
           �Ńt�@�C������, "stdin" �ŕW�����͂��特����ǂݍ��ށD �t�@�C������
           �̏ꍇ�C�t�@�C�����͋N����ɏo�Ă���v�����v�g�ɑ� ���Ďw�肷��D��
           ���C"adinnet" �� adintool �� adinnet �T�[�o�[ �ƂȂ�Cadinnet �N��
           �C�A���g���特���f�[�^�� tcp/ip �o�R�� �󂯎��D�|�[�g�ԍ��� 5530
           �ł���i"-inport" �ŕύX�\�j�D

           ���̓f�o�C�X�́C���̂ق� Julius �� "-input" �I�v�V�����ł��w��\
           �ł���D���̏ꍇ�C�v���O�C������̓��͂��\�ł���D

        -out  outputdev
           �������o�͂���f�o�C�X���w�肷��D"file" �Ńt�@�C���o�́C stdout ��
           �W���o�͂֏o�͂���D�t�@�C���̏ꍇ�C�o�̓t�@�C������ �I�v�V����
           "-filename" �ŗ^����D�o�̓t�@�C�� �`���� 16bit WAV �`���ł���D ��
           ���C"adinnet" �� adintool �� adinnet �N���C�A���g �ƂȂ�Cadinnet
           �T�[�o�֎�荞�񂾉����f�[�^�� tcp/ip �o�R�� ���M�ł���D���M��z�X
           �g�� "-server" �Ŏw�肷��D �|�[�g�ԍ��� 5530 �ł���i"-port" �ŕ�
           �X�\�j�D "vecnet" �ł́C�������͂��璊�o���������ʃx�N�g�����T�[
           �o�֑��M�ł���D

        -inport  num
           ���͂� adinnet �̏ꍇ (-in adinnet)�C�ڑ����󂯂�|�[�g�ԍ� ���w��
           ����D�w�肵�Ȃ��ꍇ�̃f�t�H���g�� 5530 �ł���D

        -server  [host] [,host...]
           �o�͂� adinnet �̏ꍇ (-out adinnet)�C���M��̃T�[�o�����w�肷��D
           ��������ꍇ�́C�J���}�ŋ�؂��Ďw�肷��D

        -port  [num] [,num...]
           �o�͂� adinnet �̏ꍇ (-out adinnet)�C���M��̊e�T�[�o�̃|�[�g�ԍ�
           ���w�肷��D�w�肵�Ȃ��ꍇ�̃f�t�H���g�� 5530 �ł���D -server �ŕ�
           ���̃T�[�o���w�肵�Ă���ꍇ�C �S�Ăɂ��Ė����I�Ƀ|�[�g�ԍ����w��
           ����K�v������D

        -filename  file
           �t�@�C���o�� (-out file) ���C�o�̓t�@�C������ �^����D�f�t�H���g��
           �́C���o���ꂽ������Ԍ��o���ƂɁC "file.0000.wav" ,
           "file.0001.wav" ... �̂悤�ɋ�Ԃ��ƂɘA�Ԃ� �L�^�����D�ԍ��̏���
           �l�� 0 �ł���i-startid�� �ύX�\�j�D�Ȃ��C�I�v�V���� -oneshot �w
           �莞�� �ŏ��̋�Ԃ����� "file" �̖��O�ŕۑ������D

        -startid  number
           �t�@�C���o�͎��C�L�^���J�n����A�Ԕԍ��̏����l���w�肷��D�i �f�t�H
           ���g�F0�j

        -oneshot
           �ŏ��̉�����Ԃ��I��������I������D

        -nosegment
           ���͉����̉�����Ԍ��o�i�����ɂ���؂�Ɩ�����Ԃ̃X�L�b�v�j�� �s
           ��Ȃ��D

        -raw
           RAW�t�@�C���`���ŏo�͂���D

        -autopause
           �o�͂� adinnet �̏ꍇ�i-out adinnet�j�C������Ԃ��I�����邽�� �ɓ�
           �͒�~�E�����~��ԂɈڍs����D�o�͐�� adinnet �T�[�o�� �瓮���
           �J�M��������Ɖ������͂��ĊJ����D

        -loosesync
           �o�͂� adinnet �i-out adinnet�j�ŕ����̏o�͐�T�[�o�֏o�͂��Ă���
           �ꍇ�C�����~��Ԃ��瓮��ĊJ�M���ɂ���ē�����ĊJ����
           �ہCadintool �� ���ׂẴT�[�o���瓮��ĊJ�M�����󂯂�܂œ������
           �J���Ȃ��D ���̃I�v�V�������w�肷��ƁC���Ȃ��Ƃ��P�̃T�[�o�����
           �J�M�� ������Γ�����ĊJ����悤�ɂȂ�D

        -rewind  msec
           ���͂��}�C�N�̂Ƃ��C��~��Ԃ��瓮����ĊJ����Ƃ��C��~������ ����
           ���ĉ������͒��������ꍇ�C�w�肳�ꂽ�~���b�����������̂ڂ��� �^����
           �J�n����D

        -paramtype  parameter_type
           �o�͂� vecnet �̎��i-out vecnet�j�C���o����� ���ʂ̌^��HTK�`���Ŏw
           �肷��i��:"MFCC_E_D_N_Z"�j�D

        -veclen  vector_length
           �o�͂� vecnet �̎��i-out vecnet�j�C�o�͂���x �N�g�����i�������j��
           �w�肷��D

   Concerning Julius options
        -input  {mic|rawfile|adinnet|stdin|netaudio|esd|alsa|oss}
           �������̓\�[�X��I������D"-in" �̑���ɂ�������g�����Ƃ� �ł���
           �i�Ō�Ɏw�肵���ق����D�悳���j�Desd ��v���O�C�����͂� �w��\
           �ł���D

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

EXAMPLES
       �}�C�N����̉������͂��C���b���Ƃ� "data.0000.wav" ���珇�ɋL�^����F
       ����Ȏ��^�����t�@�C�� "foobar.raw" ���C������Ԃ��Ƃ�
       "foobar.1500.wav" "foobar.1501.wav" ... �ɕ�������F
       �l�b�g���[�N�o�R�ŉ����t�@�C����]������(��Ԍ��o�Ȃ�)�F
       �}�C�N����̓��͉����� Julius �֑��M���ĔF���F

SEE ALSO
        julius ( 1 ) ,
        adinrec ( 1 )

COPYRIGHT
       Copyright (c) 1991-2013 ���s��w �͌�������

       Copyright (c) 1997-2000 ��񏈗��U�����Ƌ���(IPA)

       Copyright (c) 2000-2005 �ޗǐ�[�Ȋw�Z�p��w�@��w ���쌤����

       Copyright (c) 2005-2013 ���É��H�Ƒ�w Julius�J���`�[��

LICENSE
       Julius �̎g�p�����ɏ����܂��D



                                  19/12/2013                       ADINTOOL(1)

    julius

JULIUS(1)                                                            JULIUS(1)



���O
           julius
          - ���b�A�������F���G���W��

�T�v
       julius [-C jconffile] [options...]

���e
       Julius �͐������ΏۂƂ������b�A�������F�����s�����Ƃ̂ł���t���[
       �̔F���G���W���ł��D�P��N-gram��p����2�p�X�\���̒i�K�I�T���ɂ�荂��
       �x�ȔF�����s�����Ƃ��ł��܂��D�܂��C���K�͌�b�̂��߂̕��@�x�[�X�̔F��
       ��P�P��F�����s�����Ƃ��ł��܂��D�F���ΏۂƂ��ă}�C�N���́C�^���ς݂�
       �����g�`�t�@�C���C�������o�����p�����[�^�t�@�C���ȂǂɑΉ����Ă��܂��D

       �R�A�̔F�������́C�S�� JuliusLib ���C�u�����Ƃ��Ē񋟂���Ă��܂��D
       Julius��JuliusLib��p���鉹���A�v���P�[�V�����̈�ł��D

       Julius ��p���ĉ����F�����s���ɂ́C�������f���C�P�ꎫ���C����ь��ꃂ
       �f�����K�v�ł��D

�ݒ�
       Julius ����� JuliusLib �R�A�G���W���̐ݒ�i����I���C�ݒ�C���f���w
       ��C �p�����[�^�ύX�Ȃǁj�́C���ׂĂ����Ő�������u�I�v�V�����v�Ŏw�肷
       ��D Julius �ɑ΂��ẮC�����̃I�v�V�������R�}���h���C�������Ƃ��Ē�
       �ڎw �肷�邩�C���邢�̓e�L�X�g�t�@�C�����ɋL�q�������̂� "-C" �ɂÂ�
       �Ďw�肷��D���̃I�v�V�������L�q�����e�L�X�g�t�@�C���� "jconf �ݒ�t�@
       �C��" �ƌĂ΂��D

       JuliusLib ��p���鑼�̃A�v���P�[�V�����ɂ����Ă��CJuliusLib���̔F�� �G
       ���W���ւ̓���w��́C���l�ɂ��̃I�v�V�����Q�ōs���Djconf �ݒ�t�@�C��
       �ɐݒ���e���L�q���āC��������C���֐��̍ŏ���
       j_config_load_file_new(char *jconffile); �ŌĂяo �����ƂŁCJuliusLib
       ���̔F���G���W���ɐݒ���Z�b�g���邱�Ƃ��ł���D

       �Ȃ��Cjconf �ݒ�t�@�C�����ł́C���΃t�@�C���p�X�͂��� jconf �t�@�C��
       �̈ʒu����̑��΃p�X�Ƃ��ĉ��߂����i�J�����g�f�B���N�g���ł͂Ȃ��j�D
       ���ӂ��ꂽ���D

       �ȉ��Ɋe�I�v�V�������������D

   Julius �A�v���P�[�V�����I�v�V����
       JuliusLib �Ƃ͓Ɨ������C�A�v���P�[�V�����Ƃ��Ă� Julius �̋@�\�Ɋւ���
       �I�v�V�����ł���D�F�����ʂ̏o�́C�����R�[�h�ϊ��C���O�̐ݒ�C���W���[
       �����[�h�Ȃǂ��܂ށD�����̃I�v�V�����́CJuliusLib ��g�ݍ��񂾑��̃A
       �v���P�[�V�����ł͎g�p�ł��Ȃ��̂Œ��ӂ��邱�ƁD

        -outfile
           �F�����ʂ��ʂ̃t�@�C���ɏo�͂���D���̓t�@�C�����Ƃ̔F�����ʂ��C
           ���̊g���q�� ".out" �ɕς����t�@�C���ɕۑ�����D (rev. 4.0)

        -separatescore
           �F�����ʂŌ���X�R�A�Ɖ����X�R�A���ʂɏo�͂���D�w�肵�Ȃ���
           ���C�a�̒l���F�����ʂ̃X�R�A�Ƃ��ďo�͂����D

        -callbackdebug
           �R�[���o�b�N��JuliusLib����Ă΂ꂽ�Ƃ��ɃR�[���o�b�N���� �W���o��
           �ɏo�͂���D�f�o�b�O�p�ł���D(rev.4.0)

        -charconv  from to
           �o�͂ŕ����R�[�h�̕ϊ����s���Dfrom �͌��ꃂ�f���̕����Z�b�g���Cto
           �͏o �͂̕����Z�b�g���w�肷��D�����Z�b�g�́CLinux �ł� iconv �ŗp
           ������R�[�h���ł���DWindows �ł́C�R�[�h�y�[�W�ԍ����邢�͂���
           �Ɏ����R�[�h���̂ǂꂩ�ł���F ansi, mac, oem, utf-7, utf-8, sjis,
           euc�D

        -nocharconv
           �����R�[�h�ϊ����s��Ȃ��D-charconv �̎w��� ���Z�b�g����D

        -module  [port]
           Julius ���u�T�[�o���W���[�����[�h�v�ŋN������DTCP/IP �o�R�ŃN ���C
           �A���Ƃ��Ƃ肵�C�����̐����F�����ʁE�C�x���g�̒ʒm���s ��
           ��Dport �̓|�[�g�ԍ��ł���C�ȗ����� 10500 ���p������D

        -record  dir
           ��Ԍ��o���ꂽ���͉������t�@�C���ɒ����ۑ�����D dir�͕ۑ�����f�B
           ���N�g�����w�肷��D �t�@�C�����́C���ꂼ��̏����I�����̃V�X�e����
           �Ԃ��� YYYY.MMDD.HHMMSS.wav�̌`�ŕۑ������D�t�@ �C���`����16bit,
           1�`�����l����WAV�`���ł���D �Ȃ��C���͂�GMM���ɂ���Ċ��p���ꂽ��
           �����L�^�����D

        -logfile  file
           �ʏ� Julius �͑S�Ẵ��O�o�͂�W���o�͂ɏo�͂���D ���̃I�v�V������
           �w�肷�邱�ƂŁC�����̏o�͂��w��t�@�C���� �ؑւ��邱�Ƃ��ł�
           ��D(Rev.4.0)

        -nolog
           ���O�o�͂��֎~����D�W���o�͂ɂ͉����o�͂���Ȃ��Ȃ�D (Rev.4.0)

        -help
           �G���W���ݒ�C�I�v�V�����ꗗ�Ȃǂ̃w���v���o�͂��ďI������D

   �S�̃I�v�V����
       �S�̃I�v�V�����́C���f����T���ȊO�̃I�v�V�����ł���C �������́E����
       �o�EGMM�E�v���O�C���E���̑��̐ݒ���܂ށD �S�̃I�v�V�����́C �������f
       ��(-AM)�E���ꃂ�f��(-LM)�E�f �R�[�_(-SR)�Ȃǂ̃Z�N�V������`�̑O�ɒ�`
       ���邩�C -GLOBAL �̂��ƂɎw�肷��D

       �I�[�f�B�I����
            -input
           {mic|rawfile|mfcfile|outprob|adinnet|vecnet|stdin|netaudio|esd|alsa|oss}
               �������̓\�[�X��I������D�����g�`�t�@�C���̏ꍇ�� file���邢
               ��rawfile���w�� ����DHTK �`���� �����ʃt�@�C����F������ꍇ
               ��htkparam���邢 ��mfcfile ���w�肷��DHTK�p�����[�^�t�@�C�� ��
               �o�͊m���x�N�g���Ƃ��ēǂݍ��ޏꍇ�� outprob���w�肷��D�N����
               �Ƀv�����v�g���\�� ��̂ŁC����ɑ΂��ăt�@�C��������͂���D
               adinnet �ł́C adintool �Ȃǂ̃N���C�A���g�v���Z�X ���特���f�[
               �^���l�b�g���[�N�o�R�Ŏ󂯎�邱�Ƃ��ł���D vecnet�ł́CHTK��
               ����/�o�͊m���x�N�g�����l�b �g���[�N�o�R�Ŏ󂯎�邱�Ƃ��ł�
               ��D netaudio ��DatLink�̃T�[�o����C stdin�͕W�����͂���̉���
               ���͂�F������D esd�́C�����f�o�C�X�̋��L��i�Ƃ��đ�����
               Linux�̃f�X�N�g�b�v���ŗ��p����Ă��� EsounD daemon ����̓�
               �͂�F������D

            -filelist  filename
               (-input rawfile|mfcfile|outprob ��) filename���ɗ񋓂���Ă���
               �S�Ẵt�@ �C���ɂ��ĔF���������s���D filename�ɂ͔F�������
               �̓t�@�C���� ��1�s��1���L�q����D

            -notypecheck
               ���͂̓����ʃx�N�g���̌^�`�F�b�N�𖳌��ɂ���D�ʏ� Julius �͓�
               �͂̌^���������f���ƃ}�b�`���邩�ǂ������`�F�b�N���C�}�b�`����
               �� �ƃG���[�I������D���̃I�v�V�����͂��̃`�F�b�N���������D��
               ��炩�� ���R�Ō^�`�F�b�N�����܂����삵�Ȃ��Ƃ��Ɏg�p����D

            -48
               48kHz�œ��͂��s���C16kHz�Ƀ_�E���T���v�����O���Ȃ���F������D
               ����� 16kHz �̃��f�����g�p���Ă���Ƃ��̂ݗL���ł���D �_�E��
               �_���v�����O�̓����@�\�� sptk ���� �ڐA���ꂽ�D (Rev. 4.0)

            -NA  devicename
               DatLink �T�[�o�̃f�o�C�X�� (-input netaudio).

            -adport  port_number

               -input adinnet �g�p���C�ڑ����󂯕t���� adinnet �̃{�[�g�ԍ���
               �w�肷��D(default: 5530)

            -nostrip
               ������荞�ݎ��C�f�o�C�X��t�@�C���ɂ���ẮC�����g�`���ɐU��
               �� "0" �ƂȂ�t���[�������݂��邱�Ƃ�����DJulius �͒ʏ�C����
               ���͂Ɋ܂܂�邻�̂悤�ȃt���[������������D���̗�T���v������
               �� ���܂������Ȃ��ꍇ�C���̃I�v�V�������w�肷�邱�ƂŎ���������
               ���������邱�Ƃ��ł���D

            -zmean ,  -nozmean
               ���͉����X�g���[���ɑ΂��Ē��������������s���D�S�Ẳ���������
               �̑O�i�Ƃ��ď��������D -zmeansource�I�v�V����������D

            -lvscale  factor
               �������͂̐U���̎���X�P�[�����O

            -chunk_size
               �������͂̃o�b�t�@�����T���v�����Ŏw��\�i�f�t�H��
               �g�F1000�j�D���̒l������������ƒx�����������ł��邪�C��������
               ��ƕs����ɂȂ�D

       ���x���Ɨ�����ɂ����͌��m
            -cutsilence ,  -nocutsilence
               ���x���Ɨ�����ɂ����͌��m���s�����ǂ������w�肷��D�f�t�H��
               �g�́C���A���^�C���F���i�f�o�C�X���ړ��͂���уl�b�g���[�N��
               �́j �ł� on, �t�@�C�����͂ł� off �ł���D���̃I�v�V�������w��
               ���� ���ƂŁC�Ⴆ�Β����Ԙ^�����ꂽ�����t�@�C���ɑ΂��ĉ������
               ���o ���s���Ȃ���F�����s�����Ƃ��ł���D

            -lv  thres
               �U�����x���̂������l�D�l�� 0 ���� 32767 �͈̔͂Ŏw�肷��D
               (default: 2000)

            -zc  thres
               ��������̂������l�D�l�͂P�b������̌������Ŏw�肷��D
               (default: 60)

            -headmargin  msec
               ������ԊJ�n���̃}�[�W���D�P�ʂ̓~���b�D (default: 300)

            -tailmargin  msec
               ������ԏI�����̃}�[�W���D�P�ʂ̓~���b�D (default: 400)

       ���͊��p
           ���͒��C���邢�͕��σp���[�ɂ����͂̎�����p���s����D ���σp���[
           �ɂ����p�́C�f�t�H���g�ł͖���������Ă���C�\�[�X����R���p�C ��
           ����ۂ� configure�� --enable-power-reject ���w�肷�邱�ƂŗL���Ƃ�
           ��D ���A���^�C���F�����ŁC�������ʂŃp���[�������ꍇ�̂ݎg�p��
           ����D

            -rejectshort  msec
               ���o���ꂽ��Ԓ���msec�ȉ��̓��͂� ���p����D���̋�Ԃ̔F���͒�
               �f�E�j�������D

            -rejectlong  msec
               ���o���ꂽ��Ԓ���msec��蒷�����͂� ���p����D���̋�Ԃ̔F����
               ���f�E�j�������D

            -powerthres  thres
               �؂�o����Ԃ̕��σp���[�̂������l�D(Rev.4.0)

               ���̃I�v�V�����̓R���p�C������ --enable-power-reject���w�肳��
               ���Ƃ��� �L���ƂȂ�D

       GMM / GMM-VAD
            -gmm  hmmdefs_file
               GMM��`�t�@�C���D3��ԁi�o�͏�Ԃ��P�̂݁j��HMM�Ƃ��Ē�`��
               ��D�`����HTK�`���ŗ^����D�`����g�p�ł�������ʂ̐����͉���
               ���f���Ɠ����ł���D �Ȃ��CGMM�ŗp����MFCC�����ʂ̐ݒ�́C
               -AM_GMM�̂��Ƃɉ������f���Ɠ��l�Ɏw�肷��D�� �̓����ʐݒ�͉�
               �����f���ƕʂɁC�����I�Ɏw�肷��K�v�����邱�� �ɒ��ӂ��K�v�ł�
               ��D

            -gmmnum  number
               GMM�w�莞�C�v�Z����K�E�X���z�����w�肷��D�t���[�����Ƃ�GMM��
               �o�͊m�������߂�ہC�e���f���Œ�`����Ă��鍬���K�E�X���z�̂�
               ���C����number�Ŏw�肵�����̏�ʃK �E�X���z�̊m���݂̂��v�Z��
               ��D�����Ȓl���w�肷��ق�GMM�̌v�Z �ʂ��팸�ł��邪�C�v�Z���x
               �������Ȃ�D(default: 10)

            -gmmreject  string
               GMM�Œ�`����Ă��郂�f�����̂����C�񉹐��Ƃ��Ċ��p���ׂ����f
               ���̖��̂��w�肷��D���f�����𕡐��w�肷�邱�Ƃ��ł���D�����w
               �肷��ꍇ�́C�󔒂���ꂸ�R���}�ŋ�؂��Ĉ�� string�Ƃ��Ďw
               �肷��D

            -gmmmargin  frames
               (GMM_VAD) GMM VAD �ɂ���Ԍ��o�̊J�n���}�[�W���D�P�ʂ̓t���[
               �����Ŏw�肷��D(default: 20) (Rev. 4.0)

               ���̃I�v�V������--enable-gmm-vad�t���ŃR���p�C�� ���ꂽ�Ƃ��ɗL
               ���ƂȂ�D

            -gmmup  value
               (GMM_VAD) ������Ԃ̊J�n�Ƃ݂Ȃ� VAD �X�R�A��臒l�DVAD�X�R�A��
               (����GMM�̍ő�ޓx - �񉹐�HMM�̍ő�ޓx) �ŕ\�����D
               (Default: 0.7) (Rev.4.1)

               ���̃I�v�V������--enable-gmm-vad�t���ŃR���p�C�� ���ꂽ�Ƃ��ɗL
               ���ƂȂ�D

            -gmmdown  value
               (GMM_VAD) ������Ԃ̏I���Ƃ݂Ȃ� VAD �X�R�A��臒l�DVAD�X�R�A��
               (����GMM�̍ő�ޓx - �񉹐�HMM�̍ő�ޓx) �ŕ\�����D
               (Default: -0.2) (Rev.4.1)

               ���̃I�v�V������--enable-gmm-vad�t���ŃR���p�C�� ���ꂽ�Ƃ��ɗL
               ���ƂȂ�D

       �f�R�[�f�B���O�I�v�V����
           �f�R�[�f�B���O�I�v�V�����́C�g�p����F���A���S���Y���Ɋւ���ݒ��
           �s�� �I�v�V�����ł���D���̐ݒ�̓G���W���S�̂ɑ΂���ݒ�ł���C�S
           �Ă̔F�� �����C���X�^���X�ŋ��ʂ̐ݒ�ƂȂ�D�T���̕��⌾��d�݂Ȃ�
           �̌X�̃f�R�[ �f�B���O�ݒ�ɂ��ẮC�F�������C���X�^���X���ƂɎw
           �肷��D

            -realtime ,  -norealtime
               ���͂ƕ��s���ă��A���^�C���F�����s�����ǂ����𖾎��I�Ɏw�肷
               ��D �f�t�H���g�̐ݒ�͓��̓f�o�C�X�Ɉˑ����C�}�C�N���t�H������
               �f�o �C�X���ڔF���C�l�b�g���[�N���́C����� DatLink/NetAudio ��
               �͂� �ꍇ�� ON, �t�@�C�����͂�����ʓ��͂ɂ��Ă�OFF�ƂȂ��Ă�
               ��D

       ���̑�
            -C  jconffile
               jconf�ݒ�t�@�C����ǂݍ��ށD�t�@�C���̓��e�����̏ꏊ�ɓW�J����
               ��D

            -version
               �o�[�W��������W���G���[�o�͂ɏo�͂��ďI������D

            -setting
               �G���W���ݒ����W���G���[�o�͂ɏo�͂��ďI������D

            -quiet
               �o�͂�}������D�F�����ʂ͒P���݂̂��o�͂����D

            -debug
               (�f�o�b�O�p) ���f���̏ڍׂ�T���ߒ��̋L�^�ȂǁC�l�X�� �f�o�b�O
               �������O�ɏo�͂���D

            -check  {wchmm|trellis|triphone}
               �f�o�b�O�p�̃`�F�b�N���[�h�ɓ���D

            -plugindir  dirlist
               �v���O�C����ǂݍ��ރf�B���N�g�����w�肷��D�����̏ꍇ�� �R����
               �ŋ�؂��ĕ��ׂĎw�肷��D

            -outprobout  file
               �v�Z���ꂽ�o�͊m���s���HTK�`���t�@�C���ɕۑ��idebug�j�D

   �������f���F���̂��߂̃C���X�^���X�錾
        -AM  name
           �������f���C���X�^���X��V���ɐ錾����D�ȍ~�̉������f���Ɋւ� ���
           ��͂��̃C���X�^���X�ɑ΂�����̂Ɖ��߂����D name �ɂ̓C���X�^��
           �X�ɂ��閼�O�� �w�肷��i���ɂ��鉹�����f���C���X�^���X�Ɠ������O
           �ł����Ă͂� ���Ȃ��j. (Rev.4.0)

        -LM  name
           ���ꃂ�f���C���X�^���X��V���ɐ錾����D�ȍ~�̌��ꃂ�f���Ɋւ� ���
           ��͂��̃C���X�^���X�ɑ΂�����̂Ɖ��߂����D name �ɂ̓C���X�^��
           �X�ɂ��閼�O�� �w�肷��i���ɂ��錾�ꃂ�f���C���X�^���X�Ɠ������O
           �ł����Ă͂� ���Ȃ��j. (Rev.4.0)

        -SR  name am_name lm_name
           �F�������C���X�^���X��V���ɐ錾����D�ȍ~�̔F��������T���Ɋ� ����
           �ݒ�͂��̃C���X�^���X�ɑ΂�����̂Ɖ��߂����D name �ɂ̓C���X�^
           ���X�ɂ��閼�O�� �w�肷��i���ɂ���F�������C���X�^���X�Ɠ������O
           �ł����Ă͂��� �Ȃ��j�Dam_name, lm_name �ɂ͂��ꂼ�ꂱ�̃C���X�^��
           �X���g�p���鉹�����f���ƌ��ꃂ�f���̃C���X�^���X�𖼑O�C���邢 ��
           ID �ԍ��Ŏw�肷��D(Rev.4.0)

        -AM_GMM
           GMM�g�p���ɁCGMM�v�Z�̂��߂̓����ʒ��o�p�����[�^���C���̐錾�� ����
           �Ɏw�肷��D���� GMM �g�p���ɂ��̃I�v�V������GMM�p�̓����� �p�����[
           �^���w�肵�Ȃ������ꍇ�C�Ō�Ɏw�肵���������f���p�̓� ���ʂ����̂�
           �ܗp������D (Rev.4.0)

        -GLOBAL
           �S�̃I�v�V�����p�̃Z�N�V�������J�n����D-AM, -LM, -SR �Ȃǂ̃C���X
           �^���X �錾��p����ꍇ�C�������͐ݒ�Ȃǂ̑S�̃I�v�V�����́C�����
           �� �S�ẴC���X�^���X��`�����O���C���邢�͂��̃I�v�V������ ����
           �Ɏw�肷��K�v������D���̑S�̃I�v�V�����p�̃Z�N�V�����́C jconf ��
           �ŉ��񌻂�Ă��ǂ��D (Rev.4.1)

        -nosectioncheck ,  -sectioncheck
           �����C���X�^���X��p���� jconf �ɂ����āC�I�v�V�����̈ʒu�`�F�b �N
           �̗L���E�������w�肷��D�L���ł���ꍇ�C�����ނ̃C���X�^�� �X�̐�
           �������ꂽ���Ƃ́C���̃C���X�^���X�錾�������܂ŁC���� �C���X�^��
           �X�̃I�v�V���������w��ł��Ȃ��i��F -AM �̂��ƁC���� -AM�� -LM�Ȃ�
           �������܂ŁC�������f���I�v�V�������� �w��ł��Ȃ��D���̃I�v�V����
           �������ꂽ�ꍇ�̓G���[�ƂȂ�j�D �܂��C�S�̃I�v�V�����͑S�Ẵ��f
           ���C���X�^���X�̑O�Ɏw�肷��K �v������D�f�t�H���g�ł͗L���ɂȂ���
           ����D(Rev.4.1)

   ���ꃂ�f�� (-LM)
       ���̃O���[�v�ɂ́C�e���f���^�C�v���ƂɎw�肷��I�v�V�������܂܂�Ă�
       ��D ��̃C���X�^���X�ɂ͈�̃��f���^�C�v�������w��\�ł���D

       N-gram
            -d  bingram_file
               �g�p����N-gram���o�C�i���t�@�C���`���Ŏw�肷��D �o�C�i���`����
               �̕ϊ��� mkbingram �� �g�p����D

            -nlr  arpa_ngram_file
               �O���� (left-to-right) ��N-gram ���ꃂ�f�����w�肷��D
               arpa_ngram_file ��ARPA�W���`���̃t�@ �C���ł���K�v������D

            -nrl  arpa_ngram_file
               ������ (right-to-left) ��N-gram ���ꃂ�f�����w�肷��D
               arpa_ngram_file ��ARPA�W���`���̃t�@ �C���ł���K�v������D

            -v  dict_file
               N-gram�C�܂��͕��@�p�̒P�ꎫ���t�@�C�����w�肷��D

            -silhead  word_string  -siltail  word_string
               �������͗��[�̖�����Ԃɑ�������u�����P��v�G���g�����w�肷
               ��D �P��̓ǂ݁iN-gram�G���g�����j�C���邢��"#"+�P��ԍ��i����
               �t�@ �C���̍s�ԍ�-1�j�Ŏw�肷��D�f�t�H���g�͂��ꂼ�� "<s>",
               "</s>" �ł���D

            -mapunk  word_string
               unknown �ɑΉ�����P�ꖼ���w�肷��D�f�t�H���g�́C "<unk>" ����
               ���� "<UNK>" �ł���D���̒P��́C �F�������ɂ����� N-gram �ɂ�
               ���P����w�肵���ꍇ�Ƀ}�b�s���O���� ��P��ł���D

            -iwspword
               �|�[�Y�ɑΉ����閳���P��������ɒǉ�����D�ǉ������P��̓��e
               �� �I�v�V����-iwspentry�ŕύX�ł���D

            -iwspentry  word_entry_string

               -iwspword�w�莞�ɒǉ������P��G���g���̓��e ���w�肷��D����
               �G���g���Ɠ����`���Ŏw�肷��D(default: "<UNK> [sp] sp sp")

            -sepnum  number
               �؍\���������̍\�z���ɐ��`�o�^����P�ꐔ���w�肷��D(default:
               150)

            -adddict  dicfile
               �N�����Ɏ�����ǉ��œǂݍ��ށD

            -addword  entry_string
               �N�����ɒP��G���g����ǉ��œǂݍ��ށD

       ���@
           -gram��-gramlist�ŕ��@�𕡐���w�肷 �邱�ƂŁC��̃C���X�^���X��
           �ŕ����̕��@��p���邱�Ƃ��ł���D �i��Julius�̃I�v�V���� -dfa, -v
           �� �g�����͒P��̕��@�̂ݎw��\�ł���j

            -gram  gramprefix1[,gramprefix2[,gramprefix3,...]]
               �F���Ɏg�p���镶�@���w�肷��D���@�̓t�@�C���i��������э\����
               �� �L���I�[�g�}�g���j�̃v���t�B�b�N�X�Ŏw�肷��D���Ȃ킿�C����
               �F ���p���@��dir/foo.dict�� dir/foo.dfa �Ƃ��Ă���Ƃ��C
               dir/foo�̂悤�Ɋg���q���Ȃ������O�Ŏw�肷��D ���@�̓R���}�ŋ�
               �؂��ĕ����w�肷�邱�Ƃ��ł���D�܂��J��Ԃ� �g�p���邱�Ƃł���
               ���w��ł���D

            -gramlist  list_file
               �F���Ɏg�p���镶�@�̃��X�g���t�@�C���Ŏw�肷��D list_file��
               �́C -gram �Ɠ��l�̕��@�v���t�B�b�N�X��1�s�ɂP�� �L�q��
               ��D�܂��C���̃I�v�V�������J��Ԃ��g�p���邱�ƂŁC���� �̃��X�g
               �t�@�C�����w��ł���D�Ȃ��C���X�g���ŕ��@�𑊑΃p�X�� �w�肵��
               �ꍇ�C����́C���̃��X�g�t�@�C������̑��΃p�X�Ƃ��ĉ� �߂����
               ���Ƃɒ��ӂ��K�v�ł���D

            -dfa  dfa_file  -v  dict_file
               �F���Ɏg�p���镶�@�̍\������I�[�g�}�g���Ǝ��������ꂼ��w�肷
               ��D (Julius-3.x �Ƃ̌݊����̂��߂̌Â��I�v�V�����ł���C�g�p��
               �ׂ��łȂ��j

            -nogram
               ����܂ł� -gram�C-gramlist�C -dfa ����� -v �� �w�肳�ꂽ���@
               �̃��X�g���N���A���C���@�̎w��Ȃ��̏�� �ɂ���D

            -adddict  dicfile
               �N�����Ɏ�����ǉ��œǂݍ��ށD

            -addword  entry_string
               �N�����ɒP��G���g����ǉ��œǂݍ��ށD

       �P�P��
            -w  dict_file
               �P�P��F���ŗp����P�ꎫ�����w�肷��D�t�@�C���`���͒P��N-gram
               �╶�@�Ɠ���ł���D������̑S�Ă̒P�ꂪ�F���ΏۂƂȂ�D
               (Rev.4.0)

            -wlist  list_file
               �P�ꎫ���̃��X�g���w�肷��Dlist_file �ɂ�1�s�Ɉ���C�g�p��
               ��P�ꎫ���̃p�X���L�q����D���΃p�X�� �p�����ꍇ�C����͂�
               ��list_file���� �̑��΃p�X�Ƃ��ĉ��߂����D (Rev.4.0)

            -nogram
               ����܂ł� -w ���邢�� -wlist�� �w�肳�ꂽ�����̃��X�g���N���A
               ���C�w��Ȃ��̏�Ԃɖ߂��D

            -wsil  head_sil_model_name tail_sil_model_name sil_context_name
               �P�P��F�����C�������̗͂��[�̖������f������т��̃R���e�L�X�g
               �����w�肷��D sil_context_name�Ƃ��� NULL���w�肵���ꍇ�C�e��
               �f���������̂܂� �R���e�L�X�g�Ƃ��ėp������D

            -adddict  dicfile
               �N�����Ɏ�����ǉ��œǂݍ��ށD

            -addword  entry_string
               �N�����ɒP��G���g����ǉ��œǂݍ��ށD

       User-defined LM
            -userlm
               �v���O�������̃��[�U��`����X�R�A�v�Z�֐����g�p���邱�Ƃ�錾
               ����D(Rev.4.0)

       ���̑��̌��ꃂ�f���֘A
            -forcedict
               �P�ꎫ���ǂݍ��ݎ��̃G���[�𖳎�����D�ʏ�Julius�͒P�ꎫ������
               �G���[���������ꍇ�����œ�����~���邪�C���̃I�v�V������ �w��
               ���邱�ƂŁC�G���[�̐�����P����X�L�b�v���ď����𑱍s���� ����
               ���ł���D

   �������f���E�����ʒ��o (-AM) (-AM_GMM)
       �������f���I�v�V�����́C�������f������ѓ����ʒ��o�E�t�����g�G���h����
       �Ɋւ���ݒ���s���D�����ʒ��o�C���K�������C�X�y�N�g���T�u�g���N�V����
       �� �w��������ōs���D

       ����HMM�֘A
            -h  hmmdef_file
               �g�p����HMM��`�t�@�C���D HTK �� ASCII �` ���t�@�C���C���邢
               ��Julius �o�C�i���`���̃t�@�C���̂ǂ��炩�� �w�肷��D�o�C�i��
               �`���ւ� mkbinhmm �� �ϊ��ł���D

            -hlist  hmmlist_file
               HMMlist�t�@�C�����w�肷��D�e�L�X�g�`���C���邢�̓o�C�i���`��
               �̂ǂ��炩���w�肷��D�o�C�i���`���ւ� mkbinhmmlist �ŕϊ��ł�
               ��D

            -tmix  number
               Gaussianpruning �̌v�Z��Ԑ����w�肷��D���� ���قǌv�Z��������
               �邪�C�����ޓx�̌덷���傫���Ȃ�DSee also -gprune. (default:
               2)

            -spmodel  name
               �����̃V���[�g�|�[�Y�ɑΉ����鉹�CHMM�̖��O���w�肷��D���̃|�[
               �Y ���f�����́C-iwsp, -spsegment, -pausemodels�Ɋ֌W����D��
               ���C���@�g�p���� �X�L�b�v�\�ȃ|�[�Y�P��G���g���̎��ʂɂ��p��
               ����D (default: "sp")

            -multipath   -nomultipath
               ��ԊԑJ�ڂ��g������}���`�p�X���[�h��L���ɂ���D�I�v�V�����w
               �肪�Ȃ��ꍇ�CJulius �͉������f���̑J�ڂ��`�F�b�N���C�K�v�ł�
               ��Ύ����I�Ƀ}���`�p�X���[�h��L���ɂ���D���̃I�v�V����
               �́C���[ �U�������I�Ƀ��[�h���w�肵�����ꍇ�Ɏg�p����D

               ���̋@�\�� 3.x �ł̓R���p�C�����I�v�V�����ł��������C4.0 ���
               ���s���I�v�V�����ƂȂ����D(rev.4.0)

            -gprune  {safe|heuristic|beam|none|default}
               �g�p���� Gaussian pruning �A���S���Y����I������D none���w�肷
               ��� Gaussian pruning �𖳌��� �����ׂẴK�E�X���z�ɂ��Č���
               �Ɍv�Z����D safe �͏�� N ���v�Z����D heuristic �� beam
               ��safe �ɔ�ׂĂ��ϋɓI�Ȏ}������s�����ߌv�Z�ʍ팸�̌��ʂ���
               �������C �F�����x�̒ቺ�������\��������Ddefault�� �w�肳�ꂽ
               �ꍇ�C�f�t�H���g�̎�@���g���D(default: tied-mixture model �̏�
               ���Cstandard �ݒ�ł�safe�Cfast�� ��ł�beam�Dtied-mixture �ł�
               ���ꍇ none).

            -iwcd1  {max|avg|best number}
               ��1�p�X�̒P��ԃg���C�t�H���v�Z�@���w�肷��D max �w�莞�C����
               �R���e�L�X�g�̃g���C�t�H���W���� �S�ޓx�̍ő�l���ߎ��ޓx�Ƃ���
               �p����Davg �� �S�ޓx�̕��ϒl��p����Dbest number �͏�� N ��
               �� �g���C�t�H���̕��ϒl��p����D �f�t�H���g�́C�ꏏ�Ɏg�p����
               �錾�ꃂ�f���Ɉˑ�����DN-gram�g�p ���ɂ� best 3�C���@�g�p����
               avg�ƂȂ�D�������̉������f�����قȂ�^�C�v�� �����̌��ꃂ�f��
               �ŋ��L�����ꍇ�́C��ɒ�`���ꂽ�ق��̃f�t�H���g�� �f�t�H���g
               �l�Ƃ��ėp������D

            -iwsppenalty  float

               -iwsp�ɂ���Ė����ɕt�������P�ꖖ�V���[�g�|�[ �Y�̑}���y�i��
               �e�B�D�����Ŏw�肵���l���C�ʏ�P��̖�������P�� ���V���[�g�|�[
               �Y�ւ̑J�ڂɒǉ������D

            -gshmm  hmmdef_file
               Gaussian Mixture Selection �p�̃��m�t�H���������f�����w�肷��D
               GMS�p���m�t�H���͒ʏ�̃��m�t�H������ mkgshmm �ɂ���Đ����ł�
               ��D

            -gsnum  number
               GMS �g�p���C�Ή�����g���C�t�H�����ڍ׌v�Z���郂�m�t�H���� ���
               �̐����w�肷��D (default: 24)

       �����ʒ��o�p�����[�^
            -smpPeriod  period
               �����̃T���v�����O�������w�肷��D�P�ʂ́C100�i�m�b�̒P�ʂŎw
               �肷��D�T���v�����O������ -smpFreq �ł��w�� �\�D(default:
               625)

               ���̃I�v�V������ HTK �� SOURCERATE �ɑΉ�����D�����l���w��ł�
               ��D

               �����̉������f����p����ꍇ�C�S�C���X�^���X�ŋ��ʂ̒l���w�肷
               ��K�v ������D

            -smpFreq  Hz
               �����̃T���v�����O���g�� (Hz) ���w�肷��D(default: 16,000)

               �����̉������f����p����ꍇ�C�S�C���X�^���X�ŋ��ʂ̒l���w�肷
               ��K�v ������D

            -fsize  sample_num
               ���T�C�Y���T���v�����Ŏw�� (default: 400)�D

               ���̃I�v�V������ HTK �� WINDOWSIZE �ɑΉ�����D�������l��HTK��
               �قȂ�C(HTK�̒l / smpPeriod) �ƂȂ�D

               �����̉������f����p����ꍇ�C�S�C���X�^���X�ŋ��ʂ̒l���w�肷
               ��K�v ������D

            -fshift  sample_num
               �t���[���V�t�g�����T���v�����Ŏw�� (default: 160)�D

               ���̃I�v�V������ HTK �� TARGETRATE �ɑΉ�����D�������l��HTK��
               �قȂ�C(HTK�̒l / smpPeriod) �ƂȂ�D

               �����̉������f����p����ꍇ�C�S�C���X�^���X�ŋ��ʂ̒l���w�肷
               ��K�v ������D

            -preemph  float
               �v���G���t�@�V�X�W�� (default: 0.97)

               ���̃I�v�V������ HTK �� PREEMCOEF �ɑΉ�����D�����l���w��ł�
               ��D

            -fbank  num
               �t�B���^�o���N�`�����l�����D(default: 24)

               ���̃I�v�V������ HTK �� NUMCHANS �ɑΉ�����D�����l���w��ł�
               ��D�w�肵�Ȃ��Ƃ��̃f�t�H���g�l�� HTK�ƈقȂ��Ă��邱�Ƃɒ���
               �iHTK�ł�22�j�D

            -ceplif  num
               �P�v�X�g�����̃��t�^�����O�W��. (default: 22)

               ���̃I�v�V������ HTK �� CEPLIFTER �ɑΉ�����D�����l���w��ł�
               ��D

            -rawe ,  -norawe
               �G�l���M�[���̒l�Ƃ��āC�v���G���t�@�V�X�O�� raw energy ���g�p
               ���� / ���Ȃ� (default: disabled=�g�p���Ȃ�)

               ���̃I�v�V������ HTK �� RAWENERGY �ɑΉ�����D �w�肵�Ȃ��Ƃ���
               �f�t�H���g��HTK�ƈقȂ��Ă��邱�Ƃɒ��ӁiHTK�� ��enabled)�D

            -enormal ,  -noenormal
               �G�l���M�[���̒l�Ƃ��āC���b�S�̂̕��ςŐ��K���������K���G�l��
               �M�[ ��p���邩�ǂ������w�肷��D(default: -noenormal)

               ���̃I�v�V������ HTK �� ENORMALISE �ɑΉ�����D �w�肵�Ȃ��Ƃ�
               �̃f�t�H���g��HTK�ƈقȂ��Ă��邱�Ƃɒ��ӁiHTK�� ��enabled)�D

            -escale  float_scale
               �G�l���M�[���K�����́C�ΐ��G�l���M�[���̃X�P�[�����O�W���D
               (default: 1.0)

               ���̃I�v�V������ HTK �� ESCALE �ɑΉ�����D�f�t�H���g�l��HTK��
               �قȂ��Ă��邱�Ƃɒ��ӁiHTK�ł� 0.1�j�D

            -silfloor  float
               �G�l���M�[���K�����́C�������̃G�l���M�[�̃t���A�����O�l�D
               (default: 50.0)

               ���̃I�v�V������ HTK �� SILFLOOR �ɑΉ�����D�����l���w��ł�
               ��D

            -delwin  frame
               �ꎟ�����v�Z�p�̃E�B���h�E�t���[�����D(default: 2)

               ���̃I�v�V������ HTK �� DELTAWINDOW �ɑΉ�����D�����l���w���
               ����D

            -accwin  frame
               �񎟍����v�Z�p�̃E�B���h�E�t���[�����D(default: 2)

               ���̃I�v�V������ HTK �� ACCWINDOW �ɑΉ�����D�����l���w��ł�
               ��D

            -hifreq  Hz
               MFCC�̃t�B���^�o���N�v�Z���ɂ�����o���h������L��������D����
               �I�v�V�����ł̓J�b�g�I�t���g���̏���l���w�肷��D -1 ���w�肷
               �邱�ƂŖ������ł���D(default: -1)

               ���̃I�v�V������ HTK �� HIFREQ �ɑΉ�����D�����l���w��ł���D

            -lofreq  Hz
               MFCC�̃t�B���^�o���N�v�Z���ɂ�����o���h������L��������D����
               �I�v�V�����ł̓J�b�g�I�t���g���̉����l���w�肷��D -1 ���w�肷
               �邱�ƂŖ������ł���D(default: -1)

               ���̃I�v�V������ HTK �� LOFREQ �ɑΉ�����D�����l���w��ł���D

            -zmeanframe ,  -nozmeanframe
               ���P�ʂ̒�������������L�����^����������D (default: disabled)

               ���̃I�v�V������ HTK �� ZMEANSOURCE �ɑΉ�����D-zmean ���Q�Ƃ�
               ���ƁD

            -usepower
               �t�B���^�o���N��͂ŐU���̑���Ƀp���[���g���D(default:
               disabled)

       ���K������
            -cvn
               �P�v�X�g�������U���K�� (cepstral variance normalization; CVN)
               ��L���ɂ���D�t�@�C�����͂ł́C���͑S�̂̕��U�Ɋ�Â��Đ��K��
               ���s����D���ړ��͂ł͂��炩���ߕ��U�������Ȃ����߁C�Ō��
               ���͂̕��U�ő�p�����D�����M�����͂ł̂ݗL���ł���D

            -vtln  alpha lowcut hicut
               ���g�����[�s���O���s���D���������K�� (vocal tract length
               normalization; VTLN) �Ɏg�p�ł���D�����͂��ꂼ�ꃏ�[�s���O �W
               ���C���g����[�C���g�����[�ł���CHTK�ݒ��
               WARPFREQ�CWARPHCUTOFF ����� WARPLCUTOFF �ɑΉ�����D

            -cmnload  file
               �N�����ɃP�v�X�g�������σx�N�g���� file����ǂݍ��ށD�t�@�C����
               -cmnsave �ŕۑ����ꂽ�t�@�C�����w�肷��D���� �� MAP-CMN �ɂ���
               �āC�N����ŏ��̔��b�ɂ����ăP�v�X�g�������� �̏����l�Ƃ��ėp��
               ����D�ʏ�C2���b�ڈȍ~�͏����l�́C���O�� ���͂̕��ςɍX�V��
               ��邪�C-cmnnoupdate���w�� ���ꂽ�ꍇ�C��ɂ��̃t�@�C���̒l���e
               ���b�̏����l�Ƃ��ėp����� ��D

            -cmnsave  file
               �F�����Ɍv�Z�����P�v�X�g�������σx�N�g���� file�֕ۑ�����D����
               �Ƀt�@�C������ ��ꍇ�͏㏑�������D���̕ۑ��͉������͂��s���
               �邽�тɏ㏑���� �s����D

            -cmnupdate   -cmnnoupdate
               �����ԔF�����C�����P�v�X�g�������ς���͂��ƂɍX�V���邩�ǂ���
               ���w�肷��D�ʏ�͗L�� (-cmnupdate) �ł���C �ߋ�5�b�Ԃ̓��͂�
               ���ς������l�Ƃ��čX�V����D -cmnnoupdate ���w�肳�ꂽ�ꍇ�C�X
               �V�͍s��ꂸ�C �����l�͋N�����̒l�ɌŒ肳���D-cmnload �ŏ���
               �l ���w�肷�邱�ƂŁC��ɓ��������l���g���悤�ɂ��邱�Ƃ��ł�
               ��D

            -cmnmapweight  float
               MAP-CMN �̏����P�v�X�g�������ςւ̏d�݂��w�肷��D�l���傫����
               �Ǐ����l�ɒ����Ԉˑ����C�������قǑ��������͂̃P�v�X�g��������
               ��p����悤�ɂȂ�D(default: 100.0)

       �t�����g�G���h����
            -sscalc
               ���͐擪�̖�������p���āC���͑S�̂ɑ΂��ăX�y�N�g���T�u�g���N
               �V�������s���D�擪���̒�����-sscalclen�Ŏw�肷��D �t�@�C������
               �ɑ΂��Ă̂ݗL���ł���D-ssload �� �����Ɏw��ł��Ȃ��D

            -sscalclen  msec

               -sscalc�I�v�V�����w�莞�C�e�t�@�C���ɂ����� �m�C�Y�X�y�N�g����
               ����ɗp���钷�����~���b�Ŏw�肷��D(default: 300)

            -ssload  file
               �m�C�Y�X�y�N�g����file����ǂݍ��� �ŃX�y�N�g���T�u�g���N�V����
               ���s���D file�͂��炩���� mkss�ō쐬����D�}�C�N���́E�l�b�g
               ���[�N�� �͂Ȃǂ̃I�����C�����͂ł��K�p�ł���D-sscalc�� ������
               �w��ł��Ȃ��D

            -ssalpha  float

               -sscalc�����-ssload�p�� ���Z�W�����w�肷��D���̒l���傫���ق�
               �����X�y�N�g�����Z���s�����C ���Z��̐M���̘c���傫����
               ��D(default: 2.0)

            -ssfloor  float
               �X�y�N�g���T�u�g���N�V�����̃t���A�����O�W�����w�肷��D�X�y�N
               �g�����Z���C�v�Z�̌��ʃp���[�X�y�N�g�������ƂȂ��Ă��܂��ш��
               �΂��ẮC���M���ɂ��̌W�����悶���X�y�N�g�������蓖�Ă���D
               (default: 0.5)

       ���̑��̉������f���֘A�I�v�V����
            -htkconf  file

               HTK Config �t�@�C������͂��āC�Ή���������ʒ��o�I�v�V������
               Julius �Ɏ����ݒ肷��Dfile �� HTK �ŉ������f���w�K���Ɏg�p����
               Config �t�@�C�����w�肷��D�Ȃ��C Julius �� HTK �ł̓p�����[�^
               �̃f�t�H���g�l���ꕔ�قȂ邪�C ���̃I�v�V�������g�p����ꍇ�C�f
               �t�H���g�l�� HTK �̃f�t�H���g �ɐؑւ������D

   �F�������E�T�� (-SR)
       �F�������E�T���I�v�V�����́C��1�p�X�E��2�p�X�p�̃r�[�����⌾��d�݂̃p
       �����[�^�C�V���[�g�|�[�Y�Z�O�����e�[�V�����̐ݒ�C�P�ꃉ�e�B�X�ECN �o��
       �p�ݒ�Cforced alignment �̎w��C���̑��̔F�������ƌ��ʏo�͂Ɋւ���p��
       ���[�^���܂ށD

       ��1�p�X�p�����[�^
            -lmp  weight penalty
               (N-gram�g�p��) ��1�p�X�p�̌���X�R�A�d�݂���ё}���y�i���e�B�D
               �y�i���e�B�͕��ł���ΒP��}����}�����C���ł���ΒP��}����
               �i����D

            -penalty1  penalty
               (���@�g�p��) ��1�p�X�p�̒P��}���y�i���e�B�D (default: 0.0)

            -b  width
               ��1�p�X�T���̎}���� (rank pruning) �̃r�[�������w�肷��D�P��
               �� HMM �m�[�h���ł���D �f�t�H���g�l�͉������f����G���W���̐�
               ��ɂ��D���m�t�H�� �g�p����400, �g���C�t�H���g�p����800�C�g��
               �C�t�H���ł��� setup=v2.1 �̂Ƃ��� 1000 �ƂȂ�D

            -bs  width
               ��1�p�X��score pruning�̃X�R�A�����w�肷��Drank pruning�i-b
               width�j�ƕ��p�\�D�f�t�H���g�̓I�t�D

            -nlimit  num
               ��1�p�X�Ńm�[�h���Ƃɕێ����鉼���g�[�N���̍ő吔�D�ʏ�� 1 ��
               �Œ肳��Ă���ύX�ł��Ȃ��D�R���p�C������ configure��
               --enable-wpair����� --enable-wpair-nlimit ���w�肳��Ă���Ƃ�
               �̂ݕύX�ł���D

            -progout
               ��1�p�X�ŁC��莞�Ԃ����ɂ��̎��_�ł̍Ŗމ����n����o�͂���D

            -proginterval  msec

               -progout�̏o�̓C���^�[�o�����~���b�Ŏw�肷��D (default: 300)

       ��2�p�X�p�����[�^
            -lmp2  weight penalty
               (N-gram�g�p��) ��2�p�X�p�̌���X�R�A�d�݂���ё}���y�i���e�B�D
               �y�i���e�B�͕��ł���ΒP��}����}�����C���ł���ΒP��}����
               �i����D

            -penalty2  penalty
               (���@�g�p��) ��2�p�X�p�̒P��}���y�i���e�B�D (default: 0.0)

            -b2  width
               ��2�p�X�T���ɂ����鉼���W�J�񐔂̏�����w�肷��D�P�ʂ� ����
               ���D(default: 30)

            -sb  float
               ��2�p�X�̉����ޓx�v�Z���̃X�R�A�����w�肷��D�P�ʂ͑ΐ��ޓx��
               �ł���D(default: 80.0)

            -s  num
               �����̃X�^�b�N�T�C�Y���w�肷��D(default: 500)

            -n  num

               num�̕������������t����܂ŒT���� �s���D����ꂽ�����̓X�R�A
               �Ń\�[�g����ďo�͂���� �i-output������j�D�f�t�H���g�l�̓R��
               �p�C���� �̃G���W���ݒ�ɂ���ĕς��Cfast �łł� 1,
               standard�łł�10 �ł���D

            -output  num
               ��������N-best���̂����C���ʂƂ��ďo�͂��镶�����̐��� �w��
               ����D-n���Q�Ƃ̂��ƁD(default: 1)

            -m  count
               �T���ł��؂�̂��߂̉����W�J�񐔂̂������l���w�肷��D
               (default: 2000)

            -lookuprange  frame
               ��2�p�X�̒P��W�J���ɁC�ڑ������鎟�P��������t���邽�߂� �I
               �[�����̋��e�����t���[�����Ŏw�肷��D�l��傫������ق� ���̎�
               �ӂ̑����̉��������P����Ƃ��ĉ����W�J���s����悤�� �Ȃ�
               ���C�T�����O�ɐi�݂ɂ����Ȃ邱�Ƃ�����D(default: 5)

            -looktrellis
               �����W�J���1�p�X�̌��ʒP��g�����X��ɍi��D

       �V���[�g�|�[�Y�Z�O�����e�[�V����
            -spsegment
               �V���[�g�|�[�Y�Z�O�����e�[�V������L���ɂ���D (Rev.4.0)

            -spdur  frame
               ������Ԕ���̂��߂̂������l���w�肷��D�����P�ꂪ��ʉ����Ƃ�
               ��t���[�������̒l�ȏ㑱�����Ƃ��C������ԂƂ��ē��͂���؂��
               ��D(default: 10)

            -pausemodels  string
               �u�����P��v���`���邽�߂̉������f���ɂ����閳�����f���̖��O
               ���w�肷��D�R���}�ŋ�؂��ĕ����̖��O���w��ł���D ���̃I�v
               �V�������w�肳��Ȃ��ꍇ�C���@��p�����F���ł� -spmodel �Ŏw��
               ����郂�f���݂̂�ǂ݂Ƃ���P �ꂪ�����P��Ƃ����D��
               ���CN-gram�ł͂���ɉ����� -silhead ����� -siltail �� �w�肳��
               ��P��������P��Ƃ��Ĉ�����D(Rev.4.0)

            -spmargin  frame
               �f�R�[�_�x�[�XVAD�ɂ����āC�A�b�v�g���K���̊��߂������t���[��
               ���Ŏw�肷��D(Rev.4.0)

               ���̃I�v�V������configure�� --enable-decoder-vad��t���ăR���p
               �C�������Ƃ� �̂ݗL���ł���D

            -spdelay  frame
               �f�R�[�_�x�[�XVAD�ɂ����āC�A�b�v�g���K����̒x�������t���[��
               ���Ŏw�肷��D(Rev.4.0)

               ���̃I�v�V������configure�� --enable-decoder-vad��t���ăR���p
               �C�������Ƃ� �̂ݗL���ł���D

       �P�ꃉ�e�B�X / confusion network �o��
            -lattice ,  -nolattice
               �P��O���t�i���e�B�X�j�̏o�͂�L����/����������D

            -confnet ,  -noconfnet
               Confusion network �̏o�͂�L����/����������Dconfusion network
               �͒P��O���t���琶������邽�߁C�L�����͓����� -lattice ���L��
               �������D(Rev.4.0)

            -graphrange  frame
               �O���t�����ɂ����ċߖT�̓���P�ꉼ�����}�[�W����D�J�n�t���[��
               ����яI���t���[���̈ʒu�̍������ꂼ�� frame�ȉ��̓���P�ꉼ��
               �ɂ��ă}�[ �W����D���̍ہC�X�R�A�͍����ق��̂��̂��c����
               ��D�l�� -1 �̏� ���C�}�[�W�͈�؍s���Ȃ��D�l��傫������ق�
               �R���p�N�g�ȃO�� �t����������邪�C�X�R�A�̌덷���傫���Ȃ�D��
               �̃I�v�V������ -confnet�ɂ��e������D(default: 0)

            -graphcut  depth
               �������ꂽ�O���t�ɑ΂��āC�[���ɂ��J�b�g�I�t���s���D
               depth�́C����t���[���ɂ����đ��݉� �\�ȒP�ꐔ�̏�����w�肷
               ��DJulius �ł́C��2�p�X�̒T�����s���� �ȏꍇ�C�ꕔ�����ɒ[�ɐ[
               ���O���t����������邱�Ƃ��H�ɂ���C�� �̃I�v�V�����ɂ���Ă���
               ��}�����邱�Ƃ��ł���D-1 ���w�肷�� ���Ƃł��̋@�\�͖�������
               ���D(default: 80)

            -graphboundloop  count
               ����I�ɍs����P��O���t�̋��E���Ԓ����ɂ����āC�U���ɂ�� ��
               �����[�v��h�����߂̑ł��؂�l���w�肷��D(default: 20)

            -graphsearchdelay ,  -nographsearchdelay
               ����O���t�����p�ɃA���S���Y�����`���[�j���O����D���̃I�v�V��
               �����L�����CJulius �͑�1��������������O�̃O���t�������̉���
               ���f���s��Ȃ��悤�ɁC�O���t�����A���S���Y����ύX����D����
               �́C �r�[������T���͈͂��ɒ[�ɑ傫�����ċ���ȃ��[�h�O���t��
               ���� �悤�Ƃ���Ƃ��ɁC�O���t�̐��x�����P���邱�Ƃ���
               ��D(default: disabled)

       �������@/���������F��
           ���@��P�P��F���ɂ����āC��̃C���X�^���X�ŕ����̕��@�⎫����p
           ���� �ꍇ�Ɏw��ł���I�v�V�����ł���D

            -multigramout ,  -nomultigramout
               �������@���邢�͕���������p���ĔF�����s���ꍇ�C�ʏ�� Julius
               �͑S�Ă̕��@/�����̒�����Ŗމ������o�͂���D���̃I�v�V������
               �w�肷�邱�ƂŁC�^����ꂽ�X�̕��@/�������ƂɈ�ʉ����� �o��
               ���邱�Ƃ��ł���D(default: disabled)

       Forced alignment
            -walign
               �F�����ʂ�p���āC���͂ɑ΂���P��P�ʂ� forced alignment ���s
               ���D�P��̋��E�t���[���ƕ��ω����ޓx���o�͂����D

            -palign
               �F�����ʂ�p���āC���͂ɑ΂��鉹�f�P�ʂ� forced alignment ���s
               ���D���f���Ƃ̋��E�t���[���ƕ��ω����ޓx���o�͂����D

            -salign
               �F�����ʂ�p���āC���͂ɑ΂���HMM�̏�ԒP�ʂ� forced alignment
               ���s���D��Ԃ��Ƃ̋��E�t���[���ƕ��ω����ޓx���o�͂����D

       ���̑�
            -inactive
               �F�������C���X�^���X���ꎞ��~��� (inactive state) �ŋN����
               ��D (Rev.4.0)

            -1pass
               ��1�p�X�݂̂����s����D���̃I�v�V�������w�肵���ꍇ�C��2�p�X��
               ���s����Ȃ��D

            -fallback1pass
               �ʏ�C��2�p�X�̒T�������s�����Ƃ��CJulius �͔F�����ʖ����ŏI��
               ����D���̃I�v�V�������w�肷�邱�ƂŁC���̂悤�ȑ�2�p�X�̎��s��
               �ɁC ��1�p�X�̍Ŗމ������ŏI���ʂƂ��ďo�͂��邱�Ƃ��ł���D �i
               �����Julius-3.x�ł̃f�t�H���g�̐U�镑���ł���j

            -no_ccd ,  -force_ccd
               �������f�������f�R���e�L�X�g�ˑ����f���Ƃ��Ĉ������ǂ����𖾎�
               �I�Ɏw�肷��D�f�t�H���g��HMM���̃��f�������玩�����f�����D


            -cmalpha  float
               �m�M�x�v�Z�̂��߂̃X�R�A�̃X���[�W���O�W���D(default: 0.05)

            -iwsp
               �i�}���`�p�X���[�h���̂ݗL���j�P��ԂɃV���[�g�|�[�Y���f���� ��
               �ݍ��񂾔F���������s���D���̃I�v�V�������w�肷��ƁC������� �S
               �P��̖����ɁC�X�L�b�v�\�ȃV���[�g�|�[�Y���f�����t�������D
               ���̃V���[�g�|�[�Y���f���̓R���e�L�X�g���l�������C�܂��O��� ��
               �f�̃R���e�L�X�g�ɂ��\��Ȃ��D�t������V���[�g�|�[�Y���f����
               -spmodel �Ŏw��ł���D

            -transp  float
               ���ߒP��ɑ΂���ǉ��̑}���y�i���e�B���w�肷��D(default: 0.0)

            -demo

               -progout -quiet�Ɠ����D

            -mbr

            -nombr

            -mbr_wwer

            -mbr_weight

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
       �g�p��ɂ��Ă͕t���̃`���[�g���A���������������D

SEE ALSO
       julian(1), jcontrol(1), adinrec(1), adintool(1), mkbingram(1),
       mkbinhmm(1), mkgsmm(1), wav2mfcc(1), mkss(1)

       http://julius.sourceforge.jp/[1]

DIAGNOSTICS
       ����I�������ꍇ�CJulius �� exit status �Ƃ��� 0 ��Ԃ��܂��D�G���[����
       �t�������ꍇ�ُ͈�I�����C exist status �Ƃ��� 1 ��Ԃ��܂��D ���̓t�@
       �C����������Ȃ��ꍇ�₤�܂��ǂݍ��߂Ȃ������ꍇ�́C���̃t�@ �C���ɑ�
       ���鏈�����X�L�b�v���܂��D

BUGS
       �g�p�ł��郂�f���ɂ̓T�C�Y��^�C�v�Ɏ኱�̐���������܂��D�ڂ��� �̓p�b
       �P�[�W�ɕt���̃h�L�������g���Q�Ƃ��Ă��������D �o�O�񍐁E�₢���킹�E�R
       �����g�Ȃǂ� julius-info at lists.sourceforge.jp �܂ł��肢���܂��D

COPYRIGHT
       Copyright (c) 1991-2013 ���s��w �͌�������

       Copyright (c) 1997-2000 ��񏈗��U�����Ƌ���(IPA)

       Copyright (c) 2000-2005 �ޗǐ�[�Ȋw�Z�p��w�@��w ���쌤����

       Copyright (c) 2005-2013 ���É��H�Ƒ�w Julius�J���`�[��

AUTHORS
       Rev.1.0 (1998/02/20)
           �݌v�F�͌��B��Ɨ� �W�L (���s��w)

           �����F�� �W�L (���s��w)

       Rev.1.1 (1998/04/14), Rev.1.2 (1998/10/31), Rev.2.0 (1999/02/20),
       Rev.2.1 (1999/04/20), Rev.2.2 (1999/10/04), Rev.3.0 (2000/02/14),
       Rev.3.1 (2000/05/11)
           �����F�� �W�L (���s��w)

       Rev.3.2 (2001/08/15), Rev.3.3 (2002/09/11), Rev.3.4 (2003/10/01),
       Rev.3.4.1 (2004/02/25), Rev.3.4.2 (2004/04/30)
           �����F�� �W�L (�ޗǐ�[�Ȋw�Z�p��w�@��w)

       Rev.3.5 (2005/11/11), Rev.3.5.1 (2006/03/31), Rev.3.5.2 (2006/07/31),
       Rev.3.5.3 (2006/12/29), Rev.4.0 (2007/12/19), Rev.4.1 (2008/10/03),
       Rev.4.1.5 (2010/06/04), Rev.4.2 (2011/05/01), Rev.4.2.1 (2011/12/25),
       Rev.4.2.2 (2012/08/01), Rev.4.2.3 (2013/06/30), Rev.4.3 (2013/12/25)
           �����F�� �W�L (���É��H�Ƒ�w)

THANKS TO
       ���̃v���O������ Rev.3.1 �܂ŁC��񏈗��U�����Ƌ���(IPA)�Ƒn�I���Z�p
       �� �����Ɓu���{��f�B�N�e�[�V�����̊�{�\�t�g�E�F�A�̊J���v(��\�ҁF��
       �� ���G �ޗǐ�[�Ȋw�Z�p��w�@��w����)�̉������󂯂čs���܂����D
       Rev.3.4.2�܂ł́u��񏈗��w�� �A�������F���R���\�[�V�A���v�ɂ����Č��J
       �� ��܂����D

       3.x ����̃}���`�v���b�g�t�H�[�� DLL�� �́C��G����(�������w)�̎�
       �ɂ���č쐬�E���J����܂����D�܂��CWindows Microsoft Speech API�Ή���
       �� �Z�g�M�u��(���s��w�E����)�̎�ɂ����̂ł��D

       ���̂ق��C��L�̋��́E�v�����Ă������������X�C����т��܂��܂ȏ����E�R
       �����g�����������֌W�Ҋe�ʂɐ[�����ӂ������܂��D

���L
        1. http://julius.sourceforge.jp/
           http://julius.sourceforge.jp/en/



                                  19/12/2013                         JULIUS(1)

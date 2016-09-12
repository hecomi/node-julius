#! /usr/bin/perl
use strict;
use IO::Socket;
use IO::Select;

my $host = "localhost";
my $port = 10500;

print STDERR "$host($port) ����³���ޤ�\n";

# Socket������������³
my $socket;
while(!$socket){
    $socket = IO::Socket::INET->new(PeerAddr => $host,
                                    PeerPort => $port,
                                    Proto    => 'tcp',
                                    );
    if (!$socket){
        printf STDERR "$host($port) ����³�˼��Ԥ��ޤ���\n";
        printf STDERR "����³���ߤޤ�\n";
        sleep 10;
    }
}

print STDERR "$host($port) ����³���ޤ���\n";

# �Хåե���󥰤򤷤ʤ�
$| = 1;
my($old) = select($socket); $| = 1; select($old);

# Selecter������
my $selecter = IO::Select->new;
$selecter->add($socket);
$selecter->add(\*STDIN);

# �����Ԥ�
while(1){
    my ($active_socks) = IO::Select->select($selecter, undef, undef, undef);

    foreach my $sock (@{$active_socks}){
        # Julius����ν��Ϥ�ɽ��
        if ($sock == $socket){
            while(<$socket>){
                print;
                last if(/^\./);
            }
	    # ɸ�����Ϥ�Julius������
        }else{
            my $input = <STDIN>;
            # ��ʸ������ʸ�����Ѵ�
            $input =~ tr/a-z/A-Z/d;

            print $socket $input;
        }
    }
}       


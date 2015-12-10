#!/usr/bin/env perl -w

use strict;
use FindBin;
use lib "$FindBin::Bin/lib";
use IO::Socket::INET;
use POSIX qw(strftime);
use DateTime;
use Cwd qw(abs_path realpath);
use File::Basename qw( dirname );
use JSON::Parse qw(parse_json valid_json);
use Memcached::Client;
use Data::Dumper;
use Log::Log4perl;
use Log::Dispatch::FileRotate;
use Pod::Usage;
use ConfigLoader;

use constant DATETIME => strftime("%Y-%m-%d %H:%M:%S", gmtime);

Log::Log4perl::init(abs_path(dirname(realpath($0)) . '/conf/log4perl.conf'));
my $logger = Log::Log4perl::->get_logger(q(UDPClient));

my $config = eval {
    new ConfigLoader(
        file => '/../conf/servicioConteoPersonas.conf'
    );
} or die($@);

$logger->debug('start') if $logger->is_debug();

# flush after every write
$| = 1;

my ($socket, $client_socket);

$socket = new IO::Socket::INET (
    PeerHost => $config->getSetting('client.server'),
    PeerPort => $config->getSetting('client.port'),
    Proto => $config->getSetting('client.proto')
) or die "ERROR in Socket Creation : $!\n";

my $memd = Memcached::Client->new ({servers => ['127.0.0.1:11211']});
for my $device (keys($config->getSetting('devices'))) {
    my $json =  $memd->get($device);
    if (valid_json($json)) {
        my $perl = parse_json($json);
        my $imei = $config->getSetting('devices')->{$device};    
        my $linesQty = keys %{$perl->{$imei}->{count}};
        my $sensorsMaxLength = 10;
        my $sensors = [];
        my $counter = 0;

        #########################

        my $maxAdditions = 10;
        my $maxSubtractions = 10;

        my $tmpSubtotalSubstractions = 0;
        my $tmpSubtotalAdditions = 0;

        my $tmpPrefix;

        #########################

        if (defined($perl->{$imei}->{count})) {
            # {$a <=> $b}
            foreach my $line (sort {lc $a cmp lc $b} keys %{ $perl->{$imei}->{count} } )  {
                if ($counter == 10) {
                    last;
                }
                
                ########################################################################

                my $dashPosition = index($line, '-total');
                
                if ($dashPosition != -1) {
                    $tmpPrefix = substr($line, 0, $dashPosition);

                    for (my $i = $maxAdditions; $i--;) {
                        if (exists $perl->{$imei}->{count}->{$tmpPrefix . '-A' . $i}) {
                            $tmpSubtotalAdditions += $perl->{$imei}->{count}->{$tmpPrefix . '-A' . $i};
                        }
                    }
                    for (my $i = $maxSubtractions; $i--;) {
                        if (exists $perl->{$imei}->{count}->{$tmpPrefix . '-S' . $i}) {
                            $tmpSubtotalSubstractions += $perl->{$imei}->{count}->{$tmpPrefix . '-S' . $i};
                        }
                    } 
                    $perl->{$imei}->{count}->{$line} = abs($tmpSubtotalAdditions - $tmpSubtotalSubstractions);

                    $tmpSubtotalSubstractions = 0;
                    $tmpSubtotalAdditions = 0;
                }

             

                ########################################################################
                push(@{$sensors}, $perl->{$imei}->{count}->{$line});

                $counter++;
            }
        }
        if ($linesQty < 10) {
            push(@{$sensors}, (0) x ($sensorsMaxLength - @{$sensors}));
        }

        # write on the socket to server.
        my $packet =  int(rand(10));
        # 
        my $message = '$B,' . $perl->{$imei}->{deviceID}  . ',' . $packet . ',0,31,' . join(',', @{$sensors}) . ',0,0,Iriys,' . DATETIME . ',$E';
        print Dumper($message);
        $socket->send($message);

        # read the socket data sent by server.
        $socket->recv(my $response,1024);
        $logger->debug('Received from Server :' . $response) if $logger->is_debug();
    }

    sleep(5);
    $socket->close();

}
 $memd->disconnect();

$logger->debug('end') if $logger->is_debug(); 


__END__

=pod

=head1 NAME

=head1 SYNOPSIS

=head1 OPTIONS

N/A

=head1 DESCRIPTION


=head1 TODO

N/A

=head1 SEE ALSO

=head1 AUTHOR

drivarola@movilgate.com

=cut
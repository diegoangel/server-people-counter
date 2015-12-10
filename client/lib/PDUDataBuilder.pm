# class PDUDataBuilder
package PDUDataBuilder;

use strict;
use base ('BaseClass');
use POSIX qw(strftime);

sub new {
	my $class = shift;
	my $self = $class->SUPER::new();
    $self->{_startDelimiter} = '$B';
    $self->{_sensors} = [];
    $self->{_datetime} = strftime("%Y-%m-%d %H:%M:%S", gmtime);
    $self->{_package} = '';
    $self->{_model} = '';
    $self->{_imei} = 0;
    $self->{_endDelimiter} = '$E';
	bless $self, $class;
    return $self;
}

sub setStartDelimiter {
    my ($self, $startDelimiter) = @_;
    if (defined $startDelimiter) {
        $self->{_startDelimiter} = $startDelimiter;
    }
}

sub setSensor {
    my ($self, $sensor) = @_;
    if (defined $sensor) {
        if (scalar(@{$self->{_sensors}}) < 12) {
            push(@{$self->{_sensors}}, $sensor);
        }        
    }
}

sub setDatetime {
    my ($self, $datetime) = @_;
    if (defined $datetime) {
        $self->{_datetime} = $datetime;
    }      
}

sub setPackage {
    my ($self, $package) = @_;
    if (defined $package) {
        $self->{_package} = $package;
    }    
}

sub setModel {
    my ($self, $model) = @_;
    if (defined $model) {
        $self->{_model} = $model;
    }     
}

sub setImei {
    my ($self, $imei) = @_;
    if (defined $imei && (length $imei == 15 || length $imei == 14)) {
        $self->{_imei} = $imei;
    } 
}

sub setEndDelimiter {
    my ($self, $endDelimiter) = @_;
    if (defined $endDelimiter) {
        $self->{_startDelimiter} = $endDelimiter;
    }    
}

sub getData {
    my ($self) = @_;
}

sub toString {
    my ($self) = @_;
    return $self->SUPER::dumper($self);
}

sub DESTROY {
    my ($self) = @_;
    undef $self->{_startDelimiter};
    undef $self->{_sensors};
    undef $self->{_datetime};
    undef $self->{_package};
    undef $self->{_model};
    undef $self->{_imei};
    undef $self->{_endDelimiter};
}

1;

__END__

=pod

=head NAME

PDUDataBuilder

=head1 SYNOPSYS

C<< use PDUDataBuilder; >>

C<< 
my $config = eval {  
   new PDUDataBuilder();   
}  or die ($@); 
>>

=head1 DESCRIPTION

=head2 METHODS

=over

=item C<new()>

Constructor de la clase

=item C<DESTROY()>

Destructor de la clase

=back

=head1 LICENSE AND COPYRIGHT

Copyright E<0169> 2015 Movilgate

Cannot modify source-code for any purpose (cannot create derivative works)

=head1 AUTHOR

Diego - L<drivarola@movilgate.com>

=head1 SEE ALSO

L<perlpod>, L<perlpodspec>

=cut
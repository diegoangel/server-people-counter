# class MemcachedDriver
package MemcachedDriver;

use strict;
use base ('BaseClass');
use Cache::Memcached::Fast;

sub new {
    my $class = shift;
    my %args = @_;
    my $self = $class->SUPER::new();
    $self->{_host} = $args{host} || '127.0.0.1';
    $self->{_port} = $args{port} || 11211;
    $self->{_memd} = undef;
    bless $self, $class;
    $self->_connect();
    return $self;
}

sub _connect {
    my ($self) = @_;
    if (defined $self->{_host} && defined $self->{_port}) {
        $self->{_memd} = eval {
            new Cache::Memcached::Fast({
                servers => [
                    {address => $self->{_host} . ':' . $self->{_port}}
                ]
            });
        } or die($self->SUPER::getLogger()->error($@));
    }
}

sub get {
    my ($self, $key) = @_;
    my $value = '';
    if (defined $key) {
        $value = $self->{_memd}->get($key);
        if ($value eq '') {
            $self->SUPER::getLogger()->error('Key doesn\'t exists');
        }
    }
    return $value;
}

sub set {
    my ($self, $key, $value, $expirationTime) = @_;
    $expirationTime //= 0;
    if (!defined $key || !defined $value) {
        $self->SUPER::getLogger()->error('Key and value arguments are required');
        return 0;
    }
    if (!$self->{_memd}->set($key, $value, $expirationTime)) {
        $self->SUPER::getLogger()->error('An error ocurred while storing the key');
        return 0;
    }
    return 1;
}

sub delete {
    my ($self, $key) = @_;
    if (!defined $key) {
        $self->SUPER::getLogger()->error('Key argument is required');
        return 0;
    }
    if (!$self->{_memd}->delete($key)) {
        $self->SUPER::getLogger()->error('An error ocurred while deleting the key');
        return 0;        
    }
    return 1;
}

sub disconnect {
    my ($self) = @_;
    $self->{_memd}->disconnect_all();
}

sub flushAll {
    my ($self) = @_;
    if (!$self->{_memd}->flush_all()) {
        $self->SUPER::getLogger()->error('An error ocurred while flushing the cache');
        return 0;
    }
    return 1;
}

sub toString {
    my ($self) = @_;  
    $self->SUPER::dumper($self);
}

sub DESTROY {
    my ($self) = @_;
}

1;

__END__

=pod

=head1 NAME

MemcachedDriver

=head1 SYNOPSYS

C<< use MemcachedDriver; >>

C<< 
my $memd = eval {  
   new MemcachedDriver(  
        host => '127.0.0.1',
        port => 11211   
   );   
}  or die ($@); 
>>

=head1 DESCRIPTION

Hereda de Base.

=head2 METHODS 

=over 12

=item C<new()>

Constructor de la clase.

Retorna una referencia a la clase.


=item C<_connect()>

Crea una instancia de Cache::Memcached::Fast

En caso de error lanza una excepcion.

Retorna void.

=item C<set()>

Recibe 3 argumentos.

Setea una key en memcache con un valor y tiempo de expiracion determinado.

Retorna true o false.

=item C<get()>

Retorna un string, el valor de la key que deseamos o vacio si no existe.

=item C<delete()>

Elimina una key.

Retorna true o false;

=item C<disconnect()>

Desconecta del servidor.

Retorna void.

=item C<flushAll()>

Elimina el cache del servidor.

Retorna true o false;

=item C<toString()>

=item C<DESTROY()>

Destructor de la clase.

Declara las variables de la clase como C<undef>

Retorna void, o en otras palabras, no retorna nada.


=back

=head1 LICENSE AND COPYRIGHT

Copyright E<0169> 2015 Movilgate

Cannot modify source-code for any purpose (cannot create derivative works)

=head1 AUTHOR

Diego - L<drivarola@movilgate.com>

=head1 SEE ALSO

L<perlpod>, L<perlpodspec>

=cut

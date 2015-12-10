# class BaseClass
package BaseClass;

use strict;
use warnings;
use Data::Dumper;
use Cwd qw(abs_path realpath);
use File::Basename qw( dirname );
use Log::Log4perl;
use Log::Dispatch::FileRotate;

sub new {
    my $class = shift;
    my $self = {
        _logger => undef,
    };
    bless $self, $class;
    $self->_initLogger();
    return $self;
}

sub _initLogger {
    my ($self) = @_;
    Log::Log4perl::init(abs_path(dirname(realpath($0)) . '/conf/log4perl.conf'));
    $self->{_logger} = Log::Log4perl::->get_logger(q(UDPClient));
}

sub getLogger {
    my ($self) = @_;
    return $self->{_logger};
}

sub dumper {
    my ($self, $var) = @_;
    Dumper($var);
}

sub DESTROY {

}

1;

__END__

=pod

=head1 NAME

BaseClass

=head1 SYNOPSYS

Es una clase base para unificar implementaciones de diferentes modulos que puedan ser reutilizados en las 
clases que hereden de esta.

C<< use base (BaseClass); >>

En la clase derivada instanciar la clase padre de esta forma.

C<< 
sub new {
    ...;
    my $self = $class->SUPER::new();
    ...;
}
>>

Acceder a los metodos y/o atributos de esta forma.

C<< $self->SUPER::someMethod(); >>

o

C<< $self->BaseClass::someMethod(); >>

en el caso de herencia multiple. Valido tambien al momento de crear la instancia.

=head1 DESCRIPTION

Es una clase base para unificar implementaciones de diferentes modulos que puedan ser reutilizados en las 
clases que hereden de esta.

=head2 METHODS 

=over 12

=item C<new()>

Constructor de la clase.

Define las siguientes variables.

C<logger>: almacena una  instancia de la clase Log4perl

Retorna una referencia a la clase.

=item C<_initLogger()>

Inicializa una instancia de la clase Log4perl, la almacena el atributo C<_logger>.

Retorna void.


=item C<getLogger()>

Retorna la instancia de Log4perl para ser utilizada.


=item C<dumper()>

Helper para evitar tener que cargar el modulo Data::Dumper en cada clase.

Acepta un argumento de cualquier tipo.

Retorna el string resultado de aplicar a la funcion Data::Dumper() al argumento enviado al metodo.


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
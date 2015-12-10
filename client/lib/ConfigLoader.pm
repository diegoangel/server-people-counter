# class ConfigLoader
package ConfigLoader;

use strict;
use base ('BaseClass');
use JSON::Parse qw(valid_json json_file_to_perl);
use Cwd qw(abs_path realpath);
use File::Basename qw(dirname);

sub new {
    my $class = shift;
    my %args = @_;
    my $self = $class->SUPER::new();
    $self->{_file} = $args{file} || '';
    $self->{_settings} = undef;
    bless $self, $class;
    $self->_readFile();
    return $self;
}

sub _readFile {
    my ($self) = @_;
    if (!json_file_to_perl($self->_getFile())) {
        $self->SUPER::getLogger()->error('An error ocurred while converting JSON string into a perl hash');
        return 0;	
    }
    $self->{_settings} = json_file_to_perl($self->_getFile());
    return 1;
}

sub getSetting {
    my ($self, $option) = @_;
    if (index($option, '.') == -1) {
        return $self->{_settings}->{$option};
    }    
    my ($group, $item) = split(/\./, $option);
    if (!defined $self->{_settings}->{$group}->{$item}) {
        $self->SUPER::getLogger()->debug('Config option doesn\'t exists');  
        return ''; 
    }
    return $self->{_settings}->{$group}->{$item};
}

sub _getFile {
    my ($self) = @_;
    my $file = abs_path(dirname(realpath($0)) . $self->{_file});    
    if (! -e $file || ! -f $file) {
        $self->SUPER::getLogger()->error('Config file doesn\'t exists'); 
        return '';
    }
    return $file;
}

sub toString {
    my ($self) = @_;  
    $self->SUPER::dumper($self);
}

sub DESTROY {
    my ($self) = @_;
    undef $self->{_file};
    undef $self->{_settings};
}

1;

__END__

=pod

=head1 NAME

ConfigLoader

=head1 SYNOPSYS

C<< use ConfigLoader; >>

C<< 
my $config = eval {  
   new ConfigLoader(  
        file => 'somefile.conf'   
   );   
}  or die ($@); 
>>

C<< my $serverIp = $config->get('server.ip'); >>


=head1 DESCRIPTION

Hereda de Base.

Clase para manipular archivos de configuracion e formato JSON.
El archivo no deberia contener un string JSON de mas de 2 dimensiones.

Ej.:

C<
{
    "server": {
        "ip":   192.168.0.1,
        "port": 20001 
    },
    "memcached": {
        "host": 127.0.0.1,
        "port": 11211
    },
    ...
}
>

=head2 METHODS 

=over 12

=item C<new()>

Constructor de la clase.

Recibe un argumento tipo hash cuya clave debe llamarse: C<file> y luego define las variables de la clase:

C<_file>: almacena la ruta al archivo que es recibida a traves del argumento C<file>, puede ser relativa o absoluta.

C<_settings>: almacenar un hash, con las variables de configuracion obtenidas del archivo almacenado en C<_file>.

Retorna una referencia a la clase.


=item C<_readFile()>

Comprueba que path al archivo recibido como argumento en el constructor sea un path valido.
Si es un path valido declara la variable de la clase C<_file> con el valor del el path absoluto a ese archivo.

Retorna true o false.

=item C<getFile()>

Retorna un string, el path absoluto del archivo de configuracion o vacio.

=item C<getSetting()>

Acepta como parametro un string que contiene a modo de path separado por puntos, cada uno de las claves del JSON 
a las que necesitamos acceder para obtener el valor que deseamos.

Ej.: server.ip o memcached.port

C<< my $serverIp = $config->get('server.ip'); >>

Retorna un string, el valor del JSON que deseamos o vacio si no existe.


=item C<toString()>

Retorna un string, la referencia a la clase representada en una cadena de texto.


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

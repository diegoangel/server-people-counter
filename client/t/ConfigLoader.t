#!/usr/bin/env perl -w

use strict;
use FindBin;
use lib "$FindBin::Bin/../lib";
use Test::More;
use feature 'say';

require_ok 'ConfigLoader';

my $config = eval {
	new ConfigLoader(
		file => '../conf/ServicioConteoPersonas.conf'
	);
}  or die ($@);

isa_ok( $config, 'ConfigLoader');

can_ok('ConfigLoader', ('getSetting', '_readFile', '_getFile'));

ok(($config->_getFile() ne ''), 'Checking if config file exists: ' . $config->_getFile());

SKIP: {

	skip "Config file is not a valid file", 1 if ($config->_getFile() eq '');
	# 5
	ok(($config->getSetting('server.ip') ne ''), 'Checking server IP defined: ' . $config->getSetting('server.ip'));
}

#say $config->toString();

done_testing();
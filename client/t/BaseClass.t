#!/usr/bin/env perl -w

use strict;
use FindBin;
use lib "$FindBin::Bin/../lib";
use Test::More;
use feature 'say';

require_ok 'BaseClass';

my $base = eval {new BaseClass();}  or die ($@);

isa_ok($base, 'BaseClass');

can_ok('BaseClass', ('_initLogger', 'getLogger', 'dumper'));

done_testing();
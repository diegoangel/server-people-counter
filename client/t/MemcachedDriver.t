#!/usr/bin/env perl -w

use strict;
use FindBin;
use lib "$FindBin::Bin/../lib";
use Test::More;
use feature 'say';

require_ok 'MemcachedDriver';

my $memd = eval {
    new MemcachedDriver(
        host => '127.0.0.1',
        port => 11211
    );
}  or die ($@);

isa_ok( $memd, 'MemcachedDriver');

can_ok('MemcachedDriver', ('_connect', 'set', 'get'));

my $key = 'a';
my $value = 'b';

ok($memd->set($key, $value, 900), 'Testing setting a key: ' . $key . ' with value: ' . $value . ' and an expiration time fo 900 s');

ok(($memd->get($key) ne ''), 'Checking the value of the recently stored key ' . $key . ': ' . $memd->get($key));

ok($memd->delete($key), 'Testing deleting the key: a');
#say $memd->toString();

done_testing();
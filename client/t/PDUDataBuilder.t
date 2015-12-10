#!/usr/bin/env perl -w

use strict;
use FindBin;
use lib "$FindBin::Bin/../lib";
use Data::Dumper;
use Test::More;
use feature 'say';

require_ok 'PDUDataBuilder';

my $dataBuilder = eval {new PDUDataBuilder();}  or die ($@);

isa_ok($dataBuilder, 'PDUDataBuilder');

can_ok('PDUDataBuilder', 
    (
        'setStartDelimiter', 
        'setSensor',
        'setImei',
        'setPackage',
        'setDatetime',
        'setModel',
        'setEndDelimiter',
        'getData'
    )
);

for my $i (1 .. 12) {
    my $value = int(rand(100));
    ok($dataBuilder->setSensor($value), 'Testing setting sensor number ' . $i . ' with value: ' . $value);
}

ok(!$dataBuilder->setSensor(99), 'Setting the value of the 13th sensor must fail because all the 12 available positions had been filled');

my $settingsSize = scalar @{$dataBuilder->{_sensors}};
ok($settingsSize == 12, 'Hash of sensors stored in class attribute: _sensors, has ' . $settingsSize . ' elements: ' . Dumper($dataBuilder->{_sensors}));

done_testing();

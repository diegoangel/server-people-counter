#!/usr/bin/env perl -w

use strict;
use Test::Harness;

if ($ARGV[0] && $ARGV[0] eq 'v') {
    $Test::Harness::verbose = 'true';    
}

my @test_files = (
    't/BaseClass.t',
    't/ConfigLoader.t',
    't/MemcachedDriver.t',
    't/PDUDataBuilder.t'
);

runtests(@test_files);

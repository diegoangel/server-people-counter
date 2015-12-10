#!/usr/bin/env perl -w

use strict;
use Data::Dumper;
use Cache::Memcached::Fast;

my $memd = new Cache::Memcached::Fast({
        servers => [
            {address => '127.0.0.1:11211'},
        ],
    });

$memd->set("ResetCurrentCount", "1");

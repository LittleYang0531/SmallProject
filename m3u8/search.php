<?php
$source = 2;

if ($argc < 2) {
    echo "Usage: php " . $argv[0] . " [keywords] [Search ID] [vodL] [vodR] [Source ID]\n";
    echo "Source Website: \n";
    echo "1. omofun.tv(died)" . ($source == 1 ? " <-now" : "") . "\n";
    echo "2. malimali6.com" . ($source == 2 ? " <-now" : "") . "\n";
    exit;
}

if ($source == 1) require_once "omofun.php";
else if ($source == 2) require_once "malimali.php";
else echo "Invalid Source Website!\n";
?>
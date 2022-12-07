<?php
declare(strict_types=1);

$hexMrkev = "\x4D\x52\x4b\x45\x56"; // hex string for MRKEV

$octaJabko = "\112\101\102\113\117"; // octal string for JABKO

$justSpace = " ";

$delka = strlen($octaJabko);
write("Delka je: ", $delka, "\n");

$finalString = $hexMrkev . $justSpace . $octaJabko;

write("Final string is: ", $finalString, "\n");
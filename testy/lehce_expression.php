<?php
declare(strict_types=1);

write("ahoj\n");
$a = 3;
write("\$a = ", $a, "\n");
$a = $a+2-1;
write("\$a+2-1 = ", $a, "\n");
$a = 3;
write("\$a = ", $a, "\n");
$r = 1+2*3;
write("1+2*3 = ", $r, "\n");
$r = 1+2*$a;
write("1+2*\$a = ", $r, "\n");
write("\n");

$a = "mrkev";
$b = $a . $a;
write("concatenate mrkev mrkev = ", $b, "\n");
write("\n");

$a = 1.0+2.0*3.0-4.0/5.0;
write("float expression 1.0+2.0*3.0-4.0/5.0 = 1.0 = ", $a, "\n");
write("\n");

?>
<?php
declare(strict_types=1);

write("Test funkci xxxval(term), testuje se vzdy konstanta a promenna s odpovidajicim typem:\n\n");

$in = null;
$f1 = floatval(null);
$f2 = floatval($in);
write("floatval(null) = ", $f1, " a ", $f2, "\n");
$in = 1;
$f1 = floatval(1);
$f2 = floatval($in);
write("floatval(1) = ", $f1, " a ", $f2, "\n");
$in = 0.25;
$f1 = floatval(0.25);
$f2 = floatval($in);
write("floatval(0.25) = ", $f1, " a ", $f2, "\n");
$in = "0.5";
$f1 = floatval("0.5");
$f2 = "no STRNUM"; // floatval($in); goes to semantic error without STRNUM
write("floatval(\"0.5\") = ", $f1, " a ", $f2, "\n");

write("\n");

$in = null;
$f1 = intval(null);
$f2 = intval($in);
write("intval(null) = ", $f1, " a ", $f2, "\n");
$in = 1;
$f1 = intval(1);
$f2 = intval($in);
write("intval(1) = ", $f1, " a ", $f2, "\n");
$in = 2.55;
$f1 = intval(2.55);
$f2 = intval($in);
write("intval(2.55) = ", $f1, " a ", $f2, "\n");
$in = "45.67";
$f1 = intval("45.67");
$f2 = "no STRNUM"; // floatval($in); goes to semantic error without STRNUM
write("intval(\"45.67\") = ", $f1, " a ", $f2, "\n");

write("\n");

$in = null;
$f1 = strval(null);
$f2 = strval($in);
write("strval(null) = \"", $f1, "\" a \"", $f2, "\"\n");
$in = 1;
$f1 = strval(1);
$f2 = "no STRNUM"; // strval($in); goes to semantic error without STRNUM
write("strval(1) = \"", $f1, "\" a \"", $f2, "\"\n");
$in = 2.55;
$f1 = strval(2.55);
$f2 = "no STRNUM"; // strval($in); goes to semantic error without STRNUM
write("strval(2.55) = \"", $f1, "\" a \"", $f2, "\"\n");
$in = "45.67";
$f1 = strval("45.67");
$f2 = strval($in);
write("strval(\"45.67\") = \"", $f1, "\" a \"", $f2, "\"\n");

?>
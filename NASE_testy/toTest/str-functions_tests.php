<?php
declare(strict_types=1);

// test function strlen(string $s) : int
$a = strlen("mrkev");
write("strlen(\"mrkev\") = ", $a, "\n");
$a = strlen("x\nz");
write("strlen(\"x\\nz\") = ", $a, "\n");
$a2 = "2";
$a = strlen($a2);
write("strlen(\"2\") = ", $a, "\n");
$a = strlen("");
write("strlen(\"\") = ", $a, "\n");

write("\n");

// test function ord(string $s) : int
$str = "";
$res = ord($str);
write("ord(\"\") = ", $res, "\n");
$str = "a";
$res = ord($str);
write("ord(\"", $str,"\") = ", $res, "\n");
$res = ord("AHOJ");
write("ord(\"AHOJ\") = ", $res, "\n");

write("\n");

// test function chr(int $i) : string
$str = chr(65);
write("chr(65) = \"", $str, "\"\n");
$a = 49;
$str = chr($a);
write("chr(", $a,") = \"", $str, "\"\n");
write("ASCII 32-127: ");
$a = 32;
while($a<128) {
	$str = chr($a);
	write($str);
	$a = $a + 1;
}
write("\n");

write("\n");

// test function sunstring(string $s, int $i, int $j) : string
$str = "";
$sstr = substring($str, 1, 5);
write("substring(\"", $str, "\", 1, 5) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$str = "Gigachad compiler";
$len = strlen($str);
write("Testovaci retezec \"", $str, "\", delka = ", $len, " znaku \n");
$sstr = substring($str, 9, 16);
write("substring(\"", $str, "\", 9, 16) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$sstr = substring($str, -9, 16);
write("substring(\"", $str, "\", -9, 16) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$sstr = substring($str, 9, -16);
write("substring(\"", $str, "\", 9, -16) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$sstr = substring($str, 9, 7);
write("substring(\"", $str, "\", 9, 7) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$sstr = substring($str, 17, 17);
write("substring(\"", $str, "\", 17, 17) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$sstr = substring($str, 18, 18);
write("substring(\"", $str, "\", 18, 18) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$sstr = substring($str, 9, 17);
write("substring(\"", $str, "\", 9, 17) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$sstr = substring($str, 9, 18);
write("substring(\"", $str, "\", 9, 18) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$sstr = substring($str, 0, 0);
write("substring(\"", $str, "\", 0, 0) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$sstr = substring($str, 16, 17);
write("substring(\"", $str, "\", 16, 17) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$sstr = substring($str, 15, 16);
write("substring(\"", $str, "\", 15, 16) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");
$sstr = substring($str, 0, 17);
write("substring(\"", $str, "\", 0, 17) = ");
if($sstr === null) {write("null");} else {write("\"", $sstr, "\"");}
write("\n");

?>
<?php
declare(strict_types=1);

function nullFunction(?int $i) : ?string {
	if($i === null) {
		return null;
	} else {
		if($i === 0) {
			return "0";
		} else {
			return "1";
		}
	}
}

$f = nullFunction(null);
write("nullFuction(null) = ");
if($f === null) {write("null");} else {write("\"", $f, "\"");}
write("\n");
$f = nullFunction(0);
write("nullFuction(0) = ");
if($f === null) {write("null");} else {write("\"", $f, "\"");}
write("\n");
$f = nullFunction(1);
write("nullFuncion(1) = ");
if($f === null) {write("null");} else {write("\"", $f, "\"");}
write("\n");

?>
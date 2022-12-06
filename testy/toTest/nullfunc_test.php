<?php
declare(strict_types=1);

function nullFunction(int $i) : ?string {
	if($i === 0) {
		return null;
	} else {
		return "mrkev";
	}
}

$f = nullFunction(0);
write("nullFuction(0) = ");
if($f === null) {write("null");} else {write("\"", $f, "\"");}
write("\n");
$f = nullFunction(1);
write("nullFuncion(1) = ");
if($f === null) {write("null");} else {write("\"", $f, "\"");}
write("\n");

?>
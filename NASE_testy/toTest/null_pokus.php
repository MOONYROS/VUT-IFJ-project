<?php declare(strict_types=1);

$a = 123;
$a = null;
write("\"", $a, "\"\n");

if($a === null){
    write("\$a === null\n");
}
else{
    write("\$a !== null\n");
}

?>
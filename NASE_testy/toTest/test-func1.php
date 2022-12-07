<?php
declare(strict_types=1);

function addOne(int $a) : int {
    return $a + 1;
}

function addTwo(int $a) : int {
    return addOne(addOne($a));
}

$a = 0;
$a = addTwo(addTwo(addOne($a)));
write("\$a = ", $a , "\n");
?>
<?php
declare(strict_types=1);
// FUNGUJE
$a = 5;
$b = 3;
$c;
function overwritePlus5(int $a, int $b) : int{
    $a = $b;
    $a = plus5($a);
    return $a;
}
function plus5(int $a) : int{
    $a = $a + 5;
    return $a;
}
$a = overwritePlus5($a, $b);    // Returning second argument
?>

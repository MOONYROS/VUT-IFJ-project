<?php
declare(strict_types=1);
// Po definovani funcke WRITE by mělo fungovat
function firstPart(int $a,int $b,int  $c,int  $x) : int {
  $result = $a*$x*$x+$b*$x+$c;
  return $result;
}
$y = firstPart(1, 2, 3, 4);
write($y);
?>
<?php
declare(strict_types=1);

function f (int $x) : int{
   if($x < 10){
      return $x-1;
   } else{
      $x=$x-1;
      write("Calling g with ", $x, "\n");
      $res=g($x);
      return $res;
   }
}
function g(int $x) : int{
   if($x > 0){
      write("Calling f with ", $x, "\n");
      $x = f($x);
      return $x;
   } else{
      return 200;
   }
}
$res = g(10);
write("res: ", $res, "\n");
?>
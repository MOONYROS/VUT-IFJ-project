<?php
declare(strict_types=1);
// FUNGUJE
$num = 0;
$num4 = 0;
while($num < 5){
   $num2 = $num + 5;
   if ($num2 === 5){
      $num3 = 15;
      while($num4 < 3){
         $num5 = $num4;
         echo $num5;
         $num4 = $num4 + 1;
      }
   }else{
      $num = $num + 1;   
   }
   $num = $num + 1;
}
?>
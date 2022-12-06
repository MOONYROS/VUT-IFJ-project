<?php
declare(strict_types=1);
// FUNGUJE
$num = 0;
$num4 = 0;
while($num < 5){
   $num2 = $num + 5;
   write("\$num2 = ", $num2, "\n");
   if ($num2 === 5){
      $num3 = 15;
      write("\$num3 = ", $num3, "\n");
      while($num4 < 3){
         $num5 = $num4;
         write("\$num4 = ", $num4, "\n");
         write("\$num5 = ", $num5, "\n");
         $num4 = $num4 + 1;
         write("\$num4 = ", $num4, "\n");
      }
   }else{
      $num = $num + 1;   
   }
   $num = $num + 1;
   write("\$num = ", $num, "\n");
}
?>
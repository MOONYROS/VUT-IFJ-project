<?php
declare(strict_types=1);
// FUNGUJE
// Funkce pridana by PHP nervalo pro testovani lexikalniho analyzatoru
function write ($input) : void {
}
$num = 47.8;
$num2 = 20;
$num3 = 5;
if (50 > 18){
   while ($num > 30){
      $num2 = 20;
      $num3 = 5;
      while ($num3 < 8){
         while ($num2 > -10){
            write("Ok\n");
            if ($num2 > 10){
               write($num2, "\n");
            }else {
               write($num2, "\n");
            }
            $num2 = $num2 - 2;
            if($num2 === -10){
               $num3 = 0;
            }
         }
      }
      $num = $num - 1.5;
   }
} else{
   write("Hello \n");
}
?>
<?php
// FUNGUJE
declare(strict_types=1);
$num = 4;
$num2 = 2;
function number1(){
   $num = 5;
   if($num === 5){
      $num2 = -1;
   } else{
      $num = -5;
   }
}
if($num === 5){
   $num2 = 3;
}
else{
   number1();
}
?>
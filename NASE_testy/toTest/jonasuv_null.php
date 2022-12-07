<?php
declare(strict_types=1);

$tmp0 = 0;
$tmp1 = 1;
$tmpnull = null;

$a = $tmp1>$tmp0;
$b = $tmp1<$tmp0;
$c = $tmp1>$tmp1;
//$d = $tmp1>=$tmp0;
//$e = $tmp1>=$tmp1;
$f = $tmp1<=$tmp1;
$g = $tmp1<=$tmp0;
//$h = $tmp1>=$tmpnull;
//$i = $tmp0<=$tmpnull;
//$j = $tmp0>$tmpnull;
$k = $tmp1!==$tmp1;
$l = $tmp1!==$tmp0;
$m = $tmp1===$tmp1;
$n = $tmp1===$tmp0;
$o = $tmp1!==$tmpnull;
$p = $tmp0!==$tmpnull;
$q = $tmp1===$tmpnull;
$r = $tmp0===$tmpnull;
$s = $tmpnull===$tmp0;
$t = $tmpnull!==$tmp0;
$u = $tmpnull===$tmp1;
$v = $tmpnull!==$tmp1;

write(
$a, " 1>0\n",
$b, " 1<0\n",
$c, " 1>1\n",
//$d, " 1>=0\n",
//$e, " 1>=1\n",
$f, " 1<=1\n",
$g, " 1<=0\n",
//$h, " 1>=null\n",
//$i, " 0<=null\n",
//$j, " 0>null\n",
$k, " 1!==1\n",
$l, " 1!==0\n",
$m, " 1===1\n",
$n, " 1===0\n",
$o, " 1!==null\n",
$p, " 0!==null\n",
$q, " 1===null\n",
$r, " 0===null\n",
$s, " null===0\n",
$t, " null!==0\n",
$u, " null===1\n",
$v, " null!==1\n"
);
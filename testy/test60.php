<?php
declare(strict_types=1);
// FUNGUJE
$xlogin00;
$x = "x";
$login = "mencm";
$num = "00";
function makexlogin00(string $x, string $login, string $num) : string{
    $tmp = $x;
    $tmp = login($tmp, $login, $num);
    return $tmp;
}
function login(string $tmp, string $login, string $num) : string{
    $tmp = $tmp . $login;
    $tmp = xlogin00($tmp, $num);
    return $tmp;
}
function xlogin00(string $tmp, string $num) : string{
    $tmp = $tmp . $num;
    return $tmp;
}
$xlogin00 = makexlogin00($x, $login, $num);    // Creating xlogin00
?>

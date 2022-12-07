Test funkci xxxval(term), testuje se vzdy konstanta a promenna s odpovidajicim typem:

floatval(null) = 0x0p+0 a 0x0p+0
floatval(1) = 0x1p+0 a 0x1p+0
floatval(0.25) = 0x1p-2 a 0x1p-2
floatval("0.5") = 0x1p-1 a no STRNUM

intval(null) = 0 a 0
intval(1) = 1 a 1
intval(2.55) = 2 a 2
intval("45.67") = 45 a no STRNUM

strval(null) = "" a ""
strval(1) = "1" a "no STRNUM"
strval(2.55) = "0x1.4666666666666p+1" a "no STRNUM"
strval("45.67") = "45.67" a "45.67"

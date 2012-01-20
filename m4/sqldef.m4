define(`TRIPLE',`("$1",$2,$3)')dnl
define(`Tmake',`create table  $1 ( key text,link int,  pointer int);' )dnl
define(`Tdel',`drop table if exists $1;')dnl
define(`TABLE', Tdel($1) Tmake($1) )dnl
TABLE(result)

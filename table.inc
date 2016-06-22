<?php
//echo "ENGINE: $ENGINE\n";
if(!isset($table)) $table = "fact";
sql_ok(
"DROP TABLE IF EXISTS $table;",
'drop fact table'
);

$sql = "create table $table (
seq bigint,
i1  tinyint, 
i2  smallint,
-- i3  mediumint,
i4  int,
i8  bigint,
i1u tinyint unsigned,
i2u smallint unsigned,
-- i3u mediumint unsigned,
i4u int unsigned,
i8u bigint unsigned,
-- d1  decimal(65),
-- d2  decimal(65,1),
-- d3  decimal(65,30),
d5  decimal(15,5),
f1  float,
f2  float(2,1),
f23  float(23,4),
f24  float(24,10),
f53  float(53,20),
p1 double,
p2 double(2,1),
p23 double(23,4),
p24 double(24,10),
p53 double(53,20),
r1  real,
-- r23  real(23,4),
-- r24  real(24,10),
-- r53  real(53,20),
-- dp1  double precision,
-- b0  bit(0),
-- b1  bit(1),
-- b64 bit(64),
d   date,
dt  datetime,
-- ts  timestamp,
-- t   time, 
-- y2  year(2),
-- y4  year(4),
-- c0  char(0),
c1  char(1),
c2  char(255),
-- vc0 varchar(0),
vc1 varchar(1),
vc2 varchar(255)
-- vb0 varbinary(0),
-- vb1 varbinary(1),
-- vb2 varbinary(255),
-- bi0 binary(0),
-- bi1 binary(1),
-- bi2 binary(255)
-- bl0 blob(0),
-- bl1 blob(1),
-- bl2 blob(255),
-- tx0 text(0),
-- tx1 text(1),
-- tx2 text(255),
-- e1 enum('a'),
-- e2 enum(' ', 'a'),
-- e3 enum(' ', 'a', 'abc'),
-- s1 set('a'),
-- s2 set(' ', 'a'),
-- s3 set(' ', 'a', 'abc')
) ENGINE=$ENGINE
";

sql_ok($sql, 'create fact table');

$sql = "select count(*) from $table;";
sql_ok($sql, 'row count');

$cols = array(
'i1','i2','i4','i8','i1u','i2u','i4u','i8u','d5','f1','f2','f23', 'f24','f53',
'p1','p2','p23','p24','p53','r1', 'd','dt','c1','c2','vc1','vc2');

// print_r($cols);

$vals = array(
"NULL",
#-128,
#-127,
-126,
126,
127,
128,
253,
#254,
#255,
"'255'",
"'-128'",
"1.0",
"-1.0",
"-5555.5555",
"-5555.55555",
"'-5555.5555'",
"'-5555.55555'",
"55555.555",
pow(2,32)-1,
-1 * (pow(2,32)-1),
pow(2,31)-1,
-1 * (pow(2,31)-1),
pow(2,32)-1 . "." . pow(2,32),
-1 * (pow(2,32)-1) . "." . pow(2,32),
"18446744073709551600",
"-18446744073709551600",
"9223372036854775800",
"-9223372036854775800",
"'0xdeadb33f'",
"'swany'",
"'????????????????????????????'", 
"'%'",
"12345678.1234567890",
"-12344567.1234567890",
"0.0000000001",
"0.0000000099",
"'*'",
"-1",
"'NOT NULL'",
"'1978_04_30'",
"'1978-04-30'",
"'1978-04-30 2:22:00'",
"'2:22:00'",
"'1978-04-30 2:22:00.12345'"
);
$i = 0;
foreach($vals as $val) {
	$val_list = "";
	++$i;	
	foreach($cols as $col) {
		if($val_list) $val_list .= ",";
		$val_list .= $val;
	}
	$sql = "INSERT INTO $table VALUES ($i,$val_list);";
#echo $sql . "\n";
	sql_ok($sql, 'insert data iteration #' . $i);
}

sql_ok("select count(*) from $table",'count rows');

#fix columnstore munged values (stupid sad columnstore can't handle MySQL data...)
sql_ok("update $table set dt=NULL where dt = '0000-00-00'",'adjust dt');
sql_ok("update $table set d=NULL where d = '0000-00-00'",'adjust d');
sql_ok("update $table set i4u = 4294967293 where i4u > 4294967293",'adjust i4u');
sql_ok("update $table set i2u = 65533 where i2u > 65533",'adjust i2u');
sql_ok("update $table set i1 = -126 where i1 < -126",'adjust i1');
sql_ok("update $table set i2 = -32766 where i2 < -32766",'adjust i2');
sql_ok("update $table set i1u = 253 where i1u > 253",'adjust i1u');
sql_ok("update $table set i8 = -9223372036854775806 where i8 < -9223372036854775806",'adjust i8');
sql_ok("update $table set i4 = -2147483646 where i4 < -2147483646",'adjust i4');

foreach($cols as $col) {
	for($i=1;$i<=count($vals);++$i) {
		sql_ok("select $col from $table where seq=$i",'verify data for ' . $col . " at seq $i");
	}
}

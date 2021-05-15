<?php
$sff=isset($_GET['sf']) ? 'sf2/'.$_GET['sf'] : dirname(__FILE__).'/file.sf2';
$conte=file_get_contents($sff);
$parts=explode("pdta",$conte);
if($parts){
	header("Content-Type: application/stream-octet");
	die($parts[1]);
}
die(404);
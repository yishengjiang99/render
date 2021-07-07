<?php
$buf=[];
$o=fopen("php://output","wb");
header("Content-Type: Application/xml");
$ap=popen("mkxml","r");
while(!feof($ap)){
	fwrite($o,fread($ap,1024));
}
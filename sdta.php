<?php
$sff=isset($_GET['sf']) ? 'sf2/'.$_GET['SF'] : 'file.sf2';

$p2=explode("sdtasmpl",file_get_contents($sff))[1] ;
if(!$p2)die(404);

$sdta = explode("pdta",file_get_contents($sff))[0];

header("content-type: audio/raw");
echo $sdta;
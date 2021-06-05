

<!DOCTYPE HTML>
<HEAD>
<script>

const url='/render/index.php?cmd=render';
const formData=new FormData();
formData.append('sss','$psesid');
var request = new XMLHttpRequest();
request.open("POST", url);
request.send(formData,true);
request.responseType='arrayBuffer';
document.onclick=(e)=>formData.append('clicked',[e.target.clientX, e.target.clientY, e.movementX, e.movementY]);
request.on('readyStateChange',()=>{
  if(request.response){
    reuest.response.array
  }
});
</script>
</HEAD>
<?php
$proc=proc_open("./a.out",
array(
  array("pipe","r"),
  array("pipe","w"),
  array("pipe","w")
),$pipes);


print stream_get_contents($pipes[1]);


?>

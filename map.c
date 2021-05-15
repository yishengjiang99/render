var memory = new WebAssembly.Memory({
	initial : 1024,
	maximum : 1024,
});
var heap = new Uint8Array(memory.buffer);

var brk = 4096; // stack top

var sbrk = function(size)
{
	var old = brk;
	brk += size;
	if (brk > heap.length)
	{
		memory.grow(Math.ceil((brk - heap.length) / 65536));
		heap = new Uint8Array(memory.buffer);
	}
	return old;
};
var imports = {
	env : {
		memory : memory,
		memcpy : function(d, s, n){
				heap.set(heap.subarray(s, s + n), d);
}
,
		pow : Math.pow,
		memset : (d, s, n) = > heap.set(heap.subarray(s, s + n), d),
										_abort : () = > {},
										_grow : () = >
{
	heap = new Uint8Array(memory.buffer);
}
,
		console_log : (... a) = > console.log(a),
}
,
}
;

export const sf2 = await fetch("sf2.c.wasm")
											 .then((response) = > response.arrayBuffer())
											 .then((bytes) = > WebAssembly.instantiate(bytes, imports))
											 .then(async({instance}) = >
																								 {
																									 const {a} = await fetch("pdta.php")
																																	 .then((res) = > res.arrayBuffer())
																																	 .then((ab) = >
																																								{
																																									const pdtamem = sbrk(ab.byteLength);
																																									heap.set(ab, pdtamem);
																																									instance.exports.sf2_load_from_mem(pdtamem, pdtamem.byteLength);
																																									//const ctxptr = instance.exports.init_ctx();
																																									const phdrs = instance.exports.phdrptr();
																																									return {
																																											phdrs,
																																											... instance.exports,
																																									};
																																								});
																								 });

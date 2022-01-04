module mux_byte #(parameter BITS = 32)
(
	input latch,
	input [1:0] addr,
	input [BITS - 1:0] src,
	output wire [7:0] dst
);

reg [31:0] src_latch;

always @(latch)
begin
	src_latch <= src;
end 

assign dst = 
/*
	( addr == 0 ) ? 8'h21 : //src_latch[7:0] :
	( addr == 1 ) ? 8'h43 : //src_latch[15:8] :
	( addr == 2 ) ? 8'h65 : //src_latch[23:16] :
	( addr == 3 ) ? 8'h87 : 8'h99; //src_latch[31:24]	: 8'b11111111;
*/
	( addr == 0 ) ? src_latch[7:0] :
	( addr == 1 ) ? src_latch[15:8] :
	( addr == 2 ) ? src_latch[23:16] :
	( addr == 3 ) ? src_latch[31:24]	:
	8'b11111111;
	
	
endmodule

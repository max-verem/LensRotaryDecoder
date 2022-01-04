module decoder1 #(parameter BITS = 32)
(
	input clk,
	input phase_a,
	input phase_b,
	output reg [BITS - 1:0] cnt
);
 
reg [3:0] state = 0;

always @(posedge clk)
begin
	
	//   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
 	// { 0, -1,  1,  0,  1,  0,  0, -1, -1,  0,  0,  1,  0,  1, -1,  0 };
	case(state)
		1,7,8,14: cnt <= cnt - 1'b1;
		2,4,11,13: cnt <= cnt + 1'b1;
	endcase
end

always @(negedge clk)
begin
	state[3] <= state[1];
	state[2] <= state[0];
	state[1] <= phase_a;
	state[0] <= phase_b;
end

 
endmodule

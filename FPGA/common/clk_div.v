module clk_div #(parameter DIV = 1)
(
	input clk_in,
	output reg [0:0] clk_out
);

// clog2 is 'ceiling of log base 2' which gives you the number of bits needed to store a value
parameter CNT_SIZE = $clog2(DIV / 2);

reg [CNT_SIZE:0] cnt = 0;

always @(posedge clk_in)
begin
	if (cnt == (DIV / 2))
	begin
		cnt <= 0;
		clk_out <= ~clk_out;
	end
	else
	begin
		cnt <= cnt + 1'b1;
		clk_out <= clk_out;
	end
end
endmodule

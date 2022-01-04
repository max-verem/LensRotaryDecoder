module uart_tx
(
	input clk,
	input [7:0] data,
	input start,
	output reg [0:0] tx,
	output reg [0:0] busy
);

reg [3:0] step;
reg [7:0] snd;
reg [0:0] done;

always @(posedge clk)
begin
	if(busy == 0)
	begin
		if(start == 1)
		begin
			snd <= data;
			busy <= 1;
			done <= 0;
			step <= 0;
			tx <= 1;
		end
		else
			tx <= 1;
	end
	else
	begin	// busy == 1
		if(done == 1)
		begin
			if(start == 0)
				busy <= 0;
		end
		else
		begin
			case(step)
				0: tx <= 1;
				1: tx <= 0;	// start bit
				2: tx <= snd[0];
				3: tx <= snd[1];
				4: tx <= snd[2];
				5: tx <= snd[3];
				6: tx <= snd[4];
				7: tx <= snd[5];
				8: tx <= snd[6];
				9: tx <= snd[7];
				default:
					begin
						tx <= 1;
						done <= 1;
						if(start == 0)
							busy <= 0;
					end
			endcase
			step <= step + 4'b1;
		end
	end
end
	
endmodule

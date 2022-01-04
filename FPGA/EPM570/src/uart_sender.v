module uart_sender #(parameter BITS = 32)
(
	input [BITS - 1:0] rot1_cnt,
	input [BITS - 1:0] rot2_cnt,
	input [BITS - 1:0] rot3_cnt,
	input clk_bytes_seq,
	input tx_busy,
	output reg [7:0] tx_data,
	output reg [0:0] tx_start
);

reg [BITS - 1:0] rot1_save;
reg [BITS - 1:0] rot2_save;
reg [BITS - 1:0] rot3_save;

reg [7:0] cs;
reg [9:0] tx_idx = 8'h0;
always @(posedge clk_bytes_seq)
begin
	if(tx_busy == 1)
		tx_start <= 0;
	else if(tx_start == 0)
	begin
		// calc CS
		if(tx_idx == 0)
		begin
			cs = 0;
			rot1_save <= rot1_cnt;
			rot2_save <= rot2_cnt;
			rot3_save <= rot3_cnt;
		end
		else
			cs = cs + tx_data;

		// send service bytes sequence
		case(tx_idx)
			0: begin tx_data <= 8'h5A; tx_start <= 1; end	// HEADER
			1: begin tx_data <= rot1_save[7:0]; 	tx_start <= 1; end	// DATAs
			2: begin tx_data <= rot1_save[15:8]; 	tx_start <= 1; end
			3: begin tx_data <= rot1_save[23:16]; 	tx_start <= 1; end
			4: begin tx_data <= rot2_save[7:0]; 	tx_start <= 1; end
			5: begin tx_data <= rot2_save[15:8]; 	tx_start <= 1; end
			6: begin tx_data <= rot2_save[23:16]; 	tx_start <= 1; end
			7: begin tx_data <= rot3_save[7:0]; 	tx_start <= 1; end
			8: begin tx_data <= rot3_save[15:8]; 	tx_start <= 1; end
			9: begin tx_data <= rot3_save[23:16]; 	tx_start <= 1; end
			10: begin tx_data <= cs; tx_start <= 1; end		// CS
		endcase
		
		if(tx_idx == 256)
			tx_idx <= 0;
		else
			tx_idx <= tx_idx + 1'b1;
	end
end

endmodule
	
	
	
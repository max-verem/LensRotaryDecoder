module mojo_top
(
    // 50MHz clock input
    input clk,

    // Input from reset button (active low)
    input rst_n,

    // cclk input from AVR, high when AVR is ready
    input cclk,

    // Outputs to the 8 onboard LEDs
    output[7:0]led,

    // AVR SPI connections
    output spi_miso,
    input spi_ss,
    input spi_mosi,
    input spi_sck,

    // AVR ADC channel select
    output [3:0] spi_channel,

	// rotary decoders connection
	input rot1_a,
	input rot1_b,
	input rot2_a,
	input rot2_b,
	input rot3_a,
	input rot3_b,

    // Serial connections
    input avr_tx, // AVR Tx => FPGA Rx
    output avr_rx, // AVR Rx <= FPGA Tx
    input avr_rx_busy // AVR Rx buffer full
);

wire rst = ~rst_n; // make reset active high

// these signals should be high-z when not used
assign spi_miso = 1'bz;

//assign avr_rx = 1'bz;
assign spi_channel = 4'bzzzz;

// ------------------------------------

wire clk_uart_baud;

clk_div #(.DIV(100)) clk_div_uart
(
	.clk_in(clk),
   .clk_out(clk_uart_baud)
);

wire tx_busy;
reg [7:0] tx_data;
reg [0:0] tx_start;

uart_tx uart_tx
(
	.clk(clk_uart_baud),
	.tx(avr_rx),
	.busy(tx_busy),
	.start(tx_start),
	.data(tx_data)
);

// ---------------------------------------

reg [23:0] rot1_save;
reg [23:0] rot2_save;
reg [23:0] rot3_save;

wire clk_bytes_seq;

clk_div #(.DIV(16)) clk_div_bytes
(
	.clk_in(clk_uart_baud),
   .clk_out(clk_bytes_seq)
);

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

// --------------------------------------

wire clk_led1;

assign led[0] = clk_led1;
assign led[1] = ~clk_led1;

clk_div #(.DIV(10000000)) clk_div_led1
(
	.clk_in(clk),
   .clk_out(clk_led1)
);

// ----------------------------------

wire clk_rot_dec;

clk_div #(.DIV(10)) clk_div_rot_dec
(
	.clk_in(clk),
   .clk_out(clk_rot_dec)
);

wire [23:0] rot1_cnt;

decoder1 #(.BITS(24)) rot1
(
	.clk(clk_rot_dec),
	.phase_a(rot1_a),
	.phase_b(rot1_b),
	.cnt(rot1_cnt)
);

wire [23:0] rot2_cnt;

decoder1 #(.BITS(24)) rot2
(
	.clk(clk_rot_dec),
	.phase_a(rot2_a),
	.phase_b(rot2_b),
	.cnt(rot2_cnt)
);

wire [23:0] rot3_cnt;

decoder1 #(.BITS(24)) rot3
(
	.clk(clk_rot_dec),
	.phase_a(rot3_a),
	.phase_b(rot3_b),
	.cnt(rot3_cnt)
);

assign led[2] = rot1_a;
assign led[3] = rot1_b;
assign led[4] = rot2_a;
assign led[5] = rot2_b;
assign led[6] = rot3_a;
assign led[7] = rot3_b;


endmodule
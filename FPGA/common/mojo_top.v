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

assign led[4] = 1'b0;
assign led[5] = 1'b0;
assign led[6] = 1'b0;
assign led[7] = 1'b0;

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
			cs = 0;
		else
			cs = cs + tx_data;

		// send service bytes sequence
		case(tx_idx)
			0: begin tx_data <= 8'h5A; tx_start <= 1; end	// HEADER
			1: begin tx_data <= 8'h31; tx_start <= 1; end	// DATAs
			2: begin tx_data <= 8'h32; tx_start <= 1; end
			3: begin tx_data <= 8'h33; tx_start <= 1; end
			4: begin tx_data <= 8'h34; tx_start <= 1; end
			5: begin tx_data <= 8'h35; tx_start <= 1; end
			6: begin tx_data <= 8'h36; tx_start <= 1; end
			7: begin tx_data <= 8'h37; tx_start <= 1; end
			8: begin tx_data <= 8'h38; tx_start <= 1; end
			9: begin tx_data <= 8'h39; tx_start <= 1; end
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

assign led[2] = 1'b0;
assign led[3] = 1'b0;

clk_div #(.DIV(10000000)) clk_div_led1
(
	.clk_in(clk),
   .clk_out(clk_led1)
);


endmodule
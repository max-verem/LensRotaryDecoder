
/************************************************************************************************************
I2C to GPIO Port expander. Date: December 21, 2006.
************************************************************************************************************/
`timescale 1us/10ns

/* Top module */

module I2C_to_GPIO ( sda, sclk, GPIO_input, GPIO_output);

inout sda;                        // Bidirectional SDA
input sclk;                       // Bidirectional SCLK
input [7:0] GPIO_input; 
output [7:0] GPIO_output;                                         
parameter slave_address = 7'h0;   // can be changed as per the protocol
parameter n = 8;

reg start_stop;        // Indicated if the device has detected a start and is busy in some operation
reg start, stop;       // Goes high temperarily when a start or stop is detectd
reg some_other_device; // to reset start_stop, when the adress on the bus is different from the device address;
reg repeat_start;      // to reset everything if repeat start is detected. 
reg write_flag;        // to set when write oper begins
reg reset;             // Used to reset the start , stop registers     
reg sda_out;           // To force the SDA output Low. Remains tristated for sending High Logic.
reg done;              // To Indicate data transfer done
reg [3:0]count;                          // Down Counter ( 7 to 0 + ack bit ) 
reg [n-1:0] GPIO_output;                 // parallel store
reg [n-1:0] GPIO_input_reg;              // Stores the Values at the input pins before sending to master.
reg data_or_address;                     // high for data, low for address
reg read_oper, add_is_matching;          // high for master read; high for address matching


/********************************************************************************************************/

/* Detecting STOP Condition on the SDA Bus */
always @ (posedge sda or posedge reset)
if (reset) begin
    stop <= 0;
end else if (sclk) begin
    stop <= 1;
end


/* Detecting START Condition on the SDA Bus */
always @ (negedge sda or posedge reset)
if (reset) begin
    start <= 0;
end else if (sclk) begin
    start <= 1;
end


/* Tracking START_STOP related status of the device */
always @ (negedge sclk)
 if (start_stop & start) begin
     repeat_start <= 1; 
     reset <= 1;
 end else if (start) begin
     start_stop <= 1;
     reset <= 1;
     repeat_start <= 0; 
 end else if (stop) begin
     start_stop <= 0;
     reset <= 1;
     repeat_start <= 0;
 end else if (some_other_device) begin
     start_stop <= 0; 
     repeat_start <= 1;
 end else begin
     reset <= 0;
     repeat_start <= 0;  
 end

/***********************************************************************************************************/   

/* Counting of bits. First eight bits are data and the last one is ack. */ 
wire sda_is_ack = count[3] & count [2];  // High for ack , count = 1111

/* Down Counter, 1111 is ack */
always @ (negedge sclk)
if (start) begin
    count <= 4'h7;               // it would decreae to 7 on the first pos edge of sclk. 
end
else if (start_stop) begin
         if (sda_is_ack) begin   // count is restored to 7
            count <= 4'h7;
         end else begin
         count <= count - 4'h1;  // count is decremented until zero. 
         end
end 


/***********************************************************************************************************/
/* Taking the Inputs from SDA line and Comparing with Address */

always @(posedge sclk)
   if (start_stop) begin
      if (~sda_is_ack) begin
          if (~data_or_address | repeat_start) begin
              if (repeat_start) begin
              add_is_matching <= 1;               // high means matching
              data_or_address <= 0;               // default is address
              some_other_device <= 0;             // High means that device address does not match with SDA data 
              write_flag <= 0;                    
              done <= 0;                          // Low means Transfer is pending
              end
              if (count == 7 & sda != slave_address[6]) add_is_matching <= 0;  
              if (count == 6 & sda != slave_address[5]) add_is_matching <= 0;  
              if (count == 5 & sda != slave_address[4]) add_is_matching <= 0;  
              if (count == 4 & sda != slave_address[3]) add_is_matching <= 0;  
              if (count == 3 & sda != slave_address[2]) add_is_matching <= 0;  
              if (count == 2 & sda != slave_address[1]) add_is_matching <= 0;  
              if (count == 1 & sda != slave_address[0]) add_is_matching <= 0;  
              if (count == 0 & ~sda )begin
              read_oper <= 0;
              data_or_address <= 1; 
              end else if (count == 0) begin 
              read_oper <= 1;
              data_or_address <= 1;              
              end
           
          end else if (~read_oper & ~done) begin               
               if (count == 7) GPIO_output[7] <= sda;
               if (count == 6) GPIO_output[6] <= sda;
               if (count == 5) GPIO_output[5] <= sda;
               if (count == 4) GPIO_output[4] <= sda;
               if (count == 3) GPIO_output[3] <= sda;
               if (count == 2) GPIO_output[2] <= sda;
               if (count == 1) GPIO_output[1] <= sda;
               if (count == 0) begin
                   GPIO_output[0] <= sda;
                   done <= 1;    
               end
          end else begin
               if (count == 0) done <= 1;
          end   

       end else begin 
          if (add_is_matching) begin
              if (read_oper) write_flag <= 1;   
          end else begin
              data_or_address <= 0;
              some_other_device <= 1;
          end
             
       end   
    
    end else begin
    add_is_matching <= 1;  // high means matching
    data_or_address <= 0;  // default is address
    some_other_device <= 0;
    write_flag <= 0;
    done <= 0;
    end 

/***********************************************************************************************************/

/* Writing ACK and DATA on the SDA Line*/

wire ack_flag = (count[3]|count[2]|count[1]|count[0]); //becomes low when count is 0

always@(negedge sclk)
    if (start_stop) begin
        if (~ack_flag) begin                              
            if ( add_is_matching  & ~write_flag)begin sda_out <= 1'b0; 
            end else begin sda_out <= 1'bz;
            end
            if (read_oper & ~done) GPIO_input_reg <= GPIO_input;
        end else if (read_oper & data_or_address & ~done) begin
             if (ack_flag) begin 
                 if (~GPIO_input_reg[7]) begin
                     sda_out <= 1'b0;
                 end else begin 
                     sda_out <= 1'bz;
                 end
                 GPIO_input_reg <= GPIO_input_reg << 1;                    
             end
        end else begin
             sda_out <= 1'bz;
        end
    end

wire sda;

assign sda = sda_out;

endmodule
